/* TypeDefine.h
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

#ifndef NS_TYPE_DEFINE_H
#define NS_TYPE_DEFINE_H

#include <string>
#include <map>
#include <set>
#include <list>
#include <iostream>
#include "string.h"
// Header File for NS
#include <stdarg.h>
#include "agent.h"
#include "node.h"
#include "packet.h"
#include "scheduler.h"
#include "tclcl.h"
#include "timer-handler.h"
#include "config.h"
#include "object.h"

#define MAX_STRING_LEN 50
//--------- for global use-------------
enum DebugLevel {
        ERROR = 0,
        STAT = 1,
        WARNING = 2,
        INFO = 3,
        DEBUG = 4
};

typedef int NodeAddr_t;  // for test
typedef struct {
        std::string name_;
        std::string alias_;
} Name_t;

typedef map<NodeAddr_t, Name_t> AddrNameMap_t;

// **************************************
// * Type Define mainly used in nssocket.h
// *************************************
typedef nsaddr_t NodeAddr_t;
typedef nsaddr_t Port_t; // nsaddr_t is int32_t in this version

typedef enum {SUCCESS, FAIL} RunState_t;
#define SEEDER 0
#define DOWNLOADER 1

typedef enum {
        PS_OFFLINE,
        PS_ACTIVE,
        PS_IDLE,
        PS_OUTSYS
} NodeState_t;

typedef struct NodeInfoEntry {
        //data
        int maxUploadSpeed_;
        NodeAddr_t addr_;
        Port_t port_;
        NodeState_t nodeState_;
        char name_[MAX_STRING_LEN];
//method
        NodeInfoEntry () {}
        NodeInfoEntry (NodeAddr_t addr, Port_t port) {
		addr_ = addr;
		port_ = port;
		maxUploadSpeed_ = 0;
		nodeState_ = PS_ACTIVE;
		memset ( name_, 0, MAX_STRING_LEN * sizeof(char) );
	}
        NodeInfoEntry ( NodeAddr_t addr, Port_t port,  NodeState_t nodeState,  std::string name, int maxUploadSpeed ) {
                addr_ = addr;
                port_ = port;
                nodeState_ = nodeState;
                maxUploadSpeed_ = maxUploadSpeed;
                if ( name.size() +1 > MAX_STRING_LEN ) {
                        std::cout << "Truncate" << std::endl;
                        strncpy ( name_, name.c_str(), MAX_STRING_LEN - 1 );
                        name_[MAX_STRING_LEN - 1] = '\0';
                } else {
                        strncpy ( name_, name.c_str(), name.size() );
                        name_[name.size() ] = '\0';
                }
        }
        void SetName(std::string name) {
		 if ( name.size() +1 > MAX_STRING_LEN ) {
                        std::cout << "Truncate" << std::endl;
                        strncpy ( name_, name.c_str(), MAX_STRING_LEN - 1 );
                        name_[MAX_STRING_LEN - 1] = '\0';
                } else {
                        strncpy ( name_, name.c_str(), name.size() );
                        name_[name.size() ] = '\0';
                }	
	}

        bool operator< ( const NodeInfoEntry& other ) const {
                //return ( ( addr_< other.addr_ ) || ( ! ( other.addr_< addr_ ) && ( port_ < other.port_ ) ) );
		return addr_ < other.addr_;
        }
        bool operator== ( const NodeInfoEntry& other ) const {
		return addr_ == other.addr_;
//                 if ( ( addr_ == other.addr_ ) && ( port_ == other.port_ ) ) {
//                         return true;
//                 }
//                 return false;
        }
        NodeInfoEntry& operator= ( const NodeInfoEntry& other ) {
                if ( this != &other) {
			this->addr_ = other.addr_;
			this->port_ = other.port_;
			this->maxUploadSpeed_ = other.maxUploadSpeed_;
			this->nodeState_ = other.nodeState_;
			strcpy(this->name_, other.name_);
		}
		return *this;
        }
} NodeInfoEntry_t;

typedef set<NodeAddr_t> AddrSet_t;

typedef NodeAddr_t NodeID;
typedef map<NodeID, NodeInfoEntry> ID2NodeInfo_t;
typedef set<NodeInfoEntry> NodeSet_t;
typedef map<NodeID, NodeSet_t> ID2NodeSet_t;

typedef int FileTag_t;
typedef std::string FilePath_t;
typedef std::map<FileTag_t, FilePath_t> FileMap_t;

typedef map<FileTag_t, NodeInfoEntry_t> Tag2NodeInfo_t;
typedef map<FileTag_t, NodeSet_t> Tag2NodeSet_t;
typedef set <FileTag_t> TagSet_t;
typedef char BYTE;
typedef BYTE MsgID_t [3 * sizeof ( int ) ]; // a unique ID for each msg. future use.
#endif