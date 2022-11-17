
/*
 * DispatcherC.cpp
 *
 *  Created on: 15.11.22
 *      Author: Alexander Wilke
 *
 */
#include "Dispatcher.h"

Dispatcher::Dispatcher() {

		DispThread = new std::thread([this]() {ListenForEvents();});
}

void Dispatcher::DispatchMessageToSubscriber(int8_t code, int value) {

	// Send Value x to all subscribers of the code

}
void Dispatcher::registerForEventWIthConnection(int event, int conId) {


	// register to get events, puit connectionId in the subscribers list for an event code
}



void Dispatcher::ListenForEvents() {

	int channelID = ChannelCreate(0);
	if (channelID < 0) {
		perror("Could not create a channel!\n");

	}
int pid = getpid();
	int conID = ConnectAttach(0, pid, channelID, _NTO_SIDE_CHANNEL, 0); //Connect to channel.
	if (conID < 0) {
		perror("Could not connect to channel!");
	}

	chanID = channelID;
	conIDDispatcher =conID ;

	_pulse pulse;

	// run 4ever
	 while (true) {

		 	 //
		 int recvid = MsgReceivePulse(channelID, &pulse, sizeof(_pulse), nullptr);

		 		if (recvid < 0) {
		 			perror("MsgReceivePulse failed!");
		 			exit(EXIT_FAILURE);
		 		}

		 		else {
		 			DispatchMessageToSubscriber(pulse.code, pulse.value.sival_int);
		 			// Sende Message weiter an alle subscriber
		 		}

		 			// Do not ignore OS pulses!

	 }
}




