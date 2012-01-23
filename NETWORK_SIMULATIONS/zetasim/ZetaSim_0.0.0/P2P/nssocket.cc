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
#include "nssocket.h"
#include "TypeDefine.h"
#include "ZetaPacket.h"
#include "ZetaMsger.h"
#include "ZetaApp.h"
#include "udp.h"
#include "rtp.h"
#include "random.h"
#include "address.h"
#include "ip.h"

#include <iostream>


extern AddrNameMap_t addrNameMap;

RunState_t NSSocket::Bind (NodeAddr_t addr1, Port_t port1) {
    debug(DEBUG, "NSSocket::Bind(), addr1: %d, port1 %d. \n", addr1, port1);
    addr() = addr1;
    port() = port1;
    debug(DEBUG, "NSSocket::Bind(), addr(): %d, port() %d. \n", addr(), port() );
    return SUCCESS;
}

/*
* this function implements a UDP protocol, and use it to send the pkt out.
*/
void NSSocket::SendPkt(Packet *p0, Handler* h,  const char* flags ) {
    Packet *p;
    int n;
    int nbytes = p0->datalen();
    PacketData *data = (PacketData*) p0->userdata();
    hdr_zeta * zh0 = hdr_zeta::access(p0);
    n = nbytes / size_;
    //std::cout << "nbytes: " << nbytes << "\tsize_" << size_ << std::endl;

    if (nbytes == -1) {
        printf("Error:  sendmsg() for UDP should not be -1\n");
        return;
    }

    // If they are sending data, then it must fit within a single packet.
    if (data && nbytes > size_) {
        printf("Error: data greater than maximum UDP packet size\n");
        return;
    }
    double local_time = Scheduler::instance().clock();

    while (n-- > 0) {
        p = allocpkt();
	hdr_zeta * zh = hdr_zeta::access(p);
	*zh = *zh0;
	
        hdr_cmn::access(p)->size() = size_;
        hdr_rtp* rh = hdr_rtp::access(p);
        rh->flags() = 0;
        rh->seqno() = ++seqno_;
        hdr_cmn::access(p)->timestamp() =
            (u_int32_t)(SAMPLERATE*local_time);
        // add "beginning of talkspurt" labels (tcl/ex/test-rcvr.tcl)
        if (flags && (0 ==strcmp(flags, "NEW_BURST")))
            rh->flags() |= RTP_M;
        p->setdata(data);
	
        target_->recv(p);
    }
    n = nbytes % size_;
    if (n > 0) {
        p = allocpkt();
	hdr_zeta * zh = hdr_zeta::access(p);
	*zh = *zh0;
	
        hdr_cmn::access(p)->size() = n;
        hdr_rtp* rh = hdr_rtp::access(p);
        rh->flags() = 0;
        rh->seqno() = ++seqno_;
        hdr_cmn::access(p)->timestamp() =
            (u_int32_t)(SAMPLERATE*local_time);
        // add "beginning of talkspurt" labels (tcl/ex/test-rcvr.tcl)
        if (flags && (0 == strcmp(flags, "NEW_BURST")))
            rh->flags() |= RTP_M;
        p->setdata(data);
	
        //hdr_ip *iph = hdr_ip::access(p);
        target_->recv(p);
    }
    idle();
}


void NSSocket::SendPktTo (Packet *p, std::string name) {
    Tcl& tcl = Tcl::instance();
    NSSocket *s = (NSSocket *) tcl.lookup( name.c_str() );
    target( s->target() );
    SendPkt(p, NULL, 0);
}


void NSSocket::SendPktTo (Packet* pkt, NodeAddr_t peerAddress, Port_t peerPort) {
	const char* ch = addrNameMap.find(peerAddress)->second.name_.c_str();	
        Tcl &tcl = Tcl::instance();
        tcl.evalf("[Simulator instance] connect %s %s", name(), ch);
        SendPkt(pkt, NULL, 0);   
}

/*
*	\brief this overloaded function calls uploaded functions according to the received packet.
*/
void NSSocket::recv (Packet *pkt, Handler*) {
    if (msger_) {// if messager that open this socket is valid. Transfer the data to msger
        //hdr_cmn *h = hdr_cmn::access(pkt);
        //app_->process_data(h->size(), pkt->userdata());
        hdr_ip *h = hdr_ip::access(pkt);
        NodeAddr_t desAddrInPkt = h->daddr();
        Port_t desPortInPkt = h->dport();

        hdr_ip* iph = hdr_ip::access(pkt);
//         debug(DEBUG,
// 	      "Node %s receive a pakcet. src ip: [%d] port: [%d] | des ip: [%d] port: [%d] | local ip:[%d] port: [%d] \n", 
// 	      msger_->GetApp()->alias_.c_str(), 
// 	      iph->src_.addr_, 
// 	      iph->src_.port_, 
// 	      iph->dst_.addr_, 
// 	      iph->dst_.port_, 
// 	      here_.addr_, 
// 	      here_.port_);

        if ( (desAddrInPkt == here_.addr_) &&
                (desPortInPkt == here_.port_) ) {
            msger_->upcall_recv(this, pkt);
        } else {
            debug(DEBUG, "Received a Pkt with with wrong port number\n");
        }

    } else {
	debug(ERROR, "Error in nssocket.cc: A socket without corresponding messager\n");
    }
}

void NSSocket::debug(DebugLevel level, const char *fmt, ...) {
      if (level > msger_->GetApp()->debugLevel)
            return;
        switch (level) {
        case ERROR:
            printf("[ERROR in NSSocket]");
            break;
        case STAT:
            printf("[STAT in NSSocket]");
            break;
        case WARNING:
            printf("[WARNING in NSSocket]");
            break;
        case INFO:
            printf("[INFO in NSSocket]");
            break;
        case DEBUG:
            printf("[DEBUG in NSSocket]");
            break;
        }
        printf(" "TIME_FORMAT"s ", Scheduler::instance().clock());
        va_list ap;
        va_start(ap, fmt);
        vprintf(fmt, ap);
}
