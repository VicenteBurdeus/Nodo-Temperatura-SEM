#include "FuncionesNT.h"

void Init_pin_funcion(void){//Revisar 

    return;
}

uint8_t Get_battery_level(){
    //Para leer la bateria a de montarse un circuito divisor de tension
    //y un interruptor para que no este activo todo el tiempo
    //hay que tener en cuenta que puede tardar un poco en estabilizarse
    return 0; //Retorna el valor de la bateria entre 0 y 100
}

sensor_data_t Get_sensor_data(int8_t pin){
    sensor_data_t sensor_data = {0.0, 0};
    
    
    return sensor_data; // Retorna la estructura con los datos del sensor
}

void Show_status_led(uint8_t status){
//intentar programarlo para que no se quede bloaquedado en hacer ciclos con las leds

return;
}

void Deep_sleep(uint32_t time_ms){
    
    return;
}

