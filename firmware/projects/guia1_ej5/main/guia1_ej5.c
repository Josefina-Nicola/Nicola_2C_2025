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
 * | 03/09/2025 | Document creation		                         |
 *
 * @author Josefina Nicola (josefina.nicola@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"

/*==================[macros and definitions]=================================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
void ManipularEstadoPinCodificacion(uint8_t bcd_number, gpioConf_t *pines)
{
	for(uint8_t i = 0; i < 4; i++){
		GPIOInit(pines[i].pin, pines[i].dir); //Inicializo los pines
	}

	for(uint8_t i = 0; i < 4; i++){
		if((bcd_number >> i) & 1){
			GPIOOn(pines[i].pin);
		}
		else{
			GPIOOff(pines[i].pin);
		}
	}
};

/*==================[external functions definition]==========================*/
void app_main(void){
	
	uint8_t numero_bcd = 7;
	gpioConf_t pin[4];

	// Configuración de los pines
	// Pin
	pin[0].pin = GPIO_20;
	pin[1].pin = GPIO_21;
	pin[2].pin = GPIO_22;
	pin[3].pin = GPIO_23;
	
	// Dir
	pin[0].dir = GPIO_OUTPUT;
	pin[1].dir = GPIO_OUTPUT;
	pin[2].dir = GPIO_OUTPUT;
	pin[3].dir = GPIO_OUTPUT;

	ManipularEstadoPinCodificacion(numero_bcd, pin);
}
/*==================[end of file]============================================*/