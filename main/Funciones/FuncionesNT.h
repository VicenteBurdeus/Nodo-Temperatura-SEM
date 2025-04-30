#ifndef FUNCIONESNT_H
#define FUNCIONESNT_H

#include <stdint.h>
#include <esp_mac.h>
#include "FuncionesNT.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "C:\Users\Vicente\esp\v5.4.1\esp-idf\components\esp_driver_gpio\include\driver\gpio.h"
#include "dht11.h"
#include "esp_sleep.h"


#define Pin_Led_rojo 27         // Pin salida para led Rojo
#define Pin_Led_blanco 26       // Pin salida pare led Blanco
#define Pin_senial_sensor 5     // Pin entrada para senial del sensor
#define Pin_bateria 34          // Pin entrada para senial de la bateria
#define Pin_enable_divisorR 25  // Pin salida para activar el divisor de tension de la bateria



void Init_pin_funcion(void);

 //es global para que pueda acceder desde el main.c

//Esta funcion devuelve un valor entre 0 y 100 
//que representa el nivel de carga de la bateria
//Por debajo de 20 se considera bateria baja
uint8_t Get_battery_level();

//Lee el senial del sensor y devuelve la temperatura y humedad
sensor_data_t Get_sensor_data(void);


//version tarea en otro core
void Sensor_Task(void *pvParameters);

//Esta funcion controla los led de estado
void Show_status_led(uint16_t status);




//Esta funcion pone la CPU en Deep Sleep durante un tiempo determinado
void Deep_sleep(uint32_t time_ms);

#endif