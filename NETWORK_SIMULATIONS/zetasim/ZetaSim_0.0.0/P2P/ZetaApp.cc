/* ZetaApp.h
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
#include "TypeDefine.h"
#include "ZetaApp.h"
#include "ZetaMsger.h"

AddrNameMap_t addrNameMap;

ZetaApp::ZetaApp() : P2PApp ()
{
        isBootServer_ = false; // the default state of a new Zeta Application Node.
        msger_ = NULL;
        nodeState_ = PS_OFFLINE;
        manager_ = NULL;
        //downloader_ = new ZetaDownloader();
	bind ("startSeederTh_", &startSeederTh_);
        bind ( "tracker_threshold_", &th_ );
	bind_bw ("maxUploadSpeed_", &maxUploadSpeed_);
	bind_bool("isBootServer_", &isBootServer_);
	bind_time("updatePeersInterval_", &updatePeersInterval_);
	bind("codeWordSize_", &(para_.codeWordSize));
	bind("blockSize_", &(para_.blockSize));
	bind("dataSize_", &(para_.dataSize));
	//bind_bw("codeWordSize_", &(para_.codeWordSize));
	//bind_bw("blockSize_", &(para_.blockSize));
	//bind_bw("dataSize_", &(para_.dataSize));
}

ZetaApp::~ZetaApp()
{
        if ( msger_ ) {
                delete msger_;
        }
        if ( manager_ ) {
                delete manager_;
        }
}

void ZetaApp::join()
{
        debug ( DEBUG, "ZetaApp::join()\n" );
        msger_->ConnectBootServer();
}

void ZetaApp::leave()
{
        debug ( DEBUG, "ZetaApp::leave()\n" );
}

void ZetaApp::search ( FileTag_t tag )
{
        debug ( INFO, "Node [%s] wants to search file with FileTag [%d]\n", alias_.c_str(), tag );
	debug( DEBUG, "AddrNamMap: \n");
	AddrNameMap_t::iterator ai;
	for ( ai = addrNameMap.begin(); ai != addrNameMap.end(); ai++ ) {
		debug( DEBUG, "Addr: [%d] Name_ [%s] Alias [%s]\n",ai->first, ai->second.name_.c_str(), ai->second.alias_.c_str() );
	}

	if ( nodeState_ == PS_OFFLINE ) {
		debug(ERROR, "THe query of Node [%s] can not be executed because it is offline\n", alias_.c_str());
		return;
	}
        msger_->Query ( tag );
}

void ZetaApp::AddBootServer ( NodeAddr_t addr )
{
        debug ( DEBUG, "ZetaApp::AddBootServer\n" );
        pair<AddrSet_t::iterator,bool> ret = bootServerSet_.insert ( addr );
        if ( ret.second == false ) {
                debug ( INFO, "this server has been added before\n" );
        }
}

NodeInfoEntry_t* ZetaApp::GetLocalInfo() {
		return new NodeInfoEntry(GetLocalAddr(),
					 GetLocalPort(),
					 nodeState_,
					 name_,
					 maxUploadSpeed_);
}

void ZetaApp::upcall_RecvSelectTrackerAck ( NodeAddr_t addr, Port_t port, FileTag_t tag )
{
        Tag2NodeInfo_t::iterator ti = trackerMap_.find ( tag );
        NodeInfoEntry_t tracker( addr, port );
        trackerMap_.insert ( pair<FileTag_t, NodeInfoEntry_t> ( tag, tracker ) );
        //NodeInfoEntry_t tracker = ti->second;
        debug ( INFO, "Node [%s] receive a SELECT_TRACKER_ACK\n", alias_.c_str() );

	// Send REQUEST_START_SEEDER messager to all nodes who have this file.
 	NodeSet_t& peerSet = searchTable_.find(tag)->second;
	//NodeSet_t::const
 	NodeSet_t::iterator nsi;
 	NodeInfoEntry_t nodeInfo;
 	for ( nsi = peerSet.begin(); nsi != peerSet.end(); nsi++ ) {
 		nodeInfo = *nsi;
		debug(DEBUG, "Send a RequestStartSeeder for file [%d] to Node [%d]\n", tag, nodeInfo.addr_);
 		msger_->RequestStartSeeder(tag, nodeInfo.addr_, nodeInfo.port_, tracker);
 	}

	// Resolve the pending Query Message
        Tag2NodeSet_t::iterator si = pendingQueryMsg_.find ( tag );
        NodeSet_t& aps = si->second;
        if ( aps.size() == 0 ) {
                debug ( ERROR, "Node [%s] receive a wrong SELECT_TRACKER_ACK\n", alias_.c_str() );
        }
        NodeSet_t::iterator ai;
        for ( ai = aps.begin(); ai != aps.end(); ai++ ) {
                debug ( INFO,
                        "Resolve Pending Query Msg, Node [%s] send QueryHit Msg to Node [%s]\n",
                        alias_.c_str(),
                        addrNameMap.find ( ai->addr_ )->second.alias_.c_str() );
                msger_->QueryHit ( tag, ai->addr_, ai->port_, tracker );
        }
        return;

}

void ZetaApp::upcall_RecvQuery ( NodeAddr_t srcAddr, Port_t srcPort, FileTag_t tag )
{
        debug ( DEBUG,
                "Boot Server Node [%s] receive a file query with FileTag tag [%d] from Node [%s]\n",
                alias_.c_str (),
                tag,
                addrNameMap.find ( srcAddr )->second.alias_.c_str());

        Tag2NodeSet_t::iterator si = searchTable_.find ( tag );
        if ( si == searchTable_.end () ) {
                // Query Not Hit
                msger_->QueryNotHit ( srcAddr, srcPort );
                return;
        }
        //For Test, Return itself as Tracker, Need to been changed in later version
        NodeInfoEntry_t *localInfo = GetLocalInfo(); // ap contain the addr and port of the tracker;
	NodeAddr_t trackerAddr = GetLocalAddr();
	Port_t trackerPort = GetLocalPort();
//         ap.addr_ = GetLocalAddr();
//         ap.port_ = GetLocalPort();
        // for test. Always select itself as tracker.
        Tag2NodeInfo_t::iterator ti = trackerMap_.find ( tag );
        if ( ti == trackerMap_.end() ) { //If this file don't have tracker yet.
                Tag2NodeSet_t::iterator si = pendingQueryMsg_.find ( tag );
                if ( si == pendingQueryMsg_.end() ) { // if this is the first user who querys this file
                        debug ( DEBUG, "This is the first user querying file with tag [%d]\n", tag );
                        NodeSet_t aps;
                        NodeInfoEntry_t srcAP ( srcAddr, srcPort );
                        aps.insert ( srcAP );
                        pendingQueryMsg_.insert ( pair<FileTag_t, NodeSet_t> ( tag,  aps ) );
                        debug ( DEBUG,
                                "Insert it into PendingQueryMsgQueue: sizeof(pendingMsgQ): %d, sizeof(AddrPortSet): %d, Node[%s]\n",
                                pendingQueryMsg_.size(),
                                aps.size(),
                                addrNameMap.find ( srcAddr )->second.alias_.c_str() );
                        msger_->SelectTracker ( tag, trackerAddr, trackerPort, *localInfo );
                } else {
                        NodeInfoEntry_t srcAP ( srcAddr, srcPort );
                        si->second.insert ( srcAP );
                }
        } else {
                msger_->QueryHit ( tag, srcAddr, srcPort, *localInfo ); // for test return ap as the tracker
        }
        delete localInfo;
}

void ZetaApp::upcall_UpdateSharedFilesAck ()
{
        debug ( INFO, "Update Shared Files has been Acked\n" );
}


void ZetaApp::upcall_QueryNotHit ( NodeAddr_t srcAddr, Port_t srcPort )
{
        debug ( INFO,
                "Boot Server Node [%s] could not find file for Node [%s] \n",
                addrNameMap.find ( srcAddr )->second.alias_.c_str (),
                alias_.c_str () );
}

void ZetaApp::upcall_ConnectBootServer ( std::string alias, NodeAddr_t addr )
{
        if ( nodeState_ == PS_ACTIVE ) {
                debug ( INFO, "Node [%s] to Node [%s], This node has set other node as its boot server\n", alias_.c_str(), alias.c_str() );
                msger_->DisConnectBootServer ( addr, "has set other node as its boot server" );
                return;
        }
        // Set the infomation of boot server and node state
	debug( DEBUG, "bootserver addr before: %d, later: %d\n", bootServer_.addr_, addr);
        bootServer_.addr_ = addr;
        nodeState_ = PS_ACTIVE;

        debug ( INFO, "Node [%s] Join Successfully\n", alias_.c_str() );

        msger_->UpdateSharedFiles();
}

void ZetaApp::upcall_RecvConnectRequest ( NodeAddr_t addr, Port_t port, std::string name, int maxUploadSpeed )
{
        if ( !isBootServer_ ) { // Don't reply if this app is not a BootServer
                return;
        }
        NodeInfoEntry_t info ( addr, port, PS_ACTIVE, name, maxUploadSpeed );
        serveMap_.insert ( pair<NodeAddr_t, NodeInfoEntry_t> ( addr, info ) );
        debug ( INFO, "Node [%s] has been insert to serveMap.\n", name.c_str() );
        msger_->ConnectBootServerReply ( addr, port );
}

void ZetaApp::upcall_RecvDisconnectRequest ( NodeAddr_t addr )
{
        debug ( INFO, "Node [%s] has been deleted from serveMap of Node [%s].\n", serveMap_.find ( addr )->second.name_, alias_.c_str() );
        serveMap_.erase ( addr );
}

void ZetaApp::upcall_RecvUdpateSharedFilesRequest ( NodeAddr_t addr, Port_t port, FileOperation_t op, SharedFileSet_t fset, int fileNum )
{
        if ( fileNum == 0 ) {
                debug ( WARNING,
                        "Node [%s] Receive a Update Shared File Request from Node [%s] without any file tag\n",
                        alias_.c_str(),
                        addrNameMap.find ( addr )->second.alias_.c_str() );
                return;
        }

        BYTE *fi;
        for ( fi = fset; fi < fset + fileNum * sizeof ( FileTag_t ) ; fi += sizeof ( FileTag_t ) ) {
                Tag2NodeSet_t::iterator ti = searchTable_.find ( * ( FileTag_t* ) fi );
                if ( ti == searchTable_.end() ) { // If cannot find the record for this file, create one for it
                        NodeInfoEntry_t ap;
                        ap.addr_ = addr;
                        ap.port_ = port;
                        NodeSet_t APS;
                        APS.insert ( ap );
                        searchTable_.insert ( pair<FileTag_t, NodeSet_t> ( * ( FileTag_t* ) fi, APS ) );
                        debug ( INFO,
                                "Has Created a record for FileTag: [%d], Node [%s] is in the record.\n",
                                * ( FileTag_t * ) fi, addrNameMap.find ( addr )->second.alias_.c_str() );
                } else {
                        NodeInfoEntry_t ap;
                        ap.addr_ = addr;
                        ap.port_ = port;
                        ti->second.insert ( ap ); // Add this address to the record directly.
                        debug ( INFO,
                                "Node [%s] has been added to the record of FileTag: [%d].\n",
                                addrNameMap.find ( addr )->second.alias_.c_str(),
                                * ( FileTag_t * ) fi );
                }
        }
        msger_->UpdateSharedFilesAck ( addr, port ); // Send ACK to users
}

void ZetaApp::upcall_RecvSelectTracker ( FileTag_t tag, NodeInfoEntry_t ap )
{
	debug ( DEBUG, "bootServer_.addr_: %d, ap.addr_: %d\n", bootServer_.addr_, ap.addr_);
        if ( ap.addr_ != bootServer_.addr_ ) {
                debug ( ERROR,
                        "Node [%s] receive a wrong SELECT_TRACKER messager from node [%s]\n",
                        alias_.c_str(),
                        addrNameMap.find ( ap.addr_ )->second.alias_.c_str() );
                return;
        }
        debug ( INFO,
                "Node [%s] receive a SELECT_TRACKER messager from node [%s]\n",
                alias_.c_str(),
                addrNameMap.find ( ap.addr_ )->second.alias_.c_str() );
        // Create A seeder
        manager_->CreateSeeder ( tag, ap.addr_, ap.port_ );

        trackFileSet_.insert ( tag );
	NodeSet_t ps;
	DownloadingPeerSetTable_.insert( pair<FileTag_t, NodeSet_t>(tag, ps) );
        msger_->SelectTrackerAck ( tag, ap.addr_, ap.port_ );
}

void ZetaApp::upcall_RecvConnectTrackerAck(FileTag_t tag, Parameter para) {
		manager_->RecvConnectTrackerAck(tag, para);
		debug(DEBUG, "RecvConnectTrackerAck\n");
}

void ZetaApp::upcall_ConnectTracker ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeInfoEntry_t info, int nodeType )
{
        if ( trackFileSet_.find ( tag ) == trackFileSet_.end() ) {
                debug ( ERROR,
                        "Node [%s] has sent a wrong CONNECT_TRACKER message to Node [%s]\n",
                        addrNameMap.find ( addr )->second.alias_.c_str(),
                        alias_.c_str() );
                return;
        }
        debug ( INFO,
                "Node [%s] has sent a CONNECT_TRACKER message to Node [%s]\n",
                addrNameMap.find ( addr )->second.alias_.c_str(),
                alias_.c_str() );

        //debug ( DEBUG, "DownloadingPeerSetTable_.size(): %d\n", DownloadingPeerSetTable_.size() );
	if (nodeType == DOWNLOADER) {
		Tag2NodeSet_t::iterator si = DownloadingPeerSetTable_.find ( tag );
		if ( si == DownloadingPeerSetTable_.end() ) {
			debug ( ERROR,
				"Node [%s] has sent an Unrecognized CONNECT_TRACKER msg to Node [%s]\n",
				addrNameMap.find ( addr )->second.alias_.c_str(),
				alias_.c_str() );
			return;
		}
		NodeSet_t& downloadingSet = si->second;
		//debug(DEBUG, "before the peerSet Size for file [%d] in Node [%s] is %d\n",  tag, alias_.c_str(), peerSet.size());
		debug(DEBUG, "ZetaApp::upcall_ConnectTracker info.addr_: %d\n", info.addr_);
		downloadingSet.insert ( info ); // add this node to peer set.
		//debug(DEBUG, "the peerSet Size for file [%d] in Node [%s] becomes %d\n", tag, alias_.c_str(), peerSet.size());

		para_.Update(); // Update the Network Simulation Results.
		msger_->ConnectTrackerAck ( tag, addr, port, para_);
	} else if (nodeType == SEEDER) {
		Tag2NodeSet_t::iterator si = SeedingPeerSetTable_.find(tag);
		if ( si == DownloadingPeerSetTable_.end() ) {
			NodeSet_t seedingSet;
			seedingSet.insert( info );
			DownloadingPeerSetTable_.insert ( pair<FileTag_t, NodeSet_t>(tag, seedingSet) );
		} else {
			NodeSet_t& seedingSet = si->second;
			seedingSet.insert(info);
		}
	} else {
		std::cerr << "ZetaApp::upcall_ConnectTracker, unknown nodetype"<< endl;
	}
}

void ZetaApp::upcall_RecvUpdatePeers(NodeAddr_t addr, Port_t port, FileTag_t tag) {
		Tag2NodeSet_t::iterator si = DownloadingPeerSetTable_.find(tag);
		if ( si == DownloadingPeerSetTable_.end() ) {
			debug(ERROR, "Node [%s] receive a wrong UPDATE_PEERS calls from Node [%s]",
			      alias_.c_str(),
			      addrNameMap.find(addr)->second.alias_.c_str());
			return;
		}
		NodeSet_t downloadingPeerSet = si->second;
// 		if ( downloadingPeerSet.size() < (unsigned int) th_ ) {
// 			NodeInfoEntry *info = GetLocalInfo();
// 			downloadingPeerSet.insert(*info);
// 			delete info;
// 		}
		debug(DEBUG, "ZetaApp::upcall_RecvUpdatePeers the size of downloadingPeerSet: %d\n", downloadingPeerSet.size());
		msger_->UpdatePeersReply(addr, port, tag, downloadingPeerSet);
}

void ZetaApp::upcall_RecvUpdatePeersReply(NodeAddr_t addr, Port_t port, FileTag_t tag, PeerSetInfo_t pset, int num) {

	debug(DEBUG, "ZetaApp::upcall_RecvUpdatePeersReply, num of downloading peers: %d\n", num);
	BYTE *fi;
	NodeSet_t nodeSet;
	for ( fi = pset; fi < pset + num * sizeof(NodeInfoEntry_t); fi += sizeof(NodeInfoEntry_t) ) {
		NodeInfoEntry *nodeInfo = (NodeInfoEntry_t *) fi;
		nodeSet.insert(*nodeInfo);
	}

	manager_->RecvUpdatePeersReply(addr, port, tag, nodeSet, num);
}

void ZetaApp::upcall_RecvRequestStartSeeder(FileTag_t tag, NodeInfoEntry_t tracker) {
		manager_->CreateSeeder(tag, tracker.addr_, tracker.port_);
		debug(INFO, "Node [%s] starts a seeder for file [%d]\n", alias_.c_str(), tag);
}

void ZetaApp::upcall_QueryHit ( FileTag_t tag, NodeInfoEntry_t tracker) {
                manager_->CreateDownloader ( tag, tracker.addr_, tracker.port_);
}

void ZetaApp::upcall_RecvFileTransBlock (FileTag_t tag, Parameter para, NodeInfoEntry_t srcInfo, Block *blk) {
	debug(DEBUG, "Enter ZetaApp::upcall_RecvFileTransBlock()\n");
	manager_->RecvFileTransBlock ( tag, para, srcInfo,  blk );
}

void ZetaApp::upcall_RecvFinishedDownloadMsg(NodeAddr_t srcAddr, Port_t srcPort, FileTag_t tag)
{
	Tag2NodeSet_t::iterator di = DownloadingPeerSetTable_.find(tag);
	NodeSet_t::iterator nsi;
	debug(DEBUG, "ZetaApp::upcall_RecvFinishedDownloadMsg\n");
	for ( nsi = di->second.begin(); nsi != di->second.end(); nsi++ ) {
		debug(DEBUG, "ZetaApp::upcall_RecvFinishedDownloadMsg addr: [%d] Node [%s] \n",
		      nsi->addr_,
		      addrNameMap.find(nsi->addr_)->second.alias_.c_str());
	}
	NodeInfoEntry_t info(srcAddr,srcPort);
// 	NodeSet_t::iterator ni = di->second.find(info);
// 	
// 	if ( ni == di->second.end() ) {
// 		debug(ERROR, "Node [%s] has received a wrong RecvFinishedDownloadMsg from Node [%s]\n",
// 		      alias_.c_str(),
// 		      addrNameMap.find(srcAddr)->second.alias_.c_str());
// 		      Tag2NodeSet_t::iterator di;
// 		      for ( di = DownloadingPeerSetTable_.begin(); di != DownloadingPeerSetTable_.end(); di++ ) {
// 			      debug ( DEBUG, "DownloadingPeerSetTable_ tag: %d\n", di->first );
// 		      }
// 	}
	di->second.erase(info);
	// Need to be Revised. In Later Version, It Should Send a ACK back to downloader
}

void ZetaApp::SetMsger ( ZetaMsger *msger )
{
        debug ( DEBUG, "ZetaApp::SetMsger()\n" );
        msger_ = msger;
}

int ZetaApp::command ( int argc, const char*const* argv )
{
        Tcl &tcl = Tcl::instance();
        if ( strcmp ( argv[1], "join" ) == 0 ) {
                if ( isBootServer_ ) {
                        debug ( INFO, "this is a BootServer\n" );
			bootServer_.addr_ = GetLocalAddr();
                } else {
                        join();
                }
                return TCL_OK;

        } else if ( strcmp ( argv[1], "leave" ) == 0 ) {
                leave();
                return TCL_OK;

        } else if ( strcmp ( argv[1], "search" ) == 0 ) {
                //int tag = atoi(argv[2]);
                //search(tag);
                return TCL_OK;

        } else if ( strcmp ( argv[1], "attach_messager" ) == 0 ) {
                std::cout << "attach_messager()" << std::endl;
                msger_ = ( ZetaMsger * ) TclObject::lookup ( argv[2] );
                if ( msger_ == NULL ) {
                        tcl.add_errorf ( "Cannot Find TcLObject: %s",argv[2] );
                        return TCL_ERROR;
                }
                return TCL_OK;

        } else if ( strcmp ( argv[1], "init_msger" ) == 0 ) {
                //debug(DEBUG, "ZetaApp.cc line82: argc: %d \n", argc);
                if ( argc < 3 ) {
                        return TCL_ERROR;
                }

                //debug(DEBUG, "argv[2] :%s \n", argv[2]);
                NSSocket* a = ( NSSocket * ) TclObject::lookup ( argv[2] );
                if ( a == NULL ) {
                        tcl.add_errorf ( "Cannot Find TcLObject: %s",argv[2] );
                        return TCL_ERROR;
                }
                msger_ = new ZetaMsger ( this, a );
                manager_ = new ZetaFileManager ( this, msger_ );

                //debug(DEBUG, "ZetaApp.cc, line 97, argc: %d.\n", argc);
                if ( argc == 3 ) {
                        if ( alias_.empty() ) {
                                debug ( WARNING,
                                        "In Node[%s], You should set alias first\n",
                                        alias_.c_str() );
                        }
                        Name_t n;
                        n.name_ = argv[2];
                        n.alias_ = alias_;
                        addrNameMap.insert ( pair<NodeAddr_t, Name_t> ( a->addr(), n ) );
                        debug ( DEBUG,
                                "Insert A Node with addr: [%d], name: [%s] alias: [%s] to the AddrNameMap\n",
                                a->addr(),
                                argv[2],
                                alias_.c_str() );
                        return TCL_OK;
                }

                NodeAddr_t addr = atoi ( argv[3] );
                Port_t port = atoi ( argv[4] );
                debug ( DEBUG, "ZetaApp.cc: line 104 addr: %s, port: %s \n", argv[3], argv[4] );
                if ( ( addr == 0 ) || ( port == 0 ) ) {
                        debug ( ERROR, "incorrect format for init-msger\n" );
                        return TCL_ERROR;
                }
                if ( !msger_ ) {
                        debug ( ERROR, "This is no messager attached. \n" );
                        return TCL_ERROR;
                }
                if ( ! ( msger_->GetSocketP() ) ) {
                        debug ( ERROR, "Could not find a socket\n" );
                        return TCL_ERROR;
                }
                msger_->GetSocketP()->Bind ( addr, port );


                debug ( DEBUG, "zetaApp.cc::line 119 AddrNameMap.insert. addr: %d, name: %s\n", addr, argv[2] );
                //Addr Addr and Name to the AddrName Map
                if ( alias_.empty() ) {
                        debug ( WARNING, "You should set alias first\n" );
                }
                Name_t n;
                n.name_ = argv[2];
                n.alias_ = alias_;
                debug ( DEBUG, "alias_: %s", alias_.c_str() );
                addrNameMap.insert ( pair<NodeAddr_t, Name_t> ( addr, n ) );
                return TCL_OK;

        } else if ( strcmp ( argv[1], "add_boot_server" ) == 0 ) {
                ZetaApp *a = ( ZetaApp * ) TclObject::lookup ( argv[2] );
                //debug(DEBUG, "ZetaApp.cc line 123: name of added bootserver sock is: %s \n", argv[2]);
                debug ( DEBUG, "ZetaApp.cc line 124: address of added bootserver sock is: %d \n", a->GetLocalAddr() );
                AddBootServer ( a->GetLocalAddr() );
                return TCL_OK;

//         } else if ( strcmp ( argv[1], "set_type" ) == 0 ) {
//                 if ( strcmp ( argv[2], "boot_server" ) == 0 ) {
//                         isBootServer_ = true;
//                         nodeState_ = PS_ACTIVE;
//                 } else if ( strcmp ( argv[2], "average" ) == 0 ) {
//                         isBootServer_ = false;
//                 } else {
//                         std::cout << "incorrect format" << std::endl;
//                 }
//                 return TCL_OK;

        } else if ( strcmp ( argv[1], "set_alias" ) == 0 ) {
                alias_ = std::string ( argv[2] );
                return TCL_OK;

        } else if ( strcmp ( argv[1], "shared_files" ) == 0 ) {

                // This code block is valid only if "typedef int FileTag_t";
                if ( ( argc-3 ) %2 != 0 ) {
                        debug ( ERROR, "Incorrect shared_files Sentence for Node [%s] : wrong arg number\n", alias_.c_str() );
                        return TCL_OK;
                }
                if ( strcmp ( argv[2], "replace" ) == 0 ) {
                        sharedFileMap_.clear();
                        debug ( INFO, "shared map of Node [%s] has been cleared\n", alias_.c_str() );
                }
                int i;
                for ( i = 3; i < argc; i+=2 ) {
                        FileTag_t tag ( atoi ( argv[i] ) );
                        if ( tag == 0 ) {
                                debug ( ERROR, "Node [%s], FileTag should be int \n", alias_.c_str() );
                                return TCL_OK;
                        }
                        FilePath_t path ( argv[i+1] );
                        if ( strcmp ( argv[2], "add" ) == 0 ) {
                                sharedFileMap_.insert ( pair<FileTag_t, FilePath_t> ( tag, path ) );
                                debug ( INFO,
                                        "file: [%d] with path: [%s] has been added to shared map of Node [%s] \n",
                                        tag,
                                        path.c_str(),
                                        alias_.c_str() );
                        } else if ( strcmp ( argv[2], "replace" ) == 0 ) {
                                debug ( INFO,
                                        "file: [%d] with path: [%s] has been added to shared map of Node [%s] \n",
                                        tag,
                                        path.c_str(),
                                        alias_.c_str() );
                                sharedFileMap_.insert ( pair<FileTag_t, FilePath_t> ( tag, path ) );
                        } else if ( strcmp ( argv[2], "delete" ) == 0 ) {
                                sharedFileMap_.erase ( tag );
                                debug ( INFO,
                                        "file: [%d] with path: [%s] has been erased from shared map of Node [%s] \n",
                                        tag,
                                        path.c_str(),
                                        alias_.c_str() );
                        } else {
                                debug ( ERROR, "Incorrect shared_files Sentence for Node [%s] : unrecoginized cmd\n", alias_.c_str() );
                        }
                }
                return TCL_OK;

        } else if ( strcmp ( argv[1], "query" ) == 0 ) {
                FileTag_t tag = atoi ( argv[2] );
                search ( tag );
                return TCL_OK;


        } else {
                return P2PApp::command ( argc, argv );
        }
}
