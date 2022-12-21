/*
 * SMZTesting.cpp
 *
 *  Created on: 14.12.2022
 *      Author: Hendrik Marquardt
 */

#include "SMZTesting.h"

void SMZTesting::entry(){
	//LEDs testen
	actions->ledQ1Off();
	actions->ledQ2Off();
	sleep(2);
	actions->ledQ1On();
	actions->ledQ2On();
}

void SMZTesting::exit(){

}

void SMZTesting::doAction(int event, _pulse msg){

	switch(event){

	case LSAinterrupted:
		actions->greenOn();
		sleep(2);
		actions->greenOff();
		break;

	case LSSinterrupted:
		actions->greenOn();
		sleep(2);
		actions->greenOff();
		break;

	case LSRinterrupted:
		actions->greenOn();
		sleep(2);
		actions->greenOff();
		break;

	case LSEinterrupted:
		actions->greenOn();
		sleep(2);
		actions->greenOff();
		break;

	case STRinterrupted:
		exit();
		new (this)SMZTestingWS;
		entry();
	}

}
