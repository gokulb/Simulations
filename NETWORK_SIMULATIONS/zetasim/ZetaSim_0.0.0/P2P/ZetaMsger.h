/* ZetaMsger.h
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

#ifndef NS_ZETA_MSGER_H
#define NS_ZETA_MSGER_H

#include <iostream>
#include <stdlib.h>
#include <string>

#include "TypeDefine.h"
#include "ExtendInfo.h"
#include "P2PMsger.h"
#include "ZetaPacket.h"
#include "nssocket.h"

class ZetaApp;
class ZetaMsger : public P2PMsger
{
public:

        ZetaMsger() : P2PMsger() {}
        ZetaMsger ( ZetaApp* app );
        ZetaMsger ( ZetaApp* app, Socket* a );
        ~ZetaMsger();
        // -------------------------------------------
        // -- messagers functions called by ZetaApp;--
        //--------------------------------------------
	// ---------------for average nodes-----------
        void ConnectBootServer();
        void DisConnectBootServer ( NodeAddr_t addr, std::string reason );
        void Query ( FileTag_t tag );
        void UpdatePeers();
        void UpdateSharedFiles();
	// messager function for seeder in particular
        void UpdatePeers ( NodeAddr_t addr, Port_t port, FileTag_t tag );
        void QueryHit ( FileTag_t tag, NodeAddr_t addr, Port_t port, NodeInfoEntry_t ap );
        // Messager function called in downloader
        void ConnectTracker ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeState_t state, std::string name, int maxUploadSpeed, int nodeType );
        void SendLinearComb ( FileTag_t tag, NodeAddr_t addr, Port_t port, Block *blk, Parameter para );
        void HasFinishedDownloading ( FileTag_t tag, NodeAddr_t desAddr, Port_t desPort );
	
        // -------------messager function for Boot Server----------------
        void ConnectBootServerReply ( NodeAddr_t addr, Port_t port );
        void UpdateSharedFilesAck ( NodeAddr_t addr, Port_t port );
        void QueryNotHit ( NodeAddr_t addr, Port_t port );
        void SelectTracker ( FileTag_t tag,  NodeAddr_t addr, Port_t port, NodeInfoEntry_t ap );
        void RequestStartSeeder ( FileTag_t tag, NodeAddr_t addr, Port_t port, NodeInfoEntry& tracker );
        // -----------messager function for tracker in particular----------
        void SelectTrackerAck ( FileTag_t tag, NodeAddr_t addr, Port_t port );
        void ConnectTrackerAck ( FileTag_t tag, NodeAddr_t addr, Port_t port, Parameter para );
        void UpdatePeersReply ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeSet_t peerSet );

	// ---------------------------------------------
        // --upcall functions called by NSSOCKET Class--
	// ---------------------------------------------
        int upcall_recv ( Socket *sock, Packet *pkt );

	// -------------------------------------------
        // --         Set And Get Functions        ---
	// -------------------------------------------
        void SetApp ( ZetaApp *app );
        void SetFirewalled ( bool firewall );

        ZetaPacket * GetPktHandler() { return pktHandler_; }
        Socket* GetSocketP() { return udpSock_;  };
        ZetaApp *GetApp() {  return app_;    }

protected: //local functions
        virtual int command ( int argc, const char*const* argv );

        Port_t GenPortNum () { // Generate a unused port number;
                return ( Port_t ) 0; // for test, use 8888 port;
        }
        void SendMsg ( NodeAddr_t addr, Port_t port, MsgType_t msg, PacketData* pktData );
        void Dispatch ( Socket *sock, Packet *pkt );
        void debug ( DebugLevel level, const char *fmt, ... );

private:
        // socket used to send connectionless packet
        Socket* udpSock_;

        // local infomation
        bool firewalled_;

        ZetaApp *app_;
        ZetaPacket * pktHandler_;


};

#endif
