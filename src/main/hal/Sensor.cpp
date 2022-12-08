
/*
 * Sensor.cpp
 *
 *  Created on: 25.10.22
 *      Author: Alexander Wilke
 *
 */
#include "Sensor.h"

#include <iostream>
#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <sys/neutrino.h>

#include <thread>
#include <chrono>


Sensor::Sensor(Dispatcher * dispatcher) {
	disp = dispatcher;
	dispID  = dispatcher->getConnectionID();
	SensorRoutineThread = new std::thread([this]() {sensorRoutine();});


}

Sensor::~Sensor() {

}

void Sensor::sensorRoutine() {


	// Hier signale weiterleiten

	//channel erstellen
	/* ### Create channel ### */
				int chanID = ChannelCreate(0);//Create channel to receive interrupt pulse messages.
				if (chanID < 0) {
					perror("Could not create a channel!\n");
				}


				int conID = ConnectAttach(0, 0, chanID, _NTO_SIDE_CHANNEL, 0); //Connect to channel.
				if (conID < 0) {
					perror("Could not connect to channel!");
				}

				//printf("Sensorik conID: %d \n", conID);
				senorEvents={LSA1, LSE1, LSS1, HMS1};

				disp->registerForEventWIthConnection(senorEvents, conID);

				_pulse pulse;

					// run 4ever
				while (true) {

					int recvid = MsgReceivePulse(chanID, &pulse, sizeof(_pulse), nullptr);

					if (recvid < 0) {
							perror("MsgReceivePulse failed!");
							exit(EXIT_FAILURE);
					}
					//printf("Sensorik vor dem Switch Case");
						// Untersuche und Sende event an Dispatcher
						 switch(pulse.code) {

						   case LSA1:
							   //printf("Sensorik hat Lichtschranke 1 - signal erhalten \n ");
							   if (pulse.value.sival_int == 1) {
								   MsgSendPulse(dispID, -1, LSAnotInterrupted, 0);
								   break;
							   } else {
								   MsgSendPulse(dispID, -1, LSAinterrupted, 0);
							   } break;
						   case LSE1:
							   if (pulse.value.sival_int == 1) {
								   MsgSendPulse(dispID, -1, LSEnotInterrupted, 0);
								   break;
							   } else {
								   MsgSendPulse(dispID, -1, LSEinterrupted, 0);
							   } break;
						   case LSS1:
							   if (pulse.value.sival_int == 1) {
								   MsgSendPulse(dispID, -1, LSSnotInterrupted, 0);
								   break;
							   } else {
								   MsgSendPulse(dispID, -1, LSSinterrupted, 0);
							   } break;
						   case HMS1:
								   MsgSendPulse(dispID, -1, HMSinterrupted, 0);
							   break;


						   case SRT:
							   printf("Sensro STR Taste -------- \n");
								   MsgSendPulse(dispID, -1, STRinterrupted, 0);
							   break;


							case STP:
								   MsgSendPulse(dispID, -1, STPinterrupted, 0);
							   break;


							case ESTP:
								   MsgSendPulse(dispID, -1,ESTPinterrupted, 0);
							   break;

							   case RST:
								   MsgSendPulse(dispID, -1, RSTinterrupted, 0);
							   break;

							   }


					 }
}
