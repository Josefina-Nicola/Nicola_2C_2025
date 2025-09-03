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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/


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
/*==================[external functions definition]==========================*/
void app_main(void){
	
	uint32_t  data = 256855454;
	uint8_t digits = 6;
	uint8_t bcd_number[digits];
	uint8_t retorno = 0;

	retorno = convertToBcdArray(data, digits, bcd_number);

	if(retorno == 0){
		printf("El número es:\n");
		for (uint8_t i = 0; i < digits; i++){
			printf("%d", bcd_number[i]);
			printf("\n");
		}
	}
	else{
		printf("Error en la conversión");
	}
}
/*==================[end of file]============================================*/