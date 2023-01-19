
/*
 * ADCService.cpp
 *
 *  Created on: 25.10.22
 *      Author: Alexander Wilke
 *
 */
#include "ADC_Service.h"

TSCADC tsc;
ADC adc(tsc);
ADC adcSingle(tsc);


int counter = 0;
bool isInterrupted = false;

ADC_Service::ADC_Service(Dispatcher *dispatcher) {

	disp = dispatcher;
	dispId  = dispatcher->getConnectionID();
	ADCInterruptServiceThread  = new std::thread([this]() {adcInterruptService();});
}

ADC_Service::~ADC_Service() {


}

void ADC_Service::adcInterruptService() {

	ThreadCtl(_NTO_TCTL_IO, 0);
	/* ### Create channel ### */
			chanID = ChannelCreate(0);//Create channel to receive interrupt pulse messages.
			if (chanID < 0) {
				perror("Could not create a channel!\n");

			}

			conID = ConnectAttach(0, 0, chanID, _NTO_SIDE_CHANNEL, 0); //Connect to channel.
			if (conID < 0) {
				perror("Could not connect to channel!");
			}

			// schicke pulse mit sampling don wenn sampling fertig ist an adc_service selber
			adc.registerAdcISR(conID,ADC_SAMLING_FINISHED);



			events = {ADC_START_SAMPLE, ADC_SAMLING_FINISHED, START_SMZ, LSR1interrupted, LSR2interrupted};


			disp->registerForEventWIthConnection(events, conID);

			_pulse pulse;

				 while (true) {

					 adc.sample();
					 int recvid = MsgReceivePulse(chanID, &pulse, sizeof(_pulse), nullptr);

					if (recvid < 0) {
						perror("MsgReceivePulse failed! - ADC_Service");
						exit(EXIT_FAILURE);
					}
					// send adc value to dispathcer
					//switch(pulse.code){
					//case ADC_SAMLING_FINISHED:

						//Wenn noch keine Distanzmessung hinterlegt wurde für FB_ADC Messung
//					 			if(&wsa_data->distance_FB_ADC == 0){
//					 			MsgSendPulse(dispId, -1, ADC_WK_IN_HM, pulse.value.sival_int);
//					 			}
					//sleep(2);
					aktuelleHoehe = pulse.value.sival_int;
					if(pulse.code == START_SMZ){

						SMZ_checkHoehe = true;
						ws_type = pulse.value.sival_int;
						//cout << "SMZ auf true -- WS_type " << ws_type << endl;
					}

					if(!SMZ_checkHoehe){
						//WS in Höhenmessung
					 //if(aktuelleHoehe < MIN_HOEHE && aktuelleHoehe > MAX_HOEHE &&!isInterrupted){

					if(aktuelleHoehe < MIN_HOEHE && aktuelleHoehe > MAX_HOEHE &&!isInterrupted){

					isInterrupted = true;

					printf("werkstueck in hoehenmessung %d \n",counter);
					//
					MsgSendPulse(dispId, -1, ADC_WK_IN_HM, aktuelleHoehe);
					}
					//WS raus aus Höhenmessung
					else if(aktuelleHoehe > MIN_HOEHE && isInterrupted){
						isInterrupted = false;
						printf("Es wurden %d Messungn beim Höhenmesser gemacht \n",counter);
						// berechne durchschnnit
						MsgSendPulse(dispId, -1, ADC_WK_NIN_HM, aktuelleHoehe);
						//MsgSendPulse(dispId, -1, ADC_SAMLING__VALUE_FINISHED, aktuelleHöhe);
						//printSamples();
						//TODO Hier WK classify starten

						int wkType = classifyWK();
						printf("WK TYPE : %d",wkType);


						counter = 0;
						samples.clear();
						} else if (isInterrupted){
							samples.push_back(pulse.value.sival_int);
							counter++;
						}
					} else {

						if(ws_type== 0){
							smz(pulse);
						} else {
							chooseWS();
						}
					}
				 }
}

void ADC_Service::smz(_pulse pulse){

	if(samples.size()  <= 1000){
	samples.push_back(pulse.value.sival_int);
	} else {
		samples.erase(samples.begin());
		int summe=0;
		int max=10000;
		for(int i: samples){
			summe += i;
			if(i<max){
				max =i;
			}
		}
		int tmp = summe / samples.size();
		cout <<"Es wurden in " << samples.size() << "Messungen der Durschnitt:"<<  tmp <<"\n" << endl;
		cout <<"Höhste gemessene Grundhöhe:"<<  max <<"\n" << endl;
		h_grund = tmp;
		if(FESTO_TYPE == 1){
			MsgSendPulse(dispId, -1, LSR1interrupted, 0);
		} else if (FESTO_TYPE == 2){
			MsgSendPulse(dispId, -1, LSR2interrupted, 0);
		}
		// hiernach soll max und min höhe gesetzt werden
		//MsgSendPulse();
		samples.clear();
		SMZ_checkHoehe = false;
	}

}




