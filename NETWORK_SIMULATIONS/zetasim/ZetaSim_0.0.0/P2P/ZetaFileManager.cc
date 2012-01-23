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

#include "ZetaFileManager.h"
#include "ZetaApp.h"

void UpdatePeersTimer::expire ( Event* e )
{
        seeder_->UpdatePeers();
        resched ( seeder_->GetUpdatePeersInterval() );
}

void SendPacketTimer::expire ( Event* e )
{
        seeder_->SendPacket();
	if (seeder_->GetMaxUploadSpeed() == 0) {
		std::cout << "MaxUploadSeed is 0" << std::endl;	
		return;
	}
	int pktSize = sizeof(ExInfo_FILE_TRANS_t) +
			seeder_->para_.blockSize +
			seeder_->para_.codeWordSize * seeder_->para_.blockNumInData;
        resched ( pktSize / seeder_->GetMaxUploadSpeed() );
}

///////////////////////////////////////////////////////
//		Downloader::Methods                 //
///////////////////////////////////////////////////////
void Downloader::ConnectTracker()
{
        msger_->ConnectTracker ( trackerAddr_, trackerPort_, tag_, app_->GetNodeState(), app_->GetName(), app_->GetMaxUploadSpeed(), DOWNLOADER);
}

bool Downloader::IsRelated ( CodeWord_t* c, Matrix& coefMatrix)   // if the two are not related this Function will change the coef.
{
	debug(DEBUG, "Enter Downloader::IsRelated()\n");
	//PrintCoef(coefMatrix);
	//debug(DEBUG, "line32\n");
        if ( coefMatrix.mx == NULL ) {
                return false;
        }
        coefMatrix.InsertRow ( c, para_.blockNumInData );
	debug(DEBUG, "In Downloader::IsRelated(), After InsertRow\n");
        if ( coefMatrix.IsRelated() == false ) {
                return false;
        }
        coefMatrix.EraseLastRow();// back track to the last state.
        return true;
}

void Downloader::Recv ( Block *blk, NodeInfoEntry_t node ) {
	debug(INFO, "Downloader::Recv() from Node [%s] \n", addrNameMap.find(node.addr_)->second.alias_.c_str());
	if ( state_ != PS_ACTIVE ) {
		debug ( DEBUG, "Downloader::Recv, the state of Downloader is offline\n" );
		return;
	}
	debug ( DEBUG, "Downloader::Recv the state of Downloader is active\n");
	if ( !IsRelated ( blk->coef_, file_->coefMat_ ) ) {
		//debug(DEBUG, "Downloader::Recv() has excuted to line 82\n");
		file_->data_.push_back ( blk );
		debug ( DEBUG, "Number of Recved Useful Pkt: %d\n",file_->data_.size());
		//debug(DEBUG, "Downloader::Recv() has excuted to line 84\n");
		file_->PreDecode();
		//debug(DEBUG, "Downloader::Recv() has excuted to line 86\n");
	} else {
		debug ( DEBUG, "Downloade::Recv() Node [%s] recv a pkt from Node [%s], the pkt is useless\n", app_->alias_.c_str(),  addrNameMap.find(node.addr_)->second.alias_.c_str());
		delete blk; //Need to Be revised in Later Version. Memery Linkage.
	}
	int PktNum = (int) file_->data_.size();
	int TotalPktNum = para_.blockNumInData;

	if ( (PktNum == TotalPktNum)  && ( finishedDownloadFlag_ == false )) {
		debug ( INFO, "Downloader in Node [%s] for File [%d] has finished downloading\n", app_->alias_.c_str(), tag_);
		finishedDownloadFlag_ = true;
		msger_->HasFinishedDownloading ( tag_, trackerAddr_, trackerPort_ );
	}
	downRatio_ = ( ( double ) PktNum ) / ( ( double ) TotalPktNum );
	if ( ( downRatio_ >= app_->startSeederTh_ ) && ( hasCreatedSeederFlag_ == false ) ){
		hasCreatedSeederFlag_ = true;
		manager_->CreateSeeder(tag_, trackerAddr_, trackerPort_, para_, file_);
	}

}

///////////////////////////////////////////////////////
//		Seeder::Methods                      //
///////////////////////////////////////////////////////

