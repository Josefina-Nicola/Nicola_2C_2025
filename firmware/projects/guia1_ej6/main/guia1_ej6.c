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
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 500 

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

/*==================[internal functions declaration]=========================*/
int8_t convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number){
	
	// Error: puntero inválido
	if(bcd_number == NULL){
        return -1; 
    }
    
	// Calculo el divisor para extraer el dígito más significativo
    // Multiplico digits-1 veces por 10 (elevo digits-1 veces)

	uint32_t divisor = 1;
	for (uint8_t i = 1; i < digits; i++) {
        divisor *= 10;
    }


    for (uint8_t i = 0; i < digits; i++) {
        bcd_number[i] = data / divisor;   // Extraigo dígito más significativo
        data = data % divisor;            // Divido para pasar al siguiente dígito
        divisor /= 10;                    // Paso al siguiente dígito
    }


    // Si después de extraer todos los dígitos queda un número distinto de cero,
    // significa que el número no cabe en la cantidad de dígitos indicada

	// Control para que no supere los dígitos indicados
    if (data != 0) {
        return -2; // Error: número demasiado grande para la cantidad de dígitos
    }

	return 0;
}

void ManipularEstadoPinCodificacion(uint8_t bcd_number, gpioConf_t *pines){
	
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

void ManipularDisplayLCD(uint32_t data, uint8_t digits, gpioConf_t *pines, gpioConf_t *pines_digitos, uint8_t *bcd_number){
	
	for(uint8_t i = 0; i<4;i++){
		GPIOInit(pines_digitos[i].pin, pines_digitos[i].dir); // Inicialización de los pines que controlan los dígitos
	}

	ConvertToBcdArray(data, digits, bcd_number);

	for(uint8_t i = 0; i<3;i++)
	{
		GPIOOff(pines_digitos[i].pin);
		ManipularEstadoPinCodificacion(bcd_number[i], pines);
		GPIOOn(pines_digitos[i].pin);
		GPIOOff(pines_digitos[i].pin);
	}
};

/*==================[external functions definition]==========================*/
void app_main(void){
	
	gpioConf_t pin_codificacion[4];
	gpioConf_t pin_digito[3];

	uint32_t data = 857; 			// Dato a convetir
	uint8_t digits = 3;		 		// Cantidad de bits a agrupar
	uint8_t bcd_number[digits]; 	// Almacenamiento de los numeros en BCD 

	// Configuración de los pines
	// Pin
	pin_codificacion[0].pin = GPIO_20;
	pin_codificacion[1].pin = GPIO_21;
	pin_codificacion[2].pin = GPIO_22;
	pin_codificacion[3].pin = GPIO_23;
	
	// Dir
	pin_codificacion[0].dir = GPIO_OUTPUT;
	pin_codificacion[1].dir = GPIO_OUTPUT;
	pin_codificacion[2].dir = GPIO_OUTPUT;
	pin_codificacion[3].dir = GPIO_OUTPUT;

	// Configuración de los pines para controlar cada dígito
	// Pin
	pin_digito[0].pin = GPIO_19;
	pin_digito[1].pin = GPIO_18;
	pin_digito[2].pin = GPIO_9;
	
	// Dir
	pin_digito[0].dir = GPIO_OUTPUT;
	pin_digito[1].dir = GPIO_OUTPUT;
	pin_digito[2].dir = GPIO_OUTPUT;

	// Manipulación de los pines
	ManipularDisplayLCD(data, digits, pin_codificacion, pin_digito, bcd_number);
}
/*==================[end of file]============================================*/