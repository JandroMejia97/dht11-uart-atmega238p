/*
 * tp3_entregable.c
 *
 * Created: 23/06/2022 01:40:58 p. m.
 * Author : Alejandro Mejía
 */ 
#define F_CPU 16000000UL
#define DHT_PIN 1
#include "main.h"

int main(void)
{
	UART_init(0x33);
	menu_show();	//Imprimir mensaje de bienvenida y menú
	dht_init();	//Inicializar sensor DHT11
	//timer_init();
    while (1) 
    {
		if (get_se_apreto_enter()){
			menu_update();
			set_se_apreto_enter(0);
		}
		if (get_hay_para_transmitir()){
			UART_TX_Interrupt_Enable();
			set_hay_para_transmitir(0);
		}
    }
}

