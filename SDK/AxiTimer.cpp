/*
 * AxiTimer.cpp
 *
 *  Created on: Jul 3, 2018
 *      Author: Weiwen Jiang
 */

#include "AxiTimer.h"
#include <stdio.h>

AxiTimer::AxiTimer() {
	// Initialize the timer hardware...
	 XTmrCtr_Initialize(&m_AxiTimer, XPAR_TMRCTR_0_DEVICE_ID);

	 //XTmrCtr_SetOptions(&m_AxiTimer, 0, XTC_AUTO_RELOAD_OPTION);

	 // Get the clock period in seconds
	 m_timerClockFreq = (double) XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
	 m_clockPeriodSeconds = (double)1/m_timerClockFreq;

}

AxiTimer::~AxiTimer() {
	// TODO Auto-generated destructor stub
}

unsigned long AxiTimer::getElapsedTicks() {
	return m_tickCounter2 - m_tickCounter1;
}

unsigned long AxiTimer::startTimer() {
	// Start timer 0 (There are two, but depends how you configured in vivado)
	XTmrCtr_Reset(&m_AxiTimer,0);
	m_tickCounter1 =  XTmrCtr_GetValue(&m_AxiTimer, 0);
	printf("Start: %ld\n",m_tickCounter1);
	XTmrCtr_Start(&m_AxiTimer, 0);
	return m_tickCounter1;
}

unsigned long AxiTimer::stopTimer() {
	XTmrCtr_Stop(&m_AxiTimer, 0);
	m_tickCounter2 =  XTmrCtr_GetValue(&m_AxiTimer, 0);
	printf("End: %ld\n",m_tickCounter2);
	return m_tickCounter2 - m_tickCounter1;
}

double AxiTimer::getElapsedTimerInSeconds() {
	double elapsedTimeInSeconds = (double)(m_tickCounter2 - m_tickCounter1) * m_clockPeriodSeconds;
	return elapsedTimeInSeconds;
}

double AxiTimer::getClockPeriod() {
	return m_clockPeriodSeconds;
}

double AxiTimer::getTimerClockFreq() {
	return m_timerClockFreq;
}
