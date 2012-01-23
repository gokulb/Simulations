/* ZetaFileManager.h
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

#ifndef ZETA_FILE_MANAGER_H
#define ZETA_FILE_MANAGER_H

#include "TypeDefine.h"
#include "nssocket.h"
#include "FiniteFieldCal.h"

extern void RegionMultiply ( CodeWord_t* data, CodeWord_t coef, int dataSize, CodeWord_t* res, int add, int codeWordSize );
extern CodeWord_t GenRand ( int fieldSize );

class Seeder;
class UpdatePeersTimer : public TimerHandler
{
public:
        UpdatePeersTimer ( Seeder* s ) : TimerHandler() {
                seeder_ = s;
        }
protected:
        Seeder *seeder_;
        virtual void expire ( Event* e );
};

class SendPacketTimer : public TimerHandler
{
public:
        SendPacketTimer ( Seeder* s ) : TimerHandler() {
                seeder_ = s;
        }
protected:
        Seeder *seeder_;
        virtual void expire ( Event* e );
};

class ZetaApp;
class ZetaMsger;
class ZetaFileManager;
class FileObject
{
public:
	FileObject ( ZetaApp* app, ZetaMsger *msger, ZetaFileManager *manager, FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort);
        ~FileObject() {}
        
        void SetPara ( Parameter para ) { para_ = para;	}
	void SetFile ( File* file) { file_ = file; }
	RunState_t NewFile ( ) {
		if ( para_.dataSize == 0 ) {
			//printf("FileObject::Newfile() para_.dataSize:%d\n", para_.dataSize);
			return FAIL;
		}
		para_.Update();
		file_ = new File(para_.blockNumInData);
		if (!file_) {
			//printf("FileObject::Newfile() don't has enough memory\n");
			return FAIL;
		}
		return SUCCESS;
	}
	void DeleteFile() { delete [] file_; };
	Parameter para_;
protected:
        void debug ( DebugLevel level, const char *fmt, ... );

        ZetaApp *app_;
        ZetaMsger *msger_;
	ZetaFileManager *manager_;
        FileTag_t tag_;
        NodeAddr_t trackerAddr_;
        Port_t trackerPort_;
        File* file_;
};

typedef list<Packet*> PktList_t;

class Downloader : public FileObject
{
public:
        Downloader ( ZetaApp *app, ZetaMsger *msger, ZetaFileManager *manager, FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort)
        : FileObject ( app, msger, manager, tag, trackerAddr, trackerPort) {
		state_ = PS_OFFLINE;
		finishedDownloadFlag_ = false;
		hasCreatedSeederFlag_ = false;
	}
        ~Downloader() { }

        void ConnectTracker();
        void Recv ( Block *blk, NodeInfoEntry_t node );
        bool IsRelated ( CodeWord_t* c, Matrix& coefMatrix );
	NodeState_t state_;
	
protected:
        //PktList_t buffer;
	bool finishedDownloadFlag_;
	bool hasCreatedSeederFlag_;
	double downRatio_;
};

class Seeder : public FileObject
{
public:
        Seeder ( ZetaApp* app, ZetaMsger *msger, ZetaFileManager *manager, FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort, Parameter para);
        ~Seeder() {}
        void SendPacket();
        //void SearchPath();
        void UpdatePeers();
        void RecvUpdatePeerReply ( NodeAddr_t addr, Port_t port, NodeSet_t set );
        inline double GetMaxUploadSpeed();
        inline double GetUpdatePeersInterval();

        Block* GetLinearComb ();
	NodeInfoEntry& GetDes();
	void ConnectTracker();

        void InitFile ();
        File* Data2File ( BYTE *data );

protected:
        FilePath_t path_;
        UpdatePeersTimer up_timer;
        SendPacketTimer send_timer;
        //PeerMap_t peerMap_;
        ID2NodeInfo_t peerMap_;
};

typedef map<FileTag_t, Downloader*> DownloaderMap_t;
typedef map<FileTag_t, Seeder*> SeederMap_t;

class ZetaFileManager
{
public:
        ZetaFileManager ( ZetaApp* app, ZetaMsger* msger );
        ~ZetaFileManager ();

        void CreateDownloader ( FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort);
        void CreateSeeder ( FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort); // called by ZetaApp;
	void CreateSeeder ( FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort, Parameter para, File* file); // called by downloader
        void DeleteDownloader ( FileTag_t tag );
        void DeleteSeeder ( FileTag_t tag );
        void RecvUpdatePeersReply ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeSet_t set, int num );
        void RecvConnectTrackerAck ( FileTag_t tag, Parameter para );
        void RecvFileTransBlock (FileTag_t tag, Parameter para, NodeInfoEntry_t srcInfo, Block *blk);
protected: // method
        Downloader* GetDownloaderP ( FileTag_t tag );
        Seeder* GetSeederP ( FileTag_t tag );
protected: // variables
        ZetaApp* app_;
        ZetaMsger* msger_;
        DownloaderMap_t dm_;
        SeederMap_t sm_;

        void debug ( DebugLevel level, const char *fmt, ... );
        // for temp use;
        DownloaderMap_t::iterator di;
        SeederMap_t::iterator si;
};
#endif
