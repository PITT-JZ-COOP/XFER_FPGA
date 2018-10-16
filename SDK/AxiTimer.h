/*
 * AxiTimer.h
 *
 *  Created on: Jul 3, 2018
 *      Author: Weiwen Jiang
 */

#ifndef SRC_AxiTimer_H_
#define SRC_AxiTimer_H_

#include "xil_types.h"
#include "xtmrctr.h"
#include "xparameters.h"

class AxiTimer {
public:
	AxiTimer();
	virtual ~AxiTimer();
	unsigned long getElapsedTicks();
	double getElapsedTimerInSeconds();
	unsigned long startTimer();
	unsigned long stopTimer();
	double getClockPeriod();
	double getTimerClockFreq();
private:
	XTmrCtr m_AxiTimer;
	unsigned long m_tickCounter1;
	unsigned long m_tickCounter2;
	double m_clockPeriodSeconds;
	double m_timerClockFreq;
};


#endif /* SRC_AxiTimer_H_ */
