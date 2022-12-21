/*
 * AnstehendQuittiert.h
 *
 *  Created on: 21.12.2022
 *      Author: Hendrik Marquardt
 */

#ifndef SRC_MAIN_LOGIC_FZ_ANSTEHENDQUITTIERT_H_
#define SRC_MAIN_LOGIC_FZ_ANSTEHENDQUITTIERT_H_

#include "../Basestate.h"
#include "../../hal/Actuator.h"
#include "../Context.h"
#include "../../WSAData.h"

/**
 * Erbt von Basic_State
 */
class AnstehendQuittiert : public Basestate{
private:
	int myChannel;
	int myConnection;
	Dispatcher *disp;

public:
	void entry()override;
	void exit() override;
	void doAction(int, _pulse) override;
};

#endif /* SRC_MAIN_LOGIC_FZ_ANSTEHENDQUITTIERT_H_ */
