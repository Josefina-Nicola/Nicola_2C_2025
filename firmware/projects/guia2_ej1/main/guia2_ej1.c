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
 * | 10/09/2025 | Document creation		                         |
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
/*==================[macros and definitions]=================================*/
/** @def TECLA_PERIODO
 * @brief Valor del período entre el control del estado del switch
 */
#define TECLA_PERIODO 100

/** @def MEDICION_PERIODO
 * @brief Valor del período entre las mediciones
 */
#define MEDICION_PERIODO 1000
/*==================[internal data definition]===============================*/
uint16_t distancia;
bool control_on = false;
bool control_hold = false;

// TaskHandle_t leer_teclas_task_handle = NULL;
// TaskHandle_t medir_task_handle = NULL;
// TaskHandle_t mostrar_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

/** @fn void LeerTeclas(void *pvParameter)
 * @brief Analiza el estado de los switchs cada cierto período de tiempo
 * @return
 */
void LeerTeclas(void *pvParameter){
	int teclas;
	while (1){
		teclas = SwitchesRead();

		switch(teclas){
            case SWITCH_1:
                // LedOn(LED_1);
				control_on = !control_on;
            break;

            case SWITCH_2:
                // LedOff(LED_1);
				control_hold = !control_hold;
            break;
		}
		vTaskDelay(TECLA_PERIODO / portTICK_PERIOD_MS);
	}
}

/** @fn void MedirDistancia(void *pvParameter)
 * @brief Mide la distancia que detecta el sensor y activa o desactiva los LEDs correspondientes
 * @return
 */
void MedirDistancia(void *pvParameter){
	while(1){
		if(control_on){
			distancia = HcSr04ReadDistanceInCentimeters();

			if(distancia < 10){
				printf("Distancia menor a 10 cm\n");
				printf("LED_1 OFF\n");
				LedOff(LED_1);
				printf("LED_2 OFF\n");
				LedOff(LED_2);
				printf("LED_3 OFF\n");
				LedOff(LED_3);
			}

			else if((10 <= distancia) && distancia < 20){
				printf("Distancia mayor a 10 cm y menor a 20 cm\n");
				printf("LED_1 ON\n");
				LedOn(LED_1);
				printf("LED_2 OFF\n");
				LedOff(LED_2);
				printf("LED_3 OFF\n");
				LedOff(LED_3);
			}

			else if(20 <= distancia && distancia < 30){
				printf("Distancia mayor a 20 cm y menor a 30 cm\n");
				printf("LED_1 ON\n");
				LedOn(LED_1);
				printf("LED_2 ON\n");
				LedOn(LED_2);
				printf("LED_3 OFF\n");
				LedOff(LED_3);
			}

			else{
				printf("Distancia mayor a 30 cm\n");
				printf("LED_1 ON\n");
				LedOn(LED_1);
				printf("LED_2 ON\n");
				LedOn(LED_2);
				printf("LED_3 ON\n");
				LedOn(LED_3);
			}
		}
        
		else{
				printf("Medicion apagada\n");
				printf("LED_1 OFF\n");
				LedOff(LED_1);
				printf("LED_2 OFF\n");
				LedOff(LED_2);
				printf("LED_3 OFF\n");
				LedOff(LED_3);
		}

		vTaskDelay(MEDICION_PERIODO / portTICK_PERIOD_MS);
	}
}

/** @fn void MostrarDistancia(void *pvParameter)
 * @brief Muestra el dato de distancia en la pantalla LCD
 * @return
 */
void MostrarDistancia(void *pvParameter){
	
    while (1){
		if(control_on && !control_hold){
			LcdItsE0803Init();
			LcdItsE0803Write(distancia);
		}
		else if(!control_on){
			LcdItsE0803Off();
		}

		vTaskDelay(MEDICION_PERIODO / portTICK_PERIOD_MS);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){    
    
    LedsInit();
	SwitchesInit();
	HcSr04Init(GPIO_3, GPIO_2);
	LcdItsE0803Init();

	xTaskCreate(MedirDistancia, "Medir distancia", 2048, NULL, 5, NULL);
	xTaskCreate(MostrarDistancia, "Mostrar distancia", 512, NULL, 5, NULL);
	xTaskCreate(LeerTeclas, "Leer las teclas", 512, NULL, 5, NULL);
}
/*==================[end of file]============================================*/