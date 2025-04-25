#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include "./Funciones/FuncionesNT.h"
#include "./Comunicacion/Comunicaciones.h"
#include "Comunicaciones.h"

#include "cosas.h"


void app_main(void)
{
    // Inicializar los pines
    Init_pin_funcion();
    //saca por el terminal serial el id del chip
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
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
    Disable_wifi();

    // Entrar en modo de sueño profundo
    Deep_sleep(5000);
}