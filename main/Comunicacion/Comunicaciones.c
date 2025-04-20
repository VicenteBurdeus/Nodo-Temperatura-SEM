#include "./Comunicaciones.h"

void Enable_wifi(Wifi_config_t *wifi_config)
{
    
    return;
}
void Disable_wifi(void)
{
    
    return;
}

void mqtt_connect(mqtt_config_t *mqtt_config)
{
    
    return;
}
void mqtt_disconnect(void)
{
    
    return;
}

void mqtt_publish(mqtt_config_t *mqtt_config, const char *topic, const char *message, int qos)
{
    
    return;
}
char* mqtt_create_json(sensor_data_t data)
{  
    #if 0
    {
    “id_dispositivo”:”NT_3”,
    “Temperatura”:26.4,
    “Humedad”:67
    }
    #endif

    return 0;
}