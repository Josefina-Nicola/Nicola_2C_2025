/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 20/08/2025 | Document creation		                         |
 *
 * @author Josefina Nicola (josefina.nicola@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
/*==================[macros and definitions]=================================*/
struct leds{
		uint8_t mode;        /*ON, OFF, TOGGLE*/
		uint8_t n_led;       /*indica el número de led a controlar*/
		uint8_t n_ciclos;    /*indica la cantidad de ciclos de encendido/apagado*/
		uint16_t periodo;    /*indica el tiempo de cada ciclo*/
};

enum {
  on,
  off,
  toggle
};
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void ProcesarLeds(struct leds *my_leds){
	switch(my_leds->mode){
		case 0:
			LedOn(my_leds->n_led);
		break;
		case 1:
			LedOff(my_leds->n_led);
		break;
		case 2:
			LedToggle(my_leds->n_led);
		break;
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){
	
	struct leds my_leds;

	LedsInit();
	my_leds.mode = toggle;
	my_leds.n_led = LED_3;
	my_leds.periodo = 500;
	my_leds.n_ciclos = 10;
	
	int i=0;

	struct leds *my_leds_ptr = &my_leds;

	while(i<(my_leds_ptr->n_ciclos)){
		ProcesarLeds(my_leds_ptr);
		vTaskDelay(my_leds_ptr->periodo / portTICK_PERIOD_MS);
	}
}
/*==================[end of file]============================================*/