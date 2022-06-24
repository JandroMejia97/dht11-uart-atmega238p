/*
 * tp3_entregable.c
 *
 * Created: 23/06/2022 01:40:58 p. m.
 * Author : Alejandro Mejía
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "dht.c"
#include "UART.h"
#include "menu.h"
#include "Timer1.h"
#include "main.h"

int main(void)
{
	UART_init(0x33);
	menu_show();	//Imprimir mensaje de bienvenida y menú
	timer_init();
    while (1) 
    {
		if (get_set_apreto_enter()){
			menu_update();
			set_set_apreto_enter(0);
		}
		if (get_hay_para_transmitir()){
			UART_TX_Interrupt_Enable();
			set_hay_para_transmitir(0);
		}
    }
}

