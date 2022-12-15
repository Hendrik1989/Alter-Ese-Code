

/*
 * Context.cpp
 *
 *  Created on: 30.11.2022
 *      Author: Jobut, Marquahe
 */

#include "Context.h"
#include "ContextData.h"
#include "Basestate.h"

#include "RZ/RZ.h"
#include "BZ/BZ.h"
#include "ESZ/ESz.h"
#include "SMZ/SMZ.h"
#include "FZ/FZ.h"

#include <stdio.h>
#include <iostream>


Context::Context(Dispatcher *dispatcher, Actions *actions, ContextData  *contextData) {


	printf("bin in context");
	state = new RZ(); // Setze state auf ruhezustand
	disp = dispatcher;
	dispID = disp->getConnectionID();
	//state->setDispId(disp->getConnectionID());
	state->setContextData(contextData);
	state->setActions(actions);

	ContextThread = new std::thread([this]() {eventHandler();});

}

Context::~Context() {
	delete state;
	delete disp;
}


void Context::eventHandler(){


	// Gehe in die Entry methode die von Ruhezustand implementiert wird

	//state->entry();


//	int chanID = ChannelCreate(0);//Create channel to receive interrupt pulse messages.
//				if (chanID < 0) {
//					perror("Could not create a channel!\n");
//
//				}
//
//				int conID = ConnectAttach(0, 0, chanID, _NTO_SIDE_CHANNEL, 0); //Connect to channel.
//				if (conID < 0) {
//					perror("Could not connect to channel!");
//
//				}

	//actions->greenOn(disp->getConnectionID());

	//printf("bin in eventHandler\n");
//	printf("Context wurde gestartet");


//
//
	/* ### Create channel ### */
		int chanID = ChannelCreate(0);//Create channel to receive interrupt pulse messages.
		if (chanID < 0) {
			perror("Could not create a channel!\n");
		}

		int conID = ConnectAttach(0, 0, chanID, _NTO_SIDE_CHANNEL, 0); //Connect to channel.
		if (conID < 0) {
			perror("Could not connect to channel!");
		}
		//TODO alle sensorsignale einfügen
		events = {LSAinterrupted,LSEinterrupted,STRinterrupted,LSSinterrupted,ADC_WK_IN_HM,ADC_WK_NIN_HM};



		disp->registerForEventWIthConnection(events, conID);

			_pulse msg;

			while(true){

			int recvid = MsgReceivePulse(chanID, &msg, sizeof(_pulse), nullptr);

			   if (recvid < 0) {
			   			perror("MsgReceivePulse failed!");
			   			//exit(EXIT_FAILURE);
			   			//exit();
			   		}


			   switch(msg.code) {

			   case LSAinterrupted:
				   //printf("Context -------- \n");
				   //if (msg.value.sival_int == 1) { !!!!!!!!!!!!!!
				   //state->doaction(LSAinterrupted);
					   actions->startFB(dispID);
				   //}
				   break;

			   case LSEinterrupted:
				   actions->stopFB(dispID);

				   break;

			   case ADC_WK_IN_HM:
				//  printf("Werkstuekc in hoehenmesser!!! \n");
				   actions->moveSlower(dispID);
				   break;

			   case	ADC_WK_NIN_HM:
				   actions->moveFaster(dispID);
				   break;

			   case	STRinterrupted:
				   actions->greenOn(dispID);
				   break;
			   case	LSSinterrupted:
				   actions->switchOn(dispID);
				   break;

//			   case	6:
//				   actions->greenOff(dispID);
//				   break;
//
//			   case	7:
//				   actions->yellowOn(dispID);
//				   break;
//
//			   case 8:
//				   actions->yellowOff(dispID);
//			   	   break;
//
//			   case 9:
//				   actions->redOn(dispID);
//				   break;
//
//			   case 10:
//				   actions->redOff(dispID);
//				   break;
			   }


		//		printf("SENSORIK HAT INTERRUPT EHRALTEN!\n");
		}
//
//
}

