/*
 * gpio.h
 *
 *  Created on: 06/03/2013
 *      Author: eric
 */

#ifndef GPIO_H_
#define GPIO_H_

	void setup_io();
	void SET_INPUT(int Gbit);
	void SET_OUTPUT(int Gbit);
	void SET_BIT(int Gbit);
	void CLR_BIT(int Gbit);
#endif /* GPIO_H_ */
