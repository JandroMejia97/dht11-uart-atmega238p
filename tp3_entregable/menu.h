/*
 * IncFile1.h
 *
 * Created: 23/6/2022 23:48:35
 *  Author: Fran
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

	#include "main.h"
	#include "stdint.h"
	#include "string.h"
	#include <avr/io.h>
	
	void menu_show(void);
	void menu_update(void);
	void set_hay_para_transmitir(uint8_t valor);

#endif /* INCFILE1_H_ */