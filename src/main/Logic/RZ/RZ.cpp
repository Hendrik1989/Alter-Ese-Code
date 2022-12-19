/*
 * RZ.cpp
 *
 *  Created on: 30.11.2022
 *      Author: Jobut
 */

#include "RZ.h"
#include "../SMZ/SMZ.h"


void RZ::entry() {


	printf("---in rz entry");
	//actions->greenOff();
	//actions->yellowOff();
	//actions->redOff();


}

void RZ::exit() {

}


/**
 * Ruhezustand
 */
void RZ::doAction(int event, _pulse msg){

	switch (event) {
		//Starte Betriebszustand
		case STRinterrupted:
			exit();
			new(this) BZ;
			entry();
			break;
		//Starte Servicemode
		case STR_SMZ:
			exit();
			new (this) SMZ;
			entry();
			break;
	}
}


void RZ::estp(){
	exit();
	new(this) ESZ;
	entry();

}
