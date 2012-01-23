//#include "packet.h"
#ifndef ZETA_PACKET_H
#define ZETA_PACKET_H

#include "TypeDefine.h"
#include "ExtendInfo.h"
#include <set>
#include "nssocket.h"


// **********************************************************************
// * The header of the Zeta Messager.
// * 	1. MsgType_t: The Type of the Message, it is a enum varible.
// *	2. NodeAddr_t: The Address of the source node.
// *	3. MsgID_t: It is a unique ID which can help distinguish messages.
// ***********************************************************************
typedef struct hdr_zeta { // MsgHdr_t
public:
        inline static int& offset() {
                return offset_;
        }
        inline static hdr_zeta* access ( Packet* p ) {
                return ( hdr_zeta* ) p->access ( offset_ );
        }
        inline MsgType_t GetMsgType() {
                return type_;
        }
        inline NodeAddr_t GetSrcAddr() {
                return addr_;
        }
        inline MsgID_t *GetMsgIDP() {
                return &id_;
        }

        static int offset_;
        MsgType_t type_;
        NodeAddr_t addr_;
        MsgID_t id_;
} MsgHdr_t; // This is the header of a Zeta Message





//***************************************
//* Class ZetaPacket is in charge of:
//* 	1. Creating of a New Packet.
//*	2. Parsing a received Packet.
//**************************************
class ZetaPacket : public Agent
{
public:
        ZetaPacket ( ZetaMsger* msger );
        // API for ZetaMsger
        Packet *NewPacket ( MsgType_t type, NodeAddr_t addr, PacketData *data );
        Packet *NewPacket ( MsgType_t type, NodeAddr_t addr ) {
                return NewPacket ( type, addr, NULL );
        }
        PacketData *NewPacketData ( MsgType_t type, NodeAddr_t addr, int len, BYTE *data );
        PacketData *NewPacketData ( MsgType_t type, NodeAddr_t addr ) {
                return NewPacketData ( type, addr, 0, NULL );
        }
        PacketData *NewPacketData ( std::string str );
        PacketData *NewPacketDataConnectBootServer ( std::string str, int maxUploadSpeed );
        PacketData *NewPacketDataConnectBootServerReply ( std::string str );
        PacketData *NewPacketDataDisConnectBootServer(std::string reason);
        PacketData *NewPacketDataUpdateSharedFiles ( FileMap_t* fm, FileOperation_t op );
        PacketData *NewPacketDataUpdateSharedFilesAck();
        PacketData *NewPacketDataQuery();
        PacketData *NewPacketDataQueryNotHit() {
                return NewPacketData ( "Query Not Hit" );
        }
        PacketData *NewPacketDataQueryHit();
        PacketData *NewPacketDataSelectTracker ( FileTag_t tag, NodeInfoEntry_t ap );
        PacketData *NewPacketDataSelectTrackerAck ( FileTag_t tag );
	PacketData *NewPacketDataConnectTrackerAck(FileTag_t tag, Parameter para);
	PacketData *NewPacketDataRequestStartSeeder(FileTag_t tag, NodeInfoEntry& tracker);
	PacketData *NewPacketDataUpdatePeers(FileTag_t tag);
	PacketData *NewPacketDataUpdatePeersReply(FileTag_t tag, NodeSet_t peerSet);

        //PacketData *NewPacketDataConnectTracker();
	PacketData *NewPacketDataConnectTracker(NodeInfoEntry, int nodeType);
	PacketData *NewPacketDataLinearComb(FileTag_t tag, NodeAddr_t addr, Port_t port, Block* blk, Parameter para, NodeInfoEntry_t *localInfo);
	PacketData *NewPacketDataHasFinishedDownloading ( FileTag_t tag );

        MsgType_t Parse ( Packet *pkt ) {
                hdr_zeta* h = hdr_zeta::access ( pkt );
                return h->GetMsgType();
        }
        ExInfo_t *GetExtendInfo ( MsgType_t type, int len, PacketData *pktData );
        NodeAddr_t GetSrcAddr ( Packet *pkt );
        NodeAddr_t GetSrcPort ( Packet *pkt );
        void SetFileTag ( FileTag_t tag ) {
                tag_ = tag;
        }
        void SetReason ( std::string reason ) {
                reason_ = reason;
        }
        void SetAddrPortSet ( NodeSet_t as ) {
                as_ = as;
        }
        void SetTracker ( NodeInfoEntry_t ap ) {
                tracker_ = ap;
        }

        static const int hdrLen_ = sizeof ( MsgHdr_t );
protected:
        void debug ( DebugLevel level, const char *fmt, ... );

        BYTE *NewMsgID();
        Socket *udpSock_;
        int seq1_, seq2_, seq3_;
        MsgHdr_t header_;
        MsgType_t type_;
        NodeAddr_t addr_;

        // Hold Parameters;
        FileTag_t tag_; // tag of searching file. used in search
        std::string reason_; // reason for disconnection, used in Disconnection
        NodeSet_t as_; // used in
        NodeInfoEntry_t tracker_; // used in QueryHit.

        ZetaMsger *msger_;
};

#endif