int ADC_Service::classifyWK() {


	// remove noise from start and beginning
	for(int i = 0; i < 10; i++) {

		samples.erase(samples.begin());
		samples.pop_back();
	}

	int max = samples.front();
	int min = samples.front();
	int letzterWert = samples.front();
	int diff = 0;
	int maxDiff = 0;
	

	for(int s: samples){

		if(letzterWert > s) {
			diff = letzterWert - s;
		} else {
			diff = s - letzterWert;
		}

		if (s > max) {
			max = s;
		}

		if (s < min) {
			min = s;
		}


		if(diff > maxDiff) {
			maxDiff = diff;
		}

	}

	// hier damit WK klassifizieren


	printf("MAX: %d \n",max);
	printf("MIN: %d \n",min);


	if(max > h_flach -toleranz && min < h_flach + toleranz) {
			MsgSendPulse(dispId, -1, WK_FLACH, 0);
			printf("FLACHES WK ENTDECKT  \n");
			return WK_FLACH;
	} else if(max > h_bohrung -toleranz && min < h_bohrung +toleranz ) {
			MsgSendPulse(dispId, -1, WK_Bohrung_Normal, 0);
			printf("BOHRUNG WK ENTDECKT  \n");
			return WK_Bohrung_Normal;
	}	else if(max < h_normal-toleranz && max < h_normal+toleranz){
			MsgSendPulse(dispId, -1, WK_Normal, 0);
			printf("NORMAL WK ENTDECKT  \n");
			return WK_Normal;
	}
//
//	if(max > h_normal -50 ) {
//
//		if(min < h_normal +50 ) {
//			MsgSendPulse(dispId, -1, WK_Bohrung_Normal, 0);
//			printf("BOHRUNG WK ENTDECKT  \n");
//			return WK_Bohrung_Normal;
//		}
//		MsgSendPulse(dispId, -1, WK_FLACH, 0);
//		printf("FLACHES WK ENTDECKT  \n");
//		return WK_FLACH;
//	}
//
//	else if((max < 2450 && max < 2800) && maxDiff < 60) {
//		MsgSendPulse(dispId, -1, WK_Normal, 0);
//		printf("NORMAL WK ENTDECKT  \n");
//		return WK_Normal;
//	}
	MsgSendPulse(dispId, -1, WK_UNDEFINED, 0);
	printf("UNDEFINED WK ENTDECKT  \n");
	return WK_UNDEFINED;

}

void ADC_Service::printSamples(){
	int i=1;
	for(int s: samples){
		cout << i << ". höhe:"<< s << endl;
		//printf("%d. höhe: %d \n ",i,s);
		i++;
	}
}

void ADC_Service::chooseWS(){

switch(ws_type){
	case 1:
		h_flach = setWS_hoehe();
		cout << "Flach: "<< h_flach<< "\n" << endl;
		break;

	case 2:
		h_normal = setWS_hoehe();
		cout <<"Normal: "<< h_normal<< "\n" << endl;
		break;

	case 3:
		h_bohrung = setWS_hoehe();
		cout << "Bohrung: "<< h_bohrung<< "\n" << endl;
		break;

	case 4:
		h_metall = setWS_hoehe();
		cout << "Metall: "<< h_metall<< "\n" << endl;
		break;


	}
}

int ADC_Service::getGemssenehoehe(int type){
	switch(type){
		case 1:
			cout <<"Flach: "<< h_normal<< "\n" << endl;
			return h_flach;
			//break;

		case 2:
			cout <<"Normal: "<< h_normal<< "\n" << endl;
			return h_normal;
			//break;
		case 3:
			cout << "Bohrung: "<< h_bohrung<< "\n" << endl;
			return h_bohrung;
			//break;

		case 4:
			cout << "Metall: "<< h_metall<< "\n" << endl;
			return h_metall;
			//break;
		default:
			cout << "Die Nummer : "<< type<< "kann als höhe im ADC nicht ausgewählt werden\n" << endl;

		}

}
int ADC_Service::setWS_hoehe(){
	_pulse pulse;

	while(true){
		adc.sample();

		int recvid = MsgReceivePulse(chanID, &pulse, sizeof(_pulse), nullptr);
		if(pulse.code == LSR1interrupted || pulse.code == LSR2interrupted){
			int summe=0;
			for(int s: samples){

				summe += s;
			}
			summe = summe / samples.size();
			cout << "setWS_hoehe: " << summe << endl;
			//cout <<"Es wurden in " << summe <<"\n" << endl;

			SMZ_checkHoehe = false;
			samples.clear();
			return summe;
		}
		if(pulse.value.sival_int < (h_grund -100)){
			samples.push_back(pulse.value.sival_int);
		}
	}

}





