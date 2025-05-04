#include "Comunicaciones.h"


#define WIFI_CONNECTED_BIT BIT0
static EventGroupHandle_t wifi_event_group;
static const char *TAG = "COM";

static esp_mqtt_client_handle_t client = NULL;

// Eventos Wi-Fi
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        esp_wifi_connect();
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        esp_wifi_connect(); // reconecta automáticamente
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
}

// Inicializa y conecta a Wi-Fi
error_code_t Enable_wifi(Wifi_config_t *wifi_config)
{
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_cfg = { 0 };
    strncpy((char*)wifi_cfg.sta.ssid, wifi_config->ssid, sizeof(wifi_cfg.sta.ssid));
    strncpy((char*)wifi_cfg.sta.password, wifi_config->password, sizeof(wifi_cfg.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Conectando a Wi-Fi...");
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Conectado a Wi-Fi.");
    return NoError;
}

void Disable_wifi(void)
{
    esp_wifi_stop();
    esp_wifi_deinit();
    ESP_LOGI(TAG, "Wi-Fi deshabilitado.");
}

// MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT conectado.");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT desconectado.");
            break;
        default:
            break;
    }
}

error_code_t mqtt_connect(mqtt_config_t *mqtt_config)
{
    esp_mqtt_client_config_t config = {
        .broker.address.uri = mqtt_config->broker,
        .credentials.client_id = mqtt_config->client_id,
        .credentials.username = mqtt_config->username,
        .credentials.authentication.password = mqtt_config->password,
    };

    client = esp_mqtt_client_init(&config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    return NoError;
}

void mqtt_disconnect(void)
{
    if (client) {
        esp_mqtt_client_stop(client);
        esp_mqtt_client_destroy(client);
        client = NULL;
    }
}

error_code_t mqtt_publish(mqtt_config_t *mqtt_config, const char *topic, const char *message, int qos)
{
    if (client) {
        esp_mqtt_client_publish(client, topic, message, 0, qos, 0);
    }
    return NoError;
}

// Crear JSON
error_code_t mqtt_create_json(int8_t temperature, uint8_t humidity, uint8_t battery_level, char **json_string)
{
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        return SensorError; // Error: no se pudo crear el objeto JSON
    }
    cJSON_AddNumberToObject(root, "temperatura", temperature);
    cJSON_AddNumberToObject(root, "humedad", humidity);
    cJSON_AddNumberToObject(root, "battery", battery_level);
    cJSON_AddStringToObject(root, "ID", "NT_1");

    *json_string = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    return NoError;
}