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
 * | 08/10/2025 | Document creation		                         |
 *
 * @author Nicola Josefina (josefina.nicola@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "switch.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
#define MUESTREO_PERIODO_AD 2000 // En microseg
#define cadena_SIZE 231
/*==================[internal data definition]===============================*/
TaskHandle_t conversion_AD_handle; // Etiqueta
TaskHandle_t conversion_DA_handle;

int MUESTREO_PERIODO_DA = 4000;

const char ecg[cadena_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

timer_config_t timer_conversion_DA;

/*==================[internal functions declaration]=========================*/


// Función para convertir la señal de digital a analógica
void ConversionDA(){
	int i = 0;
	AnalogOutputInit();

	while(1){
		TimerStop(TIMER_B);
		timer_conversion_DA.period = MUESTREO_PERIODO_DA;

		TimerInit(&timer_conversion_DA);
		TimerStart(TIMER_B);

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogOutputWrite(ecg[i]);
		i++;

		if(i == sizeof(ecg)){
			i = 0;
		}
	}
}

void TimerDAC(void* param){
    vTaskNotifyGiveFromISR(conversion_DA_handle, pdFALSE);    // Envía una notificación a la tarea asociada al LED_1
}

void ConversionAD(){
	while(1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		uint16_t valor;
		char cadena[8];

		AnalogInputReadSingle(CH1, &valor);
		strcpy(cadena, UartItoa(valor, 10));
		strcat(cadena, "\n\r");				// Condenso todo en una cadena para llamar una vez a la UART
		UartSendString(UART_PC, cadena);  	// Envío el dato convertido a ASCII
	}
}

void TimerADC(void *param){
	vTaskNotifyGiveFromISR(conversion_AD_handle, pdFALSE); // Envía una notificación a la tarea asociada
}

/*==================[external functions definition]==========================*/
void app_main(void){

	// Configuración e inicialización del puerto serie
	serial_config_t mi_serial;
	mi_serial.port = UART_PC;
	mi_serial.baud_rate = 115200; // Bits por segundo
	mi_serial.func_p = NULL;

	// Configuración e inicialización la UART
	UartInit(&mi_serial);

	// Configuración del canal analógico
	analog_input_config_t mi_analogico;
	mi_analogico.input = CH1;
	mi_analogico.mode = ADC_SINGLE;
	mi_analogico.func_p = NULL;
	mi_analogico.param_p = NULL;

	// Inicialización de la lectura analógica
	AnalogInputInit(&mi_analogico);

	// Configuración e inicialización del timer para la conversion AD
	timer_config_t timer_conversion_AD = {
		.timer = TIMER_A,
		.period = MUESTREO_PERIODO_AD,
		.func_p = TimerADC,
		.param_p = NULL
	}; TimerInit(&timer_conversion_AD);

	// Configuración e inicialización del timer para la conversion DA
	timer_conversion_DA.timer = TIMER_B;
	timer_conversion_DA.period = MUESTREO_PERIODO_DA;
	timer_conversion_DA.func_p = TimerDAC;
	timer_conversion_DA.param_p = NULL;
	TimerInit(&timer_conversion_DA);

	// Creación de la tarea para la conversion AD
	xTaskCreate(&ConversionAD, "Conversor AD", 2048, NULL, 5, &conversion_AD_handle);

	// Creación de la tarea para la conversion DA
	xTaskCreate(&ConversionDA, "Conversor DA", 2048, NULL, 5, &conversion_DA_handle);

	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
}
/*==================[end of file]============================================*/