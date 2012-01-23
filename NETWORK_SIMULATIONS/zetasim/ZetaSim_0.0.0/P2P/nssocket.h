/* nssocket.h
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
#ifndef NS_SOCKET_H
#define NS_SOCKET_H

#include "TypeDefine.h"
#include "udp.h"

#include <iostream>

#define MIN_INTERVAL_BETWEEN_TWO_PACKETS 0.002
typedef NodeAddr_t IpAddress;
typedef Port_t Port;

class ZetaMsger;
class NSSocket : public UdpAgent { // this socket APIs are based on UDP protocol, need to be changed in later version
public:
    NSSocket () : UdpAgent(PT_ZETA) {
        bind("packetSize_", &size_);
        bind("dst_addr_", &dst_.addr_);
        bind("dst_port_", &dst_.port_);
        bind("agent_addr_", &here_.addr_);
        bind("agent_port_", &here_.port_);
    }
    NSSocket (Application *app ) : UdpAgent(PT_ZETA) {
        app_ = app;
        bind("packetSize_", &size_);
        bind("dst_addr_", &dst_.addr_);
        bind("dst_port_", &dst_.port_);
        bind("agent_addr_", &here_.addr_);
        bind("agent_port_", &here_.port_);
    }
   
    RunState_t Bind (NodeAddr_t addr, Port_t port);
    void connect (NodeAddr_t addr, Port_t port);
    void SendPkt(Packet *p, Handler* h,  const char* flags =0);
    void SendPktTo (Packet *p, std::string name);
    void SendPktTo (Packet* pkt, NodeAddr_t peerAddress, Port_t peerPort);
    void SendMsgTo (unsigned int nbytes, PacketData* data, NodeAddr_t addr, Port_t port);
    virtual void recv (Packet *pkt, Handler*);

    void SetMsger (ZetaMsger* msger) { msger_ = msger; }
    ZetaMsger *GetMsger() { return msger_; }
protected:
    virtual int command(int argc, const char*const* argv) {
        return ( UdpAgent::command(argc, argv) );
    }
    void debug(DebugLevel level, const char *fmt, ...);

protected:
    ZetaMsger *msger_;
    double schedulerSendTime;
};
typedef NSSocket Socket;

static class NSSocketClass : public TclClass {
public:
    NSSocketClass() : TclClass("Agent/NSSocket") {}
    TclObject* create(int, const char* const*) {
        return (new NSSocket());
    }
} class_ns_socket;
#endif
