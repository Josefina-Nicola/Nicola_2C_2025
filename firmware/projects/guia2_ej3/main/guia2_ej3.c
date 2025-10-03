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
 * | 03/10/2025 | Document creation		                         |
 *
 * @author Josefina Nicola (josefina.nicola@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hc_sr04.h"
#include "led.h"
#include "gpio_mcu.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lcditse0803.h"
#include "switch.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/
#define MEDICION_PERIODO_US 1000000
/*==================[internal data definition]===============================*/
uint16_t distancia;
bool control_on = true;
bool control_hold = false;

TaskHandle_t medir_task_handle = NULL;
TaskHandle_t mostrar_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

void FuncTimer(void* param){
    vTaskNotifyGiveFromISR(medir_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_1 */
	vTaskNotifyGiveFromISR(mostrar_task_handle, pdFALSE);
}

void MedirDistancia(void *pvParameter){
	while(1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if(control_on){
			distancia = HcSr04ReadDistanceInCentimeters();

			if(distancia < 10){
				// printf("Distancia menor a 10 cm\n");
				// printf("LED_1 OFF\n");
				LedOff(LED_1);
				// printf("LED_2 OFF\n");
				LedOff(LED_2);
				// printf("LED_3 OFF\n");
				LedOff(LED_3);
			}

			else if((10 <= distancia) && (distancia < 20)){
				// printf("Distancia mayor a 10 cm y menor a 20 cm\n");
				// printf("LED_1 ON\n");
				LedOn(LED_1);
				// printf("LED_2 OFF\n");
				LedOff(LED_2);
				// printf("LED_3 OFF\n");
				LedOff(LED_3);
			}

			else if((20 <= distancia) && (distancia < 30)){
				// printf("Distancia mayor a 20 cm y menor a 30 cm\n");
				// printf("LED_1 ON\n");
				LedOn(LED_1);
				// printf("LED_2 ON\n");
				LedOn(LED_2);
				// printf("LED_3 OFF\n");
				LedOff(LED_3);
			}

			else{
				// printf("Distancia mayor a 30 cm\n");
				// printf("LED_1 ON\n");
				LedOn(LED_1);
				// printf("LED_2 ON\n");
				LedOn(LED_2);
				// printf("LED_3 ON\n");
				LedOn(LED_3);
			}
		}
        
		else{
				// printf("Medicion apagada\n");
				// printf("LED_1 OFF\n");
				LedOff(LED_1);
				// printf("LED_2 OFF\n");
				LedOff(LED_2);
				// printf("LED_3 OFF\n");
				LedOff(LED_3);
		}
	}
}

void MostrarDistancia(void *pvParameter){
	while (1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
		if(control_on && !control_hold){
			LcdItsE0803Init();
			LcdItsE0803Write(distancia);

			char linea[12];  // Suficiente para enviar "ddd cm\r\n" + null
  			sprintf(linea, "%03d cm\r\n", distancia);
  			UartSendString(UART_PC, linea);
		}
		
		else if(!control_on){
			LcdItsE0803Off();
		}
	}
}

void CambiarSwitch1(){
	control_on = !control_on;
}

void CambiarSwitch2(){
	control_hold = !control_hold;
}

void EntradaPorTeclado(void *param){
	uint8_t tecla_ingresada;
	
	UartReadByte(UART_PC, &tecla_ingresada);	// Lee la tecla ingresada y cambia la variable
	UartSendByte(UART_PC, (char*) &tecla_ingresada);

	if(tecla_ingresada == 'O'){
		CambiarSwitch1();
	}
	else if(tecla_ingresada == 'H'){
		CambiarSwitch2();
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){    
    
    LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();

	// Inicialización y configuración del puerto serie
	serial_config_t mi_serial;
	mi_serial.port = UART_PC;
	mi_serial.baud_rate = 115200; 
	mi_serial.func_p = EntradaPorTeclado;

	// Inicialización y configuración de la UART
	UartInit(&mi_serial);

	timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = MEDICION_PERIODO_US,
        .func_p = FuncTimer,
        .param_p = NULL
    };
	TimerInit(&timer_medicion);

	xTaskCreate(MedirDistancia, "Medir distancia", 2048, NULL, 5, &medir_task_handle);
	xTaskCreate(MostrarDistancia, "Mostrar distancia", 4096, NULL, 5, &mostrar_task_handle);

	TimerStart(timer_medicion.timer);

	SwitchActivInt(SWITCH_1, CambiarSwitch1, NULL);
	SwitchActivInt(SWITCH_2, CambiarSwitch2, NULL);
}
/*==================[end of file]============================================*/