Seeder::Seeder ( ZetaApp* app, ZetaMsger *msger, ZetaFileManager *manager, FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort, Parameter para)
	: FileObject ( app, msger, manager, tag, trackerAddr, trackerPort),
	up_timer ( this ),
	send_timer ( this )
{
	up_timer.sched ( 0 );
	send_timer.sched ( 0.1 );
	para_ = para;
	para_.Update();
}

double Seeder::GetMaxUploadSpeed() {
	//debug(DEBUG, "app_->GetMaxUploadSpeed(): %f\n", app_->GetMaxUploadSpeed());
	return app_->GetMaxUploadSpeed();
}

double Seeder::GetUpdatePeersInterval() {
	return app_->GetUpdatePeersInterval();
}

void Seeder::InitFile () {
	BYTE *data = ( BYTE * ) malloc ( para_.dataSize );
	int i;
	for ( i = 0; i < para_.dataSize; i++ ) {
		data[i] = 'a' + i;
	}
	file_ = Data2File(data);
}

File* Seeder::Data2File ( BYTE *data )
{
        File* f = new File ( para_.blockNumInData );
        BYTE* di;
        int i = 0;
        for ( di = data; di < data + para_.dataSize; di += para_.blockSize ) {
                BYTE* ci;
                Block *bp = new Block ( para_.blockNumInData, para_.wordNumInBlock );
                if ( bp == NULL ) {
                        printf ( "bp == NULL \n" );
                }
                bp->data_ = ( CodeWord_t* ) di;
                //printf("Data2File: *(bp->data_) %c\n", *((char*)bp->data_));

                bp->coef_[i] = 1;
                i++;

                f->data_.push_back ( bp );
                f->coefMat_.InsertRow ( bp->coef_, para_.blockNumInData );
        }
        return f;

}
        
void Seeder::SendPacket()
{
	//debug(INFO, "Seeder::SendPacket() in Node [%s]\n", app_->alias_.c_str());
	ID2NodeInfo_t::iterator ni;
	Block *bp = GetLinearComb();
	//debug(DEBUG, "Has Gotten the LinearComb\n");
	// This Version is with Same Probability
	if ( peerMap_.size() == 0) {
		return;
	}
	NodeInfoEntry info = GetDes();
	if ( info.addr_ == app_->GetLocalAddr() ) {
		return;
	}
	//debug(DEBUG, "Has Gotten the Address of destion\n");
	debug(DEBUG, "Seeder::SendPacket(), info.addr_: %d, info.port_: %d\n", info.addr_, info.port_);
	msger_->SendLinearComb(tag_, info.addr_, info.port_, bp, para_);
}

void Seeder::UpdatePeers()
{
	//debug(INFO, "Seeder::UpdatePeers in Node [%s]\n", app_->alias_.c_str());
	peerMap_.clear();
        if ( trackerAddr_ == app_->GetLocalAddr() ) {
                NodeSet_t& downloadingPS = app_->SearchDownloadingPST ( tag_ );
                NodeSet_t::iterator nsi;
                for ( nsi = downloadingPS.begin(); nsi != downloadingPS.end(); nsi++ ) {
                        NodeInfoEntry_t nodeInfo = *nsi;
                        if ( nodeInfo.addr_ == app_->GetLocalAddr() ) {
                                continue;
                        }
                        peerMap_.insert ( pair<NodeID, NodeInfoEntry_t> ( nodeInfo.addr_, nodeInfo ) );
                }

        } else {
		msger_->UpdatePeers ( trackerAddr_, trackerPort_, tag_ );
	}
}

void Seeder::RecvUpdatePeerReply ( NodeAddr_t addr, Port_t port, NodeSet_t downloadingPeerSet )
{
        if ( addr != trackerAddr_ ) {
                std::cout << "[ERROR in Seeder], Seeder for file ["
                          << tag_ << "]" << "in Node [" << app_->alias_
                          << "], receive a wrong RecvUpdateReerReply msg"<< std::endl;
                return;

        }
        debug(DEBUG, "Seeder::RecvUpdatePeerReply() the size of peerset is: %d\n", downloadingPeerSet.size());
        NodeSet_t::iterator nsi;
	int i =0;
        for ( nsi = downloadingPeerSet.begin(); nsi != downloadingPeerSet.end(); nsi++ ) {
                NodeInfoEntry_t nodeInfo = *nsi;
// 		if (nodeInfo.addr_ == app_->GetLocalAddr()) {
// 			continue;
// 		}
		//printf("Seeder::RecvUpdatePeerReply() i: %d nodeInfo.addr_: %d\n", i, nodeInfo.addr_);
		i++;
		//printf("");
		peerMap_.insert ( pair<NodeID, NodeInfoEntry_t> ( nodeInfo.addr_, nodeInfo ) );
//                 std::cout << "[INFO in Seeder], NodeInfo addr:"
//                           << nodeInfo.addr_ << " port: "
//                           << nodeInfo.port_ << " maxUploadSpeed: "
//                           << nodeInfo.maxUploadSpeed_ << std::endl;
        }
        //printf("\m");
        debug(DEBUG, "Seeder::RecvUpdatePeerReply() the size of peerMap_ is: %d\n", peerMap_.size());
        return;
}

Block* Seeder::GetLinearComb ()
{
        Block* nbp = new Block ( para_.blockNumInData, para_.wordNumInBlock );
        //printf("line 555\n");
        BlockPVec_t::iterator vi;
        Block *bp;
        CodeWord_t randCoef;
	int i = 0;
	//printf("file Size: %d\n", file->data_.size());
        for ( vi = file_->data_.begin(); vi != file_->data_.end(); vi++ ) {
		i++;
// 		printf("Block [%d]\n", i);
                bp = *vi;
                randCoef = GenRand ( para_.codeWordSize * 8 );

                RegionMultiply ( bp->data_,
                                 randCoef,
                                 para_.blockSize,
                                 nbp->data_,
                                 1,
                                 para_.codeWordSize );

                RegionMultiply ( bp->coef_,
                                 randCoef,
                                 para_.codeWordSize * para_.blockNumInData,
                                 nbp->coef_,
                                 1,
                                 para_.codeWordSize );
        }
        return nbp;
}
void Seeder::ConnectTracker() {
	msger_->ConnectTracker ( trackerAddr_, trackerPort_, tag_, app_->GetNodeState(), app_->GetName(), app_->GetMaxUploadSpeed(), SEEDER);
}

NodeInfoEntry& Seeder::GetDes() { // In this version, send Pkt with same probability
		int num = peerMap_.size();
		if (num == 0) {
			debug(INFO, "the size of peerMap is %d\n", num);
			exit(1);
		}
		int seq = rand() % num;
		debug(DEBUG, "num: %d, seq: %d\n", num, seq);
		ID2NodeInfo_t::iterator ni = peerMap_.begin();
		for (int i = 0; i < seq; i++) {
			ni++;
		}
		return ni->second;
}


///////////////////////////////////////////////////////
//		FileManager::Methods                 //
///////////////////////////////////////////////////////
 ZetaFileManager::ZetaFileManager ( ZetaApp* app, ZetaMsger* msger ) {
                app_ = app;
                msger_ = msger;
}

ZetaFileManager::~ZetaFileManager () {
	for ( di = dm_.begin(); di != dm_.end(); di++ ) {
		di->second->~Downloader();
	}
	for ( si = sm_.begin(); si != sm_.end(); si++ ) {
		si->second->~Seeder();
	}
}

 void ZetaFileManager::RecvUpdatePeersReply ( NodeAddr_t addr, Port_t port, FileTag_t tag, NodeSet_t set, int num ) {
                Seeder* s = GetSeederP ( tag );
                s->RecvUpdatePeerReply ( addr, port, set );
        }
void ZetaFileManager::DeleteDownloader ( FileTag_t tag )
{
        di = dm_.find ( tag );
        di->second->~Downloader();
        dm_.erase ( di );
}

void ZetaFileManager::DeleteSeeder ( FileTag_t tag )
{
        si = sm_.find ( tag );
        si->second->~Seeder();
        sm_.erase ( si );
}

void ZetaFileManager::CreateDownloader ( FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort)
{
	//Parameter para(codeWordSize, blockSize, dataSize);
	debug(DEBUG, "Enter in ZetaFileManager::CreateDownloader \n");
	Downloader *fd = new Downloader ( app_, msger_, this, tag, trackerAddr, trackerPort);
	fd->ConnectTracker();
	dm_.insert ( pair<FileTag_t, Downloader*> ( tag, fd ) );
	debug ( INFO,
		"A downloader has been created from file with FileTag %d, tracker addr is %d, port is %d\n",
		tag,
		trackerAddr,
		trackerPort );
}


