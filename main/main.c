#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include "./Funciones/FuncionesNT.h"
#include "./Comunicacion/Comunicaciones.h"

#include "cosas.h"


void app_main(void)
{   

    // Configuración de Wi-Fi
    Wifi_config_t wifi_config = {
        .ssid = "SSID",
        .password = "PASSWORD"
    };
    // Configuración de MQTT
    mqtt_config_t mqtt_config = {
        .broker = "100.93.177.37:1883",
        .client_id = "NT_1",
        .username = "user",
        .password = "password",
        .topic = "NT"
    };

    uint8_t bateria = 0;
    int8_t temperatura = 0;
    uint8_t humedad = 0;
    error_code_t status = NoError;
    char *json_string = NULL;

    if(!status){status = Init_pin_funcion();}
    if(!status){status = Enable_wifi(&wifi_config);}
    if(!status){status = mqtt_connect(&mqtt_config);}
    if(!status){status = get_data(&temperatura, &humedad, &bateria);}
    if(!status){status = mqtt_create_json(temperatura, humedad, bateria, &json_string);}
    if(!status){status = mqtt_publish(&mqtt_config, mqtt_config.topic, json_string, 0);}
    if(!status){
        Disable_wifi();
        mqtt_disconnect();}
    status = Show_status_led(status);
    if(!status){status = Deep_sleep(5000);}
}