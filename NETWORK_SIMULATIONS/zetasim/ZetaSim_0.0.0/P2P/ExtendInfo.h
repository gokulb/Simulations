/* ExtendInfo.h
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
#ifndef EXTEND_INFO_H
#define EXTEND_INFO_H
#include "TypeDefine.h"
#include "FiniteFieldCal.h"
typedef enum { // MsgType_t
        NO_TYPE = 0,
// Average Users
        CONNECT_BOOT_SERVER,
        CONNECT_BOOT_SERVER_REPLY,
        DISCONNECT_BOOT_SERVER,
        UPDATE_SHARED_FILES,
        QUERY,
        CONNECT_TRACKER, // used in downloader
        HAS_FINISHED_DOWNLOADING,  // used in downloader
        UPDATE_PEERS, // used in seeder
        FILE_TRANS, // used in seeder
// For Boot Server
        UPDATE_SHARED_FILES_ACK,
        QUERY_NOT_HIT,
        QUERY_HIT ,
        SELECT_TRACKER,
        REQUEST_START_SEEDER,
// For File Tracker
        SELECT_TRACKER_ACK,
        CONNECT_TRACKER_ACK,
	UPDATE_PEERS_REPLY
} MsgType_t;


//-------------------------------------
//--  The Structure of Extend infomation for
// -- each message.
//--------------------------------------
class Parameter;
typedef struct {
        char name_[MAX_STRING_LEN];
        int maxUploadSpeed_;
} ExInfo_CONNECT_BOOT_SERVER_t;

typedef struct { // ConnectBootServerReply_t
        char name_[MAX_STRING_LEN];
} ExInfo_CONNECT_BOOT_SERVER_REPLY_t;

typedef struct {
        char reason_[MAX_STRING_LEN];
} ExInfo_DISCONNECT_BOOT_SERVER_t;


#define MAX_FILE_NUM 100
typedef BYTE SharedFileSet_t [ MAX_FILE_NUM * sizeof ( FileTag_t ) ];
typedef enum { REPLACE, ADD, DELETE } FileOperation_t;
typedef struct { // UpdateSharedFile_t
        FileOperation_t op;
        int fileNum;
        SharedFileSet_t sharedFiles_;
} ExInfo_UPDATE_SHARED_FILES_t;

typedef struct {
        FileTag_t tag;
} ExInfo_QUERY_t;

typedef struct {
        FileTag_t tag_;
        NodeInfoEntry info_;
	int nodeType_;
} ExInfo_CONNECT_TRACKER_t;

typedef struct {
        FileTag_t tag_;
        NodeInfoEntry tracker_;
	//Parameter* para;
} ExInfo_QUERY_HIT_t;

typedef struct {
        FileTag_t tag_;
        NodeInfoEntry bserver_;
} ExInfo_SELECT_TRACKER_t;

typedef struct {
        FileTag_t tag_;
} ExInfo_SELECT_TRACKER_ACK_t;

typedef struct {
        FileTag_t tag_;
	Parameter para_;
} ExInfo_CONNECT_TRACKER_ACK_t;


typedef struct {
	FileTag_t tag_;
	NodeInfoEntry tracker_;
} ExInfo_REQUEST_START_SEEDER_t;

typedef struct {
	FileTag_t tag_;
} ExInfo_UPDATE_PEERS_t;

typedef struct {
	FileTag_t tag_;
} ExInfo_HAS_FINISHED_DOWNLOADING_t;

#define MAX_PEER_NUM 10
typedef BYTE PeerSetInfo_t [ MAX_PEER_NUM * sizeof ( NodeInfoEntry_t ) ];
typedef struct {
	FileTag_t tag_;
	int num;
	PeerSetInfo_t peerSet_;
} ExINFO_UPDATE_PEERS_REPLY_t;

typedef struct {
	FileTag_t tag_;
	Parameter para_;
	NodeInfoEntry_t nodeInfo_;
} ExInfo_FILE_TRANS_t;
//#define MAX_ADDR_NUM 100
typedef BYTE AddrArray_t [ MAX_FILE_NUM * sizeof ( NodeInfoEntry_t ) ];

typedef struct ExInfo {
        ExInfo_CONNECT_BOOT_SERVER_t *connectBootServer_;
        ExInfo_CONNECT_BOOT_SERVER_REPLY_t *connectBootServerReply_;
        ExInfo_DISCONNECT_BOOT_SERVER_t *disconnectBootServer_;
        ExInfo_UPDATE_SHARED_FILES_t * updateSharedFile_;
        ExInfo_QUERY_t * query_;
        ExInfo_QUERY_HIT_t * queryHit_;
        ExInfo_CONNECT_TRACKER_t *connectTracker_;
        ExInfo_SELECT_TRACKER_t *selectTracker_;
        ExInfo_SELECT_TRACKER_ACK_t *selectTrackerAck_;
        ExInfo_CONNECT_TRACKER_ACK_t *connectTrackerAck_;
	ExInfo_REQUEST_START_SEEDER_t *requestStartSeeder_;
	ExInfo_UPDATE_PEERS_t *updatePeers_;
	ExINFO_UPDATE_PEERS_REPLY_t *updatePeersReply_;
	ExInfo_FILE_TRANS_t *fileTrans_;
	ExInfo_HAS_FINISHED_DOWNLOADING_t *finishedDownloading_;

        ExInfo () {
                connectBootServer_ = NULL;
                connectBootServerReply_ = NULL;
                disconnectBootServer_ = NULL;
                updateSharedFile_ = NULL;
                query_ = NULL;
                queryHit_ = NULL;
                connectTracker_ = NULL;
                selectTracker_ = NULL;
                selectTrackerAck_ = NULL;
                connectTrackerAck_  = NULL;
		requestStartSeeder_ = NULL;
		updatePeers_ = NULL;
		updatePeersReply_ = NULL;
		fileTrans_ = NULL;
		finishedDownloading_ = NULL;
        }
        ~ExInfo() {
                if ( connectBootServer_ ) { delete connectBootServer_; }
                if ( connectBootServerReply_ ) { delete connectBootServerReply_; }
                if ( disconnectBootServer_ ) { delete disconnectBootServer_; }
                if ( updateSharedFile_ ) { delete updateSharedFile_;}
                if ( query_ ) { delete query_; }
                if ( queryHit_ ) { delete queryHit_; }
                if ( connectTracker_ ) { delete connectTracker_; }
                if ( selectTracker_ ) { delete selectTracker_; }
                if ( selectTrackerAck_ ) { delete selectTrackerAck_; }
                if ( connectTrackerAck_ ) {  delete connectTrackerAck_; }
                if ( requestStartSeeder_ ) { delete requestStartSeeder_;	}
		if ( updatePeers_ ) {	delete updatePeers_;	}
		if ( updatePeersReply_ ) { delete updatePeersReply_; }
		if ( fileTrans_ ) { delete fileTrans_; }
		if ( finishedDownloading_ ) { delete finishedDownloading_; }
        }

} ExInfo_t;

#endif