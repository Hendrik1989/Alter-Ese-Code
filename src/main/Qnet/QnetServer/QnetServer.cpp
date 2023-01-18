/*
 * QnetServer.cpp
 *
 *  Created on: 15.12.2022
 *      Author: Dresske, Christian
 */

#include "QnetServer.h"
#include <stdio.h>
#include "../../Imports.h"

QnetServer::QnetServer(const char* attach_point, Dispatcher *disp) {
	//this->data = data;
	this ->dispatcher = disp;
	this->attach_point =attach_point;
	ServerThread = new std::thread([this]() {server();});
}

QnetServer::~QnetServer() {
}

int QnetServer::server(){

	name_attach_t *attach;
	bool serverRunning = false;
	std::mutex mut;
	int chid = 0;


		if(USE_GNS){
			if ((attach = name_attach(NULL, attach_point, NAME_FLAG_ATTACH_GLOBAL)) == NULL) {  // important!! Use "NAME_FLAG_ATTACH_GLOBAL" flag if the other machine should see it !!!
			   perror("[Server] Failed to attach name!");
		    }
			std::cout << "[Server] Channel open with name: " << attach_point << "\n";
			chid = attach->chid;

		}else{
			if((chid = ChannelCreate(0)) < 0 ){
				perror("[Server] Failed to create channel!");
			}
		}

		mut.lock();
		serverRunning = true;
		mut.unlock();

		 while (1) {

//			 if (data->getServerCounter() == 20) {
//				 data->resetServerCounter();
//			 }

			   // Waiting for a message and read first header
			   _pulse pulse;
			   int rcvid = MsgReceive (chid, &pulse, sizeof (_pulse), NULL);

		       if (rcvid == -1) { // Error occurred
		    	   perror("[Server]: MsgReceived failed!");
		           break;
		       }

		       if (rcvid == 0) {// Pulse was received
		    	   switch (pulse.code) {
		    	   	case _PULSE_CODE_DISCONNECT:
		    	   		printf("[Server] received _PULSE_CODE_DISCONNECT\n");
		    	   	    /* A client disconnected all its connections (called
		    	   	     * name_close() for each name_open() of our name) or
		    	   	     * terminated. */
		    	   	    ConnectDetach(pulse.scoid);
		    	   	    break;
		    	   	case _PULSE_CODE_UNBLOCK:
		    	   	    printf("[Server] received _PULSE_CODE_UNBLOCK\n");
		    	   	    /* REPLY blocked client wants to unblock (was hit by
		    	   	     * a signal or timed out).  It's up to you if you
		    	   	     * reply now or later. */
		    	   	    break;

					//nur zum Testen
		    	   	// case LSE:
		    	   	// 	MsgSendPulse(dispatcher->getConnectionID(), -1, LSA1interrupted, 0);
		    	   	// 	break;

		    	   	case HELLO:
		    	   		std::cout << "Hallo von anderer Anlage gesendet!\n" << endl ;
		    	   	    break;

		    	   	case LSE1interrupted:
		    	   		if(FESTO_TYPE == 2) {
		    	   			MsgSendPulse(dispatcher->getConnectionID(), -1, LSE1interrupted, 0);
		    	   		}
						break;

		    	   	case LSA2interrupted:
		    	   		if(FESTO_TYPE == 1) {
		    	   			MsgSendPulse(dispatcher->getConnectionID(), -1, LSA2interrupted, 0);
		    	   		}
		    	   		break;

// ESTP
					case ESTP1interrupted:
		    	   		if(FESTO_TYPE == 2) {
		    	   			MsgSendPulse(dispatcher->getConnectionID(), -1, ESTP1interrupted, 0);
		    	   		}
		    	   		break;


		    	   	case ESTP2interrupted:
		    	   		if(FESTO_TYPE == 1) {
		    	   			MsgSendPulse(dispatcher->getConnectionID(), -1, ESTP2interrupted, 0);
		    	   		}
		    	   		break;

					case LSR1notInterrupted:
		    	   		if(FESTO_TYPE == 2) {
		    	   			MsgSendPulse(dispatcher->getConnectionID(), -1, LSR1notInterrupted, 0);
		    	   		}
		    	   		break;

					case LSR2notInterrupted:
		    	   		if(FESTO_TYPE == 1) {
		    	   			MsgSendPulse(dispatcher->getConnectionID(), -1, LSR2notInterrupted, 0);
		    	   		}
		    	   		break;

					case LSR1interrupted:
		    	   		if(FESTO_TYPE == 2) {
		    	   			MsgSendPulse(dispatcher->getConnectionID(), -1, LSR1interrupted, 0);
		    	   		}
		    	   		break;

					case LSR2interrupted:
					if(FESTO_TYPE == 1) {
						MsgSendPulse(dispatcher->getConnectionID(), -1, LSR2interrupted, 0);
					}
					break;

					case WSDATEN:
					if(FESTO_TYPE == 2) {
						MsgSendPulse(dispatcher->getConnectionID(), -1, WSDATEN, 0);
					}
					break;

//					case FBM2_SEND_WK:
//
//						int recvid = MsgReceivePulse (chid, &pulse, sizeof (_pulse), NULL);
//
//						switch (pulse.code) {
//							case FBM1_WK_FLACH: data->getWerkstueck(data->getServerCounter()).typ2 = FLACH; data->getWerkstueck(data->getServerCounter()).Hoehe2 = pulse.value.sival_int;   break;
//							case FBM1_WK_NORMAL: data->getWerkstueck(data->getServerCounter()).typ2 = NORMAL;  data->getWerkstueck(data->getServerCounter()).Hoehe2 = pulse.value.sival_int;  break;
//							case FBM1_WK_BOHRUNG: data->getWerkstueck(data->getServerCounter()).typ2 = BOHRUNG;  data->getWerkstueck(data->getServerCounter()).Hoehe2 = pulse.value.sival_int;  break;
//							case FBM1_WK_METALL: data->getWerkstueck(data->getServerCounter()).typ2 = METALL; data->getWerkstueck(data->getServerCounter()).metall2 = METALL;  data->getWerkstueck(data->getServerCounter()).Hoehe2 = pulse.value.sival_int; break;
//							case FBM1_WK_CODIERT_GRADE: data->getWerkstueck(data->getServerCounter()).typ2 = GRADECODIERT;  data->getWerkstueck(data->getServerCounter()).Hoehe2 = pulse.value.sival_int;  break;
//							case FBM1_WK_CODIERT_UNGRADE: data->getWerkstueck(data->getServerCounter()).typ2 = UNGRADECODIERT;  data->getWerkstueck(data->getServerCounter()).Hoehe2 = pulse.value.sival_int;  break;
//							case FBM1_WK_CODIERT_NULL: data->getWerkstueck(data->getServerCounter()).typ2 = NULLCODIERT;  data->getWerkstueck(data->getServerCounter()).Hoehe2 = pulse.value.sival_int;  break;
//						}
//						data->increaseServerCounter();
		    	   }

		       }

		       if ((_IO_BASE <= pulse.type) && (pulse.type <= _IO_MAX)) {
				   // Some QNX IO msg generated by gns was received
				   if (pulse.type == _IO_CONNECT ) {
						// QNX IO msg _IO_CONNECT was received; answer with EOK
						printf("[Server] received _IO_CONNECT (sync. msg) \n");
						MsgReply( rcvid, EOK, NULL, 0 );
				   }

		       }



		 }
		 return 1;

}