void ZetaFileManager::CreateSeeder ( FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort)
{
	//Parameter para(codeWordSize, blockSize, dataSize);
        Seeder *fs = new Seeder ( app_, msger_, this, tag, trackerAddr, trackerPort, app_->para_);
	fs->NewFile();
	fs->InitFile();
        sm_.insert ( pair<FileTag_t, Seeder*> ( tag, fs ) );
//         debug ( INFO,
//                 "A seeder has been created in Node [%s] for file with FileTag %d, tracker addr is %d, port is %d\n",
//                 app_->alias_.c_str(),
//                 tag,
//                 trackerAddr,
//                 trackerPort );
}

void ZetaFileManager::CreateSeeder ( FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort, Parameter para, File* file )  // called by downloader
{
        Seeder *fs = new Seeder ( app_, msger_, this,  tag, trackerAddr, trackerPort, para);
	fs->SetFile(file);
        sm_.insert ( pair<FileTag_t, Seeder*> ( tag, fs ) );
}

void ZetaFileManager::RecvFileTransBlock (FileTag_t tag, Parameter para, NodeInfoEntry_t srcInfo, Block *blk)
{
	debug(DEBUG, "ZetaFileManager::RecvFileTransBlock()\n");
	//Block *bp = new Block(para.blockNumInData, para.wordNumInBlock);
	GetDownloaderP(tag)->Recv(blk, srcInfo);
}

void ZetaFileManager::RecvConnectTrackerAck ( FileTag_t tag, Parameter para ) {
	Downloader *fd = GetDownloaderP ( tag );
	fd->SetPara ( para );
	RunState_t s = fd->NewFile();
	if ( s == FAIL ) {
		debug ( ERROR, "ZetaFileManager::RecvConnectTrackerAck unable to create a new file in Node [%s]\n", app_->alias_.c_str());
	}
	fd->state_ = PS_ACTIVE;
	debug ( INFO, "Node [%s] 's Connection to tracker has been acked\n",  app_->alias_.c_str());
}

Downloader* ZetaFileManager::GetDownloaderP ( FileTag_t tag )
{
        di = dm_.find ( tag );
        if ( di == dm_.end() ) {
                return NULL;
        }
        return ( di->second );
}

Seeder* ZetaFileManager::GetSeederP ( FileTag_t tag )
{
        si = sm_.find ( tag );
        if ( si == sm_.end() ) {
                return NULL;
        }
        return ( si->second );
}

void ZetaFileManager::debug ( DebugLevel level, const char *fmt, ... )
{
        if ( level > app_->debugLevel )
                return;
        switch ( level ) {
        case ERROR:
                printf ( "[ERROR in ZetaFileManager]" );
                break;
        case STAT:
                printf ( "[STAT in ZetaFileManager]" );
                break;
        case WARNING:
                printf ( "[WARNING in ZetaFileManager]" );
                break;
        case INFO:
                printf ( "[INFO in ZetaFileManager]" );
                break;
        case DEBUG:
                printf ( "[DEBUG in ZetaFileManager]" );
                break;
        }
        printf ( " "TIME_FORMAT"s ", Scheduler::instance().clock() );
        va_list ap;
        va_start ( ap, fmt );
        vprintf ( fmt, ap );
}

///////////////////////////////////////////////////////
//		FileObject::Methods                 //
///////////////////////////////////////////////////////

FileObject::FileObject ( ZetaApp* app, ZetaMsger *msger, ZetaFileManager *manager, FileTag_t tag, NodeAddr_t trackerAddr, Port_t trackerPort )
{
        app_ = app;
        msger_ = msger;
	manager_ = manager;
        tag_ = tag;
        trackerAddr_ = trackerAddr;
        trackerPort = trackerPort;
}

void FileObject::debug ( DebugLevel level, const char *fmt, ... )
{
        if ( level > app_->debugLevel )
                return;
        switch ( level ) {
        case ERROR:
                printf ( "[ERROR in FileObject]" );
                break;
        case STAT:
                printf ( "[STAT in FileObject]" );
                break;
        case WARNING:
                printf ( "[WARNING in FileObject]" );
                break;
        case INFO:
                printf ( "[INFO in FileObject]" );
                break;
        case DEBUG:
                printf ( "[DEBUG in FileObject]" );
                break;
        }
        printf ( " "TIME_FORMAT"s ", Scheduler::instance().clock() );
        va_list ap;
        va_start ( ap, fmt );
        vprintf ( fmt, ap );
}

