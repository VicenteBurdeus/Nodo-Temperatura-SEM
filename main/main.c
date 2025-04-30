#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include "./Funciones/FuncionesNT.h"
#include "./Comunicacion/Comunicaciones.h"

#include "cosas.h"


void app_main(void)
{
    Init_pin_funcion();

    //crear la tarea del sensor
    xTaskCreatePinnedToCore(
        Sensor_Task,      // Función de la tarea
        "SensorTask",     // Nombre de la tarea
        2048,             // Tamaño del stack
        NULL,             // Parámetro
        5,                // Prioridad
        NULL,             // Handle (no la necesitamos ahora)
        1                 // Núcleo (0 o 1) — aquí corre en el core 1
    );

    
    // Configuración de Wi-Fi
    Wifi_config_t wifi_config = {
        .ssid =  nombrewifi,
        .password = contrasena
    };
    Enable_wifi(&wifi_config);

    // Configuración de MQTT
    mqtt_config_t mqtt_config = {
        .broker = "mqtt_broker",
        .client_id = "client_id",
        .username = "username",
        .password = "password",
        .topic = "PR2/PA9/Keso/sensor",
        .topic2 = "PR2/PA9/Keso/batteria",
    };
    //mqtt_connect(&mqtt_config);

    // Publicar un mensaje
    //mqtt_publish(&mqtt_config, mqtt_config.topic, mqtt_create_json(Get_sensor_data(Pin_senial_sensor)), 1);

    //int battery_level = Get_battery_level();
    //mqtt_publish(&mqtt_config, mqtt_config.topic, mqtt_create_json(Get_sensor_data(Pin_senial_sensor)), 1);
    // Desconectar MQTT y Wi-Fi
    //mqtt_disconnect();
    Show_status_led(0); // LED rojo encendido
    vTaskDelay(pdMS_TO_TICKS(1000));
    Show_status_led(3001); // LED blanco encendido
    vTaskDelay(pdMS_TO_TICKS(1000));
    Disable_wifi();

    // Entrar en modo de sueño profundo
    Deep_sleep(5000);
}