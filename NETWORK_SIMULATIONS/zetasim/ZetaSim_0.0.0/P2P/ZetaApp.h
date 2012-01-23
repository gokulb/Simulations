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
#ifndef NS_ZETA_APP_H
#define NS_ZETA_APP_H

#include "TypeDefine.h"
#include "ExtendInfo.h"
#include "P2PApp.h"
#include "ZetaMsger.h"
#include "ZetaFileManager.h"


extern AddrNameMap_t addrNameMap;
class ZetaApp:public P2PApp
{
public:		// functions
        ZetaApp ();
        ~ZetaApp ();
//-----API to Users---------
//overloaded method.
        virtual void join ();
        virtual void leave ();
        virtual void search ( FileTag_t tag );
//new method,
        void AddBootServer ( NodeAddr_t addr );
        void DelBootServer ();

// ------------------------------------
// upcall functions called by ZetaMsger
//--------------------------------------
// upcalls in average users
        void upcall_UpdatePeers ();
        void upcall_ConnectBootServer ( std::string alias, NodeAddr_t addr );
        void upcall_QueryNotHit ( NodeAddr_t srcAddr, Port_t srcPort );
        void upcall_QueryHit ( FileTag_t tag, NodeInfoEntry_t tracker);
        
	void upcall_RecvConnectTrackerAck(FileTag_t tag, Parameter para);
	void upcall_RecvRequestStartSeeder(FileTag_t tag, NodeInfoEntry_t tracker);
	void upcall_RecvUpdatePeersReply(NodeAddr_t addr, Port_t port, FileTag_t tag, PeerSetInfo_t pset, int num);
	void upcall_RecvFileTransBlock (FileTag_t tag, Parameter para, NodeInfoEntry_t srcInfo, Block* blk);
// upcalls in Boot Server
        void upcall_UpdateSharedFilesAck ();
        void upcall_RecvConnectRequest ( NodeAddr_t addr, Port_t port, std::string name, int maxUploadSpeed );
        void upcall_RecvDisconnectRequest ( NodeAddr_t addr );
        void upcall_RecvUdpateSharedFilesRequest ( NodeAddr_t addr, Port_t port,
                        FileOperation_t op, SharedFileSet_t fset, int fileNum );
        void upcall_RecvQuery ( NodeAddr_t srcAddr, Port_t srcPort, FileTag_t tag );
        void upcall_RecvSelectTrackerAck ( NodeAddr_t addr, Port_t port, FileTag_t tag );
// upcalls in tracker
        void upcall_RecvSelectTracker ( FileTag_t tag, NodeInfoEntry_t ap );
        void upcall_ConnectTracker ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeInfoEntry_t info, int nodeType);
	void upcall_RecvUpdatePeers(NodeAddr_t addr, Port_t port, FileTag_t tag);
	void upcall_RecvFinishedDownloadMsg(NodeAddr_t srcAddr, Port_t srcPort, FileTag_t tag);

// -----functions that can change the state of app-----
        void SetMsger ( ZetaMsger * msger );
        AddrSet_t *GetbootServerSetP () {  return &bootServerSet_;  }

        NodeInfoEntry_t *GetbootServerP () {    return &bootServer_;    }
        FileMap_t *GetSharedFileMapP () {  return &sharedFileMap_;    }
	// Get Local Infomation
	NodeInfoEntry_t* GetLocalInfo();
	NodeAddr_t GetLocalAddr () {  return msger_->GetSocketP ()->addr ();   }
        NodeAddr_t GetLocalPort () {  return msger_->GetSocketP()->port();  }
        double GetMaxUploadSpeed () {    return maxUploadSpeed_;     }
        NodeState_t GetNodeState() { return nodeState_; }
        std::string GetName() {	return name_; }
        std::string GetAlias() { return alias_; }
        double GetUpdatePeersInterval() { return updatePeersInterval_; }
        NodeSet_t& SearchDownloadingPST(FileTag_t tag) {
		Tag2NodeSet_t::iterator ti = DownloadingPeerSetTable_.find(tag);
		return ti->second;
	}

public:		// variables
        std::string alias_;
        int isBootServer_;
	//-------------SIMULATION PARAMETERS---------------
	Parameter para_;
	double startSeederTh_; // When the ratio of downloaded parts has surpass startSeederTh_, downloader will signal to the file manager to start a seeder.
	int th_; // when the number of seeder is < th_, the tracker will help to seed file, too.
	double updatePeersInterval_;

protected:			// methos
        virtual int command ( int argc, const char *const *argv );
protected:			// variables
        AddrSet_t bootServerSet_;
        NodeInfoEntry_t bootServer_;
	
        double maxUploadSpeed_;
        NodeState_t nodeState_;
        FileMap_t sharedFileMap_;

        //PeerMap_t peerMap_;
        ZetaMsger *msger_;
        ZetaFileManager *manager_;

//for Boot Server
        //PeerMap_t serveMap_;
	ID2NodeInfo_t serveMap_;
	Tag2NodeSet_t searchTable_; // this table stores the infomation of all files in its sub nodes.
	Tag2NodeInfo_t trackerMap_;
	Tag2NodeSet_t pendingQueryMsg_;
	// for tracker
	TagSet_t trackFileSet_;
	Tag2NodeSet_t DownloadingPeerSetTable_; // stores the downloading peer set of each file.
	Tag2NodeSet_t SeedingPeerSetTable_; // stores the seeding peer set of each file.

};

static class ZetaAppClass:public TclClass
{
public:
        ZetaAppClass () :TclClass ( "Application/ZetaApp" ) {
        }
        TclObject *create ( int, const char *const * ) {
                return ( new ZetaApp () );
        }
} class_zeta_app;

#endif
