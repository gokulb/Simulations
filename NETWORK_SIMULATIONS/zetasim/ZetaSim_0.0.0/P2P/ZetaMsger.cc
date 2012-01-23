/* ZetaMsger.cc
 * Jing Wang
 * June, 2010

ZetaSim - A NS2 based Simulation suit for Zeta Protocol
Copright (C) 2010 Jing Wang

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Jing Wang
Department of Electronics and Information Engineering
Huazhong University of Science and Technology
Wuhan, Hubei, 430074
hbhzwj@gmail.com

*/
#include "ZetaMsger.h"
#include "ZetaApp.h"

Port_t port = ( Port_t ) 100;
extern AddrNameMap_t addrNameMap;

extern void PrintCodeWord ( CodeWord_t word );
extern void PrintCodeWord ( CodeWord_t word, char op );
extern void PrintCodeWord ( CodeWord_t* wordP, int n );
extern void PrintCoef ( Matrix mat );

static class ZetaMsgerAppClass : public TclClass
{
public:
        ZetaMsgerAppClass() : TclClass ( "Agent/ZetaMsger" ) {}
        TclObject* create ( int, const char* const* ) {
                return ( new ZetaMsger() );
        }
} class_zeta_msger;


ZetaMsger::ZetaMsger ( ZetaApp* app ) : P2PMsger()
{
        app_ = app;
        firewalled_ = false;
        pktHandler_ = new ZetaPacket ( this );
        udpSock_ = NULL;
}

ZetaMsger::ZetaMsger ( ZetaApp* app, Socket* a ) : P2PMsger()
{
        app_ = app;
        firewalled_ = false;
        udpSock_ = ( Socket* ) a;
        udpSock_->SetMsger ( this );
        pktHandler_ = new ZetaPacket ( this );
}

ZetaMsger::~ZetaMsger()
{
        delete pktHandler_;
}

void ZetaMsger::ConnectBootServer()
{
        debug ( DEBUG, "ZetaMsger::ConnectBootServer()\n" );

        AddrSet_t *bootServerSetP = app_->GetbootServerSetP();
        // Check whether there is cached Boot Server
        if ( bootServerSetP->empty() ) {
                std::cout << "Unable to boot, there is no boot server" << std::endl;
        } else {
                AddrSet_t::iterator ai;
                for ( ai = bootServerSetP->begin(); ai != bootServerSetP->end(); ai++ ) {
                        debug ( DEBUG, "The addr of boot server: %d \n", *ai );

                        PacketData* pktData = pktHandler_->NewPacketDataConnectBootServer ( app_->alias_, app_->GetMaxUploadSpeed() );
                        SendMsg ( *ai, port, CONNECT_BOOT_SERVER, pktData );  // BootServer user famous port. defined in line 9
                }
        }
}

void ZetaMsger::ConnectBootServerReply ( NodeAddr_t addr, Port_t port )
{
        PacketData* pktData = pktHandler_->NewPacketDataConnectBootServerReply ( app_->alias_ );
        SendMsg ( addr, port, CONNECT_BOOT_SERVER_REPLY, pktData );
}

void ZetaMsger::UpdateSharedFilesAck ( NodeAddr_t addr, Port_t port )
{
        PacketData* pktData = pktHandler_->NewPacketDataUpdateSharedFilesAck();
        SendMsg ( addr, port, UPDATE_SHARED_FILES_ACK, pktData );
}

void ZetaMsger::SelectTracker ( FileTag_t tag,  NodeAddr_t addr, Port_t port, NodeInfoEntry_t ap )
{
        PacketData *pktData = pktHandler_->NewPacketDataSelectTracker ( tag, ap );
        SendMsg ( addr, port, SELECT_TRACKER, pktData );
}

void ZetaMsger::RequestStartSeeder ( FileTag_t tag, NodeAddr_t addr, Port_t port, NodeInfoEntry& tracker )
{
        PacketData *pktData = pktHandler_->NewPacketDataRequestStartSeeder ( tag, tracker );
        SendMsg ( addr, port, REQUEST_START_SEEDER, pktData );
}

void ZetaMsger::ConnectTrackerAck ( FileTag_t tag, NodeAddr_t addr, Port_t port, Parameter para )
{
        //PacketData *pktData = pktHandler_->NewPacketData("ConnectTrackerAck");

        PacketData *pktData = pktHandler_->NewPacketDataConnectTrackerAck ( tag, para );
        SendMsg ( addr, port, CONNECT_TRACKER_ACK, pktData );
}

void ZetaMsger::UpdatePeers ( NodeAddr_t addr, Port_t port, FileTag_t tag )
{
        PacketData *pktData = pktHandler_->NewPacketDataUpdatePeers ( tag );
        SendMsg ( addr, port, UPDATE_PEERS, pktData );
}

void ZetaMsger::UpdatePeersReply ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeSet_t peerSet )
{
        debug ( DEBUG, "ZetaMsger::UpdatePeersReply " );
        NodeSet_t::iterator ni;
// 		printf("peerSet Addr ");
// 		for (ni = peerSet.begin(); ni != peerSet.end(); ni++ ) {
// 			printf(" %d ", ni->addr_);
// 		}
// 		printf("\n");
        PacketData *pktData = pktHandler_->NewPacketDataUpdatePeersReply ( tag, peerSet );
        SendMsg ( addr, port, UPDATE_PEERS_REPLY, pktData );
}

void ZetaMsger::SelectTrackerAck ( FileTag_t tag, NodeAddr_t addr, Port_t port )
{
        PacketData *pktData = pktHandler_->NewPacketDataSelectTrackerAck ( tag );
        SendMsg ( addr, port, SELECT_TRACKER_ACK, pktData );
}

void ZetaMsger::QueryNotHit ( NodeAddr_t addr, Port_t port )
{
        PacketData *pktData = pktHandler_->NewPacketDataQueryNotHit();
        SendMsg ( addr, port, QUERY_NOT_HIT, pktData );
}

void ZetaMsger::DisConnectBootServer ( NodeAddr_t addr, std::string reason )
{
        debug ( DEBUG, "ZetaMsger::DisConnectBootServer\n" );
        //pktHandler_->SetReason ( reason );
        PacketData* pktData = pktHandler_->NewPacketDataDisConnectBootServer(reason);
        SendMsg ( addr, port, DISCONNECT_BOOT_SERVER, pktData );
}

void ZetaMsger::UpdateSharedFiles()
{
        debug ( DEBUG, \
                "Node [%s] wants to update shared files to its boot server Node [%s]\n",
                app_->alias_.c_str(),
                addrNameMap.find ( app_->GetbootServerP()->addr_ )->second.alias_.c_str() );
        PacketData *pktData = pktHandler_->NewPacketDataUpdateSharedFiles ( app_->GetSharedFileMapP(), REPLACE );
        SendMsg ( app_->GetbootServerP()->addr_, port, UPDATE_SHARED_FILES, pktData );
}

void ZetaMsger::Query ( FileTag_t tag )
{
        pktHandler_-> SetFileTag ( tag );
        PacketData *pktData = pktHandler_->NewPacketDataQuery();
	debug ( DEBUG, "ZetaMsge::Query boot server addr: %d \n", app_->GetbootServerP()->addr_);
        SendMsg ( app_->GetbootServerP()->addr_, port, QUERY, pktData );
}

void ZetaMsger::QueryHit ( FileTag_t tag, NodeAddr_t addr, Port_t port, NodeInfoEntry_t ap )
{
        pktHandler_->SetTracker ( ap );
        pktHandler_->SetFileTag ( tag );
        PacketData *pktData = pktHandler_->NewPacketDataQueryHit();
        SendMsg ( addr, port, QUERY_HIT, pktData );
}

void ZetaMsger::ConnectTracker ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeState_t state, std::string name, int maxUploadSpeed, int nodeType )
{
        pktHandler_->SetFileTag ( tag );
        NodeInfoEntry *localInfo  = app_->GetLocalInfo();
        PacketData *pktData = pktHandler_->NewPacketDataConnectTracker ( *localInfo, nodeType );
        delete localInfo;
        SendMsg ( addr, port, CONNECT_TRACKER, pktData );
}

void ZetaMsger::SendLinearComb ( FileTag_t tag, NodeAddr_t addr, Port_t port, Block *blk, Parameter para )
{
        NodeInfoEntry *ni = app_->GetLocalInfo();
        PacketData *pktData = pktHandler_->NewPacketDataLinearComb ( tag, addr, port, blk, para, ni );
        delete ni;
        SendMsg ( addr, port, FILE_TRANS, pktData );
}

void ZetaMsger::HasFinishedDownloading ( FileTag_t tag, NodeAddr_t desAddr, Port_t desPort )
{
        PacketData *pktData = pktHandler_->NewPacketDataHasFinishedDownloading ( tag );
        SendMsg ( desAddr, desPort, HAS_FINISHED_DOWNLOADING, pktData );
}

void ZetaMsger::SendMsg ( NodeAddr_t addr, Port_t port, MsgType_t msg, PacketData *pktData )
{
//     debug(DEBUG, "Node [%s] Send Message: %s to Node [%s]\n", \
//           app_->alias_.c_str(), \
//           msg.c_str(), \
//           addrNameMap.find(addr)->second.alias_.c_str());

        Packet* pkt = pktHandler_->NewPacket ( msg, udpSock_->addr(), pktData );
        udpSock_->SendPktTo ( pkt, addr, port );
}

int ZetaMsger::upcall_recv ( Socket *sock, Packet *pkt )
{

        // Wait to be finished, Check the status of ZetaApplication
        //
        debug ( DEBUG, "ZetaMsger::upcall_recv()\n" );
        Dispatch ( sock, pkt );
        int size = pkt->userdata()->size();
        Packet::free(pkt);
        return size;
}

void ZetaMsger::Dispatch ( Socket *sock, Packet *pkt )
{
        // Get the Header of the Packet.
        MsgType_t msg = pktHandler_->Parse ( pkt );
        // Dispatch the Packet According to the Message Type
        NodeAddr_t srcAddr = pktHandler_->GetSrcAddr ( pkt );
        Port_t srcPort = pktHandler_->GetSrcPort ( pkt );
        ExInfo_t *info = NULL;
        //debug(DEBUG, "the srcAddr in IP header srcAddr: %d\n", srcAddr);
        //debug(DEBUG, "msg: %d\n", msg);
        int blockSize, blockNumInData, codeWordSize;
        BYTE *dataptr = NULL, *blkData = NULL, *blkCoef = NULL;
        Block *blk = NULL;


        switch ( msg ) {
        case NO_TYPE:
                debug ( DEBUG, "Node [%s] received a Packet from Node [%s] with type: NO_TYPE\n", \
                        app_->alias_.c_str(), \
                        addrNameMap.find ( srcAddr )->second.alias_.c_str() ); // only for test
                break;

        case CONNECT_BOOT_SERVER:
                debug ( DEBUG, "Node %s Received a Packet from Node [%s] with type: CONNECT_BOOT_SERVER\n", \
                        app_->alias_.c_str(), \
                        addrNameMap.find ( srcAddr )->second.alias_.c_str() ); // only for test

                info = pktHandler_->GetExtendInfo ( CONNECT_BOOT_SERVER,
                                                    pkt->datalen(),
                                                    ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvConnectRequest ( srcAddr,
                                                  port,
                                                  info->connectBootServer_->name_,
                                                  info->connectBootServer_->maxUploadSpeed_ );
                break;

        case DISCONNECT_BOOT_SERVER:
                debug ( DEBUG,
                        "Node [%s] Received a Packet with from Node [%s] type: DISCONNECT_BOOT_SERVER\n",
                        app_->alias_.c_str(),
                        addrNameMap.find ( srcAddr )->second.alias_.c_str() ); // only for test
                app_->upcall_RecvDisconnectRequest ( srcAddr );
                break;

        case CONNECT_BOOT_SERVER_REPLY:
                debug ( DEBUG, "Node [%s] Received a Packet from Node [%s] with type: CONNECT_BOOT_SERVER_REPLY\n", \
                        app_->alias_.c_str(), \
                        addrNameMap.find ( srcAddr )->second.alias_.c_str() );

                info = pktHandler_->GetExtendInfo ( CONNECT_BOOT_SERVER_REPLY, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_ConnectBootServer ( info->connectBootServerReply_->name_, srcAddr );
                break;

        case UPDATE_SHARED_FILES:
                debug ( DEBUG, "Node [%s] Received a Packet from Node [%s] with type: UPDATE_SHARED_FILES\n", \
                        app_->alias_.c_str(), \
                        addrNameMap.find ( srcAddr )->second.alias_.c_str() ); // only for test
                info = pktHandler_->GetExtendInfo ( UPDATE_SHARED_FILES, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                //debug(DEBUG, "------------------------\n");
                //debug(DEBUG, "info->updateSharedFile_->op %d\n", info->updateSharedFile_->op);
                //x = info->updateSharedFile_->sharedFiles_;
                //debug(DEBUG, "++++++++++++++++++++++++\n");
                app_->upcall_RecvUdpateSharedFilesRequest ( srcAddr,
                                srcPort,
                                info->updateSharedFile_->op,
                                info->updateSharedFile_->sharedFiles_,
                                info->updateSharedFile_->fileNum );
                break;

        case UPDATE_SHARED_FILES_ACK:
                app_->upcall_UpdateSharedFilesAck();
                break;

        case QUERY:
                info = pktHandler_->GetExtendInfo ( QUERY, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvQuery ( srcAddr, srcPort, info->query_->tag );
                break;

        case QUERY_NOT_HIT:
                app_->upcall_QueryNotHit ( srcAddr, srcPort );
                break;

        case QUERY_HIT:
                debug ( DEBUG, "Node [%s] QueryHit\n", app_->alias_.c_str() );
                info = pktHandler_->GetExtendInfo ( QUERY_HIT, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                //app_->upcall_QueryHit(info->queryHit_->tag_,
                //                      info->queryHit_->addrArr_,
                //                      info->queryHit_->addrNum_);

                app_->upcall_QueryHit ( info->queryHit_->tag_, info->queryHit_->tracker_ );
                break;

        case CONNECT_TRACKER:
                debug ( DEBUG, "ZetaMsger::Dispatch, Message: CONNECT_TRACKER\n" );
                info = pktHandler_->GetExtendInfo ( CONNECT_TRACKER, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_ConnectTracker ( srcAddr,
                                              srcPort,
                                              info->connectTracker_->tag_,
                                              info->connectTracker_->info_,
                                              info->connectTracker_->nodeType_ );
                break;

        case SELECT_TRACKER:
                info = pktHandler_->GetExtendInfo ( SELECT_TRACKER, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvSelectTracker ( info->selectTracker_->tag_, info->selectTracker_->bserver_ );
                break;
        case SELECT_TRACKER_ACK:
                info = pktHandler_->GetExtendInfo ( SELECT_TRACKER_ACK, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvSelectTrackerAck ( srcAddr, srcPort,info->selectTrackerAck_->tag_ );
                break;

        case CONNECT_TRACKER_ACK:
                debug ( DEBUG, "in ZetaMsger.cc line 233\n" );
                info = pktHandler_->GetExtendInfo ( CONNECT_TRACKER_ACK, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvConnectTrackerAck ( info->connectTrackerAck_->tag_,  info->connectTrackerAck_->para_ );
                break;

        case REQUEST_START_SEEDER:
                info = pktHandler_->GetExtendInfo ( REQUEST_START_SEEDER, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvRequestStartSeeder ( info->requestStartSeeder_->tag_, info->requestStartSeeder_->tracker_ );
                break;

        case UPDATE_PEERS:
                info = pktHandler_->GetExtendInfo ( UPDATE_PEERS, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvUpdatePeers ( srcAddr, srcPort, info->updatePeers_->tag_ );
                break;

        case UPDATE_PEERS_REPLY:
                info = pktHandler_->GetExtendInfo ( UPDATE_PEERS_REPLY, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                app_->upcall_RecvUpdatePeersReply (
                        srcAddr,
                        srcPort,
                        info->updatePeersReply_->tag_,
                        info->updatePeersReply_->peerSet_,
                        info->updatePeersReply_->num );
                break;

        case HAS_FINISHED_DOWNLOADING:
                info = pktHandler_->GetExtendInfo ( HAS_FINISHED_DOWNLOADING, pkt->datalen(), ( PacketData* ) pkt->userdata() );
                app_->upcall_RecvFinishedDownloadMsg ( srcAddr, srcPort, info->finishedDownloading_->tag_ );
                break;

        case FILE_TRANS:
                debug ( DEBUG, "ZetaMsger::Dispatch() case FILE_TRANS\n" );
                info = pktHandler_->GetExtendInfo ( FILE_TRANS, pkt->datalen(), ( PacketData * ) pkt->userdata() );
                PacketData *pktData = ( PacketData * ) pkt->userdata();
                dataptr = ( BYTE * ) pktData->data();
                blockSize = info->fileTrans_->para_.blockSize;
                blockNumInData = info->fileTrans_->para_.blockNumInData;
                codeWordSize = info->fileTrans_->para_.codeWordSize;
                //blkData = ( BYTE * ) malloc ( blockSize );
                //memcpy ( blkData, dataptr + sizeof ( ExInfo_FILE_TRANS_t ), blockSize );
                //blkCoef = ( BYTE * ) malloc ( codeWordSize * blockNumInData );
                //memcpy ( blkCoef, dataptr + sizeof ( ExInfo_FILE_TRANS_t ) + blockSize, codeWordSize * blockNumInData );
		blkData = dataptr + sizeof(ExInfo_FILE_TRANS_t);
		blkCoef = blkData + blockSize;
                blk = new Block();
                blk->data_ = ( CodeWord_t * ) blkData;
                blk->coef_ = ( CodeWord_t * ) blkCoef;
                app_->upcall_RecvFileTransBlock ( info->fileTrans_->tag_,
                                                  info->fileTrans_->para_,
                                                  info->fileTrans_->nodeInfo_,
                                                  blk );
                break;
        }

//         if ( info ) {
//                 //debug(DEBUG, "delete info created by Msg: [%d]\n", msg);
//                 delete info;
//                 //debug(DEBUG, "has deleted info created by Msg: [%d]\n", msg);
//         }
}

void ZetaMsger::SetApp ( ZetaApp *app )
{
        app_ = app;
}

void ZetaMsger::SetFirewalled ( bool firewalled )
{
        firewalled_ = firewalled;
}

int ZetaMsger::command ( int argc, const char*const* argv )
{
        if ( strcmp ( argv[1], "attach_app" ) == 0 ) {
                Tcl &tcl = Tcl::instance();
                ZetaApp *app = ( ZetaApp * ) tcl.lookup ( argv[2] );
                if ( app == NULL ) {
                        return TCL_ERROR;
                } else {
                        SetApp ( app );
                        return TCL_OK;
                }
        } else if ( strcmp ( argv[1], "firewalled" ) == 0 ) {
                if ( strcmp ( argv[2], "true" ) == 0 ) {
                        SetFirewalled ( true );
                        return TCL_OK;
                } else if ( strcmp ( argv[2], "false" ) == 0 ) {
                        SetFirewalled ( false );
                        return TCL_OK;
                } else {
                        return TCL_ERROR;
                }
        } else {
                return P2PMsger::command ( argc, argv );
        }
}

void ZetaMsger::debug ( DebugLevel level, const char *fmt, ... )
{

        if ( level > app_->debugLevel )
                return;
        switch ( level ) {
        case ERROR:
                printf ( "[ERROR in ZetaMsger]" );
                break;
        case STAT:
                printf ( "[STAT in ZetaMsger]" );
                break;
        case WARNING:
                printf ( "[WARNING in ZetaMsger]" );
                break;
        case INFO:
                printf ( "[INFO in ZetaMsger]" );
                break;
        case DEBUG:
                printf ( "[DEBUG in ZetaMsger]" );
                break;
        }
        printf ( " "TIME_FORMAT"s ", Scheduler::instance().clock() );
        va_list ap;
        va_start ( ap, fmt );
        vprintf ( fmt, ap );
}
