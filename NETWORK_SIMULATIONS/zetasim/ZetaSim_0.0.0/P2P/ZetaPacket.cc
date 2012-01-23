/* ZetaPacket.cc
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
#include "ZetaPacket.h"
//#include <climits>
#include <iostream>
#include <string.h>

#define INT_MAX 100000000 // for test
#include "ZetaMsger.h"
#include "ZetaApp.h"
#include "TypeDefine.h"

extern void PrintCodeWord ( CodeWord_t word );
extern void PrintCodeWord ( CodeWord_t word, char op);
extern void PrintCodeWord ( CodeWord_t* wordP, int n );
extern void PrintCoef ( Matrix mat );

int hdr_zeta::offset_ ;
static class ZetaHeaderClass : public PacketHeaderClass
{
public:
        ZetaHeaderClass() : PacketHeaderClass ( "PacketHeader/Zeta",
                                                                sizeof ( hdr_zeta ) ) {
                bind_offset ( &hdr_zeta::offset_ );
        }
} class_zetahdr;



ZetaPacket::ZetaPacket ( ZetaMsger* msger ) : Agent ( PT_ZETA ), seq1_ ( 0 ), seq2_ ( 0 ), seq3_ ( 0 )
{
        msger_ = msger;
}

NodeAddr_t ZetaPacket::GetSrcAddr ( Packet *pkt )
{
        //hdr_zeta* h = hdr_zeta::access(pkt);
        hdr_ip *h2 = hdr_ip::access ( pkt );
        //debug(DEBUG, "Zeta src addr: %d \t ip src: %d \n\n", h->GetSrcAddr(),  h2->saddr());
        return h2->saddr();
}
NodeAddr_t ZetaPacket::GetSrcPort ( Packet *pkt )
{
        hdr_ip *h = hdr_ip::access ( pkt );
        return h->sport();
}

Packet * ZetaPacket::NewPacket ( MsgType_t type, NodeAddr_t addr, PacketData *data )
{
        Packet* pkt = allocpkt();
        hdr_zeta *hdr = hdr_zeta::access ( pkt );
        hdr->type_ = type;
        hdr->addr_ = addr;

        BYTE* id = NewMsgID();
        if ( id == NULL ) {
                std::cout << "failed to create new message ID" << std::endl;
                return NULL;
        } else {
                memcpy ( &hdr->id_, id, sizeof ( MsgID_t ) );
        }
        pkt->setdata ( data );
        return pkt;
}

PacketData *ZetaPacket::NewPacketData ( std::string str )
{
        PacketData *userdata = new PacketData ( str.size() + 1 ); // Attention About Memery Linakage
        BYTE *dataptr = (BYTE *) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        debug(DEBUG, "ZetaPacket::line66\n");
        //std::cout << str.data() << std::endl;
        memcpy ( dataptr, str.data(), str.size() );
	memcpy( dataptr + str.size(), &"\0", 1);
	debug(DEBUG, "ZetaPacket::NewPacketData()\n");
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataConnectBootServer ( std::string str, int maxUploadSpeed )
{
        //return NewPacketData(str);
        ExInfo_CONNECT_BOOT_SERVER_t info;
        if ( str.size() + 1 > MAX_STRING_LEN ) {
                debug ( ERROR, "MAX_STRING_LEN is to small\n" );
                return NULL;
        }
        memcpy ( &info.name_[0], str.data(), str.size() );
        info.name_[ str.size() ] = '\0';
        info.maxUploadSpeed_ = maxUploadSpeed;

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_CONNECT_BOOT_SERVER_t ) );
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        memcpy ( dataptr, &info, sizeof ( ExInfo_CONNECT_BOOT_SERVER_t ) );
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataDisConnectBootServer(std::string reason)
{
	ExInfo_DISCONNECT_BOOT_SERVER_t info;
        if ( reason.size() + 1 > MAX_STRING_LEN ) {
                debug ( ERROR, "MAX_STRING_LEN is to small\n" );
                return NULL;
        }
        memcpy ( &info.reason_[0], reason.data(), reason.size() );
        info.reason_[ reason.size() ] = '\0';

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_DISCONNECT_BOOT_SERVER_t ) );
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        memcpy ( dataptr, &info, sizeof ( ExInfo_DISCONNECT_BOOT_SERVER_t ) );
        return userdata;
        //return NewPacketData(str);
//         ExInfo_DISCONNECT_BOOT_SERVER_t info;
//         if ( reason_.size() + 1 > MAX_STRING_LEN ) {
//                 debug ( ERROR, "MAX_STRING_LEN is to small\n" );
//                 return NULL;
//         }
//         memcpy ( &info.reason_[0], reason_.data(), reason_.size() );
//         info.reason_[ reason_.size() ] = '\0';
// 
//         PacketData* userdata = new PacketData ( sizeof ( ExInfo_DISCONNECT_BOOT_SERVER_t ) );
//         BYTE *dataptr = ( BYTE * ) userdata->data();
//         if ( dataptr == NULL ) {
//                 return NULL;
//         }
//         memcpy ( dataptr, &info, sizeof ( ExInfo_DISCONNECT_BOOT_SERVER_t ) );
//         return userdata;

}

PacketData *ZetaPacket::NewPacketDataConnectBootServerReply ( std::string str )
{
        //return NewPacketData(str);
        ExInfo_CONNECT_BOOT_SERVER_REPLY_t info;
        if ( str.size() + 1 > MAX_STRING_LEN ) {
                debug ( ERROR, "MAX_STRING_LEN is to small\n" );
                return NULL;
        }
        memcpy ( &info.name_[0], str.data(), str.size() );
        info.name_[ str.size() ] = '\0';

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_CONNECT_BOOT_SERVER_REPLY_t ) );
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        memcpy ( dataptr, &info, sizeof ( ExInfo_CONNECT_BOOT_SERVER_REPLY_t ) );
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataUpdateSharedFiles ( FileMap_t* fm, FileOperation_t op )
{
        ExInfo_UPDATE_SHARED_FILES_t info;
        info.op = op;
        FileMap_t::iterator fi;
        int i = 0;
        for ( fi = fm->begin(); fi != fm->end(); fi++ ) {
                FileTag_t tag = fi->first;
                memcpy ( &info.sharedFiles_[0] + i * sizeof ( FileTag_t ), &tag, sizeof ( FileTag_t ) );
                i++;
		if ( i == MAX_FILE_NUM ) {
			debug(WARNING, "MAX_FILE_NUM is too small\n");
			break;
		}
        }
        info.fileNum = i;
        //debug(DEBUG, "info.sharedFiles_.max_size() %d\n", info.sharedFiles_.max_size());
        PacketData* userdata = new PacketData ( sizeof ( ExInfo_UPDATE_SHARED_FILES_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        //debug(DEBUG, "sizeof(ExInfo_UPDATE_SHARED_FILES_t): %d\n", sizeof(ExInfo_UPDATE_SHARED_FILES_t));
        memcpy ( dataptr, &info, sizeof ( ExInfo_UPDATE_SHARED_FILES_t ) ); // be aware of size.
        //debug(DEBUG, "userdata->size():\t %d\n", userdata->size());
        return userdata;
}


PacketData *ZetaPacket::NewPacketDataUpdateSharedFilesAck()
{
	debug(DEBUG, "ZetaPacket::NewPacketDataUpdateSharedFilesAck\n");
        return NewPacketData ( "UpdateSharedFilesAck" );
}

PacketData *ZetaPacket::NewPacketDataQuery()
{
        ExInfo_QUERY_t info;
        info.tag = tag_;

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_QUERY_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        //debug(DEBUG, "sizeof(ExInfo_UPDATE_SHARED_FILES_t): %d\n", sizeof(ExInfo_QUERY_t));
        memcpy ( dataptr, &info, sizeof ( ExInfo_QUERY_t ) ); // be aware of size.
        //debug(DEBUG, "userdata->size():\t %d\n", userdata->size());
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataQueryHit()
{
        ExInfo_QUERY_HIT_t info;
        info.tracker_ = tracker_;
        info.tag_ = tag_;

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_QUERY_HIT_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        memcpy ( dataptr, &info, sizeof ( ExInfo_QUERY_HIT_t ) ); // be aware of size.
        return userdata;

}

PacketData *ZetaPacket::NewPacketDataConnectTracker(NodeInfoEntry pi,  int nodeType)
{
        ExInfo_CONNECT_TRACKER_t info;
        info.tag_ = tag_;
	info.info_ = pi;
	info.nodeType_ = nodeType;

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_CONNECT_TRACKER_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        memcpy ( dataptr, &info, sizeof ( ExInfo_CONNECT_TRACKER_t ) ); // be aware of size.
        return userdata;

}


PacketData *ZetaPacket::NewPacketDataSelectTracker ( FileTag_t tag, NodeInfoEntry_t ap )
{
        ExInfo_SELECT_TRACKER_t info;
        info.tag_ = tag;
        info.bserver_ = ap;

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_SELECT_TRACKER_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        memcpy ( dataptr, &info, sizeof ( ExInfo_SELECT_TRACKER_t ) ); // be aware of size.
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataSelectTrackerAck ( FileTag_t tag )
{
        ExInfo_SELECT_TRACKER_ACK_t info;
        info.tag_ = tag;

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_SELECT_TRACKER_ACK_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        memcpy ( dataptr, &info, sizeof ( ExInfo_SELECT_TRACKER_ACK_t ) ); // be aware of size.
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataConnectTrackerAck(FileTag_t tag, Parameter para)
{
        ExInfo_CONNECT_TRACKER_ACK_t info;
        info.tag_ = tag;
	info.para_ = para;

        PacketData* userdata = new PacketData ( sizeof ( ExInfo_CONNECT_TRACKER_ACK_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        memcpy ( dataptr, &info, sizeof ( ExInfo_CONNECT_TRACKER_ACK_t ) ); // be aware of size.
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataRequestStartSeeder(FileTag_t tag, NodeInfoEntry& tracker) {
	ExInfo_REQUEST_START_SEEDER_t info;
	info.tag_ = tag;
	info.tracker_ = tracker;

	PacketData* userdata = new PacketData ( sizeof ( ExInfo_REQUEST_START_SEEDER_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        memcpy ( dataptr, &info, sizeof ( ExInfo_REQUEST_START_SEEDER_t ) ); // be aware of size.
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataUpdatePeers(FileTag_t tag) {
	ExInfo_UPDATE_PEERS_t info;
	info.tag_ = tag;

	PacketData* userdata = new PacketData ( sizeof ( ExInfo_UPDATE_PEERS_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        memcpy ( dataptr, &info, sizeof ( ExInfo_UPDATE_PEERS_t ) ); // be aware of size.
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataUpdatePeersReply(FileTag_t tag, NodeSet_t peerSet) {
	debug(DEBUG, "ZetaPacket::NewPacketDataUpdatePeersReply the sizeof peerSet: %d\n", peerSet.size());
	ExINFO_UPDATE_PEERS_REPLY_t info;
	info.tag_ = tag;
	
	NodeSet_t::iterator nsi;
	int i = 0;
	for ( nsi = peerSet.begin(); nsi != peerSet.end(); nsi++) {
		NodeInfoEntry_t nodeInfo = *nsi;
		memcpy ( &info.peerSet_[0] + i * sizeof(NodeInfoEntry_t), &nodeInfo, sizeof(NodeInfoEntry_t) );
		i++;
		if ( i == MAX_PEER_NUM ) {
			debug(WARNING, "MAX_PEER_NUM is too small\n");
			break;
		}
	}
	info.num = i;

        PacketData* userdata = new PacketData ( sizeof ( ExINFO_UPDATE_PEERS_REPLY_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        //debug(DEBUG, "sizeof(ExInfo_UPDATE_SHARED_FILES_t): %d\n", sizeof(ExInfo_UPDATE_SHARED_FILES_t));
        memcpy ( dataptr, &info, sizeof ( ExINFO_UPDATE_PEERS_REPLY_t ) ); // be aware of size.
        //debug(DEBUG, "userdata->size():\t %d\n", userdata->size());
        return userdata;
}

PacketData *ZetaPacket::NewPacketDataLinearComb(FileTag_t tag, NodeAddr_t addr, Port_t port, Block* blk, Parameter para, NodeInfoEntry_t *localInfo) {
	ExInfo_FILE_TRANS_t info;
	info.tag_ = tag;
	info.para_ = para;
	info.nodeInfo_ = *localInfo;

	int blockSize = para.blockSize;
	int coefSize = para.codeWordSize * para.blockNumInData;
	PacketData* userdata = new PacketData ( sizeof ( ExInfo_FILE_TRANS_t ) + blockSize + coefSize ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        memcpy ( dataptr, &info, sizeof(ExInfo_FILE_TRANS_t) );
	dataptr += sizeof(ExInfo_FILE_TRANS_t);
	memcpy ( dataptr, blk->data_, blockSize );
	
	dataptr += blockSize;
	memcpy ( dataptr, blk->coef_, coefSize);
	//printf("The PacketSize is: %d",  sizeof ( ExInfo_FILE_TRANS_t ) + blockSize + coefSize);
        return userdata;
	
}
PacketData *ZetaPacket::NewPacketDataHasFinishedDownloading ( FileTag_t tag )
{
	ExInfo_HAS_FINISHED_DOWNLOADING_t info;
	info.tag_ = tag;

	PacketData* userdata = new PacketData ( sizeof ( ExInfo_HAS_FINISHED_DOWNLOADING_t ) ) ; // be aware of memory.
        BYTE *dataptr = ( BYTE * ) userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }
        //debug(DEBUG, "sizeof(ExInfo_UPDATE_SHARED_FILES_t): %d\n", sizeof(ExInfo_UPDATE_SHARED_FILES_t));
        memcpy ( dataptr, &info, sizeof ( ExInfo_HAS_FINISHED_DOWNLOADING_t ) ); // be aware of size.
        //debug(DEBUG, "userdata->size():\t %d\n", userdata->size());
        return userdata;
}

ExInfo_t* ZetaPacket::GetExtendInfo ( MsgType_t type, int len, PacketData *pktData )   // Attention: this function New() a ExInfo class.
{
        BYTE *dataptr = ( BYTE * ) pktData->data();
        ExInfo_t *info = new ExInfo();

        //debug(DEBUG, "In ZetaPacket::GetExtendInfo, dataptr: %s\n", dataptr);
        int rightSize;
        switch ( type ) {
        case CONNECT_BOOT_SERVER:
                rightSize = sizeof ( ExInfo_CONNECT_BOOT_SERVER_t );
                info->connectBootServer_ = ( ExInfo_CONNECT_BOOT_SERVER_t * ) dataptr;
                break;

        case CONNECT_BOOT_SERVER_REPLY:
                rightSize = sizeof ( ExInfo_CONNECT_BOOT_SERVER_REPLY_t );
                info->connectBootServerReply_ = ( ExInfo_CONNECT_BOOT_SERVER_REPLY_t * ) dataptr;
                break;

        case UPDATE_SHARED_FILES:
                rightSize = sizeof ( ExInfo_UPDATE_SHARED_FILES_t );
                info->updateSharedFile_ = ( ExInfo_UPDATE_SHARED_FILES_t * ) dataptr;
                //debug(DEBUG, "In ZetaPacket.cc Line232, sizeof(ExInfo_UPDATE_SHARED_FILES_t):\t %d\n", sizeof(ExInfo_UPDATE_SHARED_FILES_t));
                break;

        case QUERY:
                rightSize = sizeof ( ExInfo_QUERY_t );
                info->query_ = ( ExInfo_QUERY_t * ) dataptr;
                break;

        case QUERY_HIT:
                rightSize = sizeof ( ExInfo_QUERY_HIT_t );
                info->queryHit_ = ( ExInfo_QUERY_HIT_t * ) dataptr;
                break;

        case CONNECT_TRACKER:
                rightSize = sizeof ( ExInfo_CONNECT_TRACKER_t );
                debug ( DEBUG, "ZetaPacket::GetExtendInfo rightSize: %d\n", rightSize );
                info->connectTracker_ = ( ExInfo_CONNECT_TRACKER_t * ) dataptr;
                break;

        case SELECT_TRACKER:
                rightSize = sizeof ( ExInfo_SELECT_TRACKER_t );
                info->selectTracker_ = ( ExInfo_SELECT_TRACKER_t * ) dataptr;
                break;

        case SELECT_TRACKER_ACK:
                rightSize = sizeof ( ExInfo_SELECT_TRACKER_ACK_t );
                info->selectTrackerAck_ = ( ExInfo_SELECT_TRACKER_ACK_t * ) dataptr;
                break;
		
	case CONNECT_TRACKER_ACK:
		rightSize = sizeof ( ExInfo_CONNECT_TRACKER_ACK_t );
		info->connectTrackerAck_ = (ExInfo_CONNECT_TRACKER_ACK_t *) dataptr;
		//debug(DEBUG, "tag: %d\n", info->connectTrackerAck_->tag_);
		break;
	case REQUEST_START_SEEDER:
		rightSize = sizeof( ExInfo_REQUEST_START_SEEDER_t );
		info->requestStartSeeder_ = (ExInfo_REQUEST_START_SEEDER_t *) dataptr;
		break;

	case UPDATE_PEERS:
		rightSize = sizeof(ExInfo_UPDATE_PEERS_t);
		info->updatePeers_ = (ExInfo_UPDATE_PEERS_t *) dataptr;
		break;

	case UPDATE_PEERS_REPLY:
		rightSize = sizeof(ExINFO_UPDATE_PEERS_REPLY_t);
		info->updatePeersReply_ = (ExINFO_UPDATE_PEERS_REPLY_t *) dataptr;
		break;

	case FILE_TRANS:
		rightSize = sizeof(ExInfo_FILE_TRANS_t);
		info->fileTrans_ = (ExInfo_FILE_TRANS_t *) dataptr;
		debug(DEBUG, "in ZetaPacket::GetExtendInfo(), the header size: %d, the size of the data %d, blockNuminData: %d, codeWordSize %d, wordNuminBlock:, %d, dataSize %d\n", rightSize, len, info->fileTrans_->para_.blockNumInData, info->fileTrans_->para_.codeWordSize, info->fileTrans_->para_.wordNumInBlock, info->fileTrans_->para_.dataSize);
		//printf("ZetaPacket::GetExtendInfo(), block: ");
		//PrintCodeWord( (CodeWord_t*) dataptr + sizeof(ExInfo_FILE_TRANS_t), info->fileTrans_->para_.blockSize + (info->fileTrans_->para_.codeWordSize) * (info->fileTrans_->para_.blockNumInData));
		break;

	case HAS_FINISHED_DOWNLOADING:
		rightSize = sizeof ( ExInfo_HAS_FINISHED_DOWNLOADING_t );
		info->finishedDownloading_ = ( ExInfo_HAS_FINISHED_DOWNLOADING_t* ) dataptr;
		break;

        default:
                debug ( ERROR, "In ZetaPacket::GetExtendInfo, Unknown Msg type [%d]\n", type );
        }

        if ( (len != rightSize) && (type != FILE_TRANS) ) {
                debug ( DEBUG, "Len of Data: %d \t rightSize: %d\n", len, rightSize );
                debug ( ERROR, "In ZetaPacket::GetExtendInfo, size of data incorrect\n" );
                return NULL;
        }
        return info;
}

void ZetaPacket::debug ( DebugLevel level, const char *fmt, ... )
{
        if ( level > msger_->GetApp()->debugLevel )
                return;
        switch ( level ) {
        case ERROR:
                printf ( "[ERROR in ZetaPacket]" );
                break;
        case STAT:
                printf ( "[STAT in ZetaPacket]" );
                break;
        case WARNING:
                printf ( "[WARNING in ZetaPacket]" );
                break;
        case INFO:
                printf ( "[INFO in ZetaPacket]" );
                break;
        case DEBUG:
                printf ( "[DEBUG in ZetaPacket]" );
                break;
        }
        printf ( " "TIME_FORMAT"s ", Scheduler::instance().clock() );
        va_list ap;
        va_start ( ap, fmt );
        vprintf ( fmt, ap );
}


// Note: This Function has been verified in 2010-5-13
// This function is to create a unquie ID for each message.
// The Struture of Message ID:
// ------------------------------
// seq1_1| seq2_| seq3_|
// ------------------------------
//0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
BYTE * ZetaPacket::NewMsgID()
{
        BYTE * newid = NULL;
        newid = ( BYTE * ) malloc ( sizeof ( MsgID_t ) ); // 千万注意内存泄漏。

        // This part is to generate a unquie id for this messager. The
        // reason to use 3 int numbers is to extend the range of the ID
        if ( ( seq3_ > INT_MAX ) || ( seq2_ > INT_MAX ) || ( seq3_ > INT_MAX ) ) {
                std::cout << "In File ZetaPacket.cc,. Caution: Decription ID overflow1" << std::endl;
                return NULL;
        }
        seq1_ ++;
        if ( seq1_ > INT_MAX ) {
                seq1_ = 0;
                seq2_ ++;
                if ( seq2_ > INT_MAX ) {
                        seq2_ = 0;
                        seq3_ ++;
                        if ( seq3_ > INT_MAX ) {
                                std::cout << "In File ZetaPacket.cc,. Caution: Decription ID overflow2" << std::endl;
                                return NULL;
                        }
                }
        }
        // Attention: Error Prone in Memeory copy. Pay atttention to the subscript.
        memcpy ( ( void* ) newid, &seq1_, sizeof ( NodeAddr_t ) );
        memcpy ( ( void* ) ( newid + sizeof ( int ) ), &seq2_, sizeof ( int ) );
        memcpy ( ( void* ) ( newid+ 2 * sizeof ( int ) ), &seq3_, sizeof ( int ) );
        return newid;
}

// Refer TypeDefine.h for the specification of the packet header.
PacketData* ZetaPacket::NewPacketData ( MsgType_t type, NodeAddr_t addr, int len, BYTE* data )
{
        std::cout << "ZetaPacket::NewPacket()" << std::endl; // for test

        PacketData* userdata = new PacketData ( hdrLen_ + len );
        unsigned char *dataptr = userdata->data();
        if ( dataptr == NULL ) {
                return NULL;
        }

        // the following sentences generate a header
        header_.type_ = type;
        BYTE* id = NewMsgID();
        if ( id == NULL ) {
                std::cout << "failed to create new message ID" << std::endl;
                return NULL;
        } else {
                memcpy ( &header_.id_, id, sizeof ( MsgID_t ) );
        }
        delete id;
        //std::cout << "Size of MsgID_t: " << sizeof(MsgID_t) << std::endl; // for test
        header_.addr_ = addr;

        // Copy the header to the Packet.
        memcpy ( dataptr, &header_, hdrLen_ );
        // Copy the Data to the Packet, Waited to be finished 2010-5-13
        memcpy ( dataptr + hdrLen_, data, len );

        return ( PacketData* ) userdata;
}