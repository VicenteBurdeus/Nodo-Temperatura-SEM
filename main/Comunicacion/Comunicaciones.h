#ifndef COMUNICACIONES_H
#define COMUNICACIONES_H

#include <stdint.h>
#include <cJSON.h>


#ifndef SENSOR_TYPE
#define SENSOR_TYPE
typedef struct {
    float temperature;
    int humidity; 
} sensor_data_t;
#endif // SENSOR_TYPE

typedef struct
{
    char *broker; // Broker MQTT
    char *client_id; // ID del cliente MQTT
    char *username; // Nombre de usuario para el broker MQTT
    char *password; // Contraseña para el broker MQTT
    char *topic; // Tema al que se suscribe el cliente MQTT
    char *topic2; // Tema al que se publica el cliente MQTT
} mqtt_config_t;

typedef struct
{
    char *ssid; // Nombre de la red Wi-Fi
    char *password; // Contraseña de la red Wi-Fi
} Wifi_config_t;




void Enable_wifi(Wifi_config_t *wifi_config);

void Disable_wifi(void);

void mqtt_connect(mqtt_config_t *mqtt_config);

void mqtt_disconnect(void);

void mqtt_publish(mqtt_config_t *mqtt_config, const char *topic, const char *message, int qos);

char* mqtt_create_json(sensor_data_t data);





#endif // COMUNICACIONES_H