#include "Comunicaciones.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_MAX_WAIT_TIME_MS 1000 // Tiempo máximo de espera para la conexión Wi-Fi
static EventGroupHandle_t wifi_event_group;
static const char *TAG = "COM";

static esp_mqtt_client_handle_t client = NULL;

#define WIFI_CONNECTED_BIT BIT0
static EventGroupHandle_t wifi_event_group;
static const char *TAG_WIFI = "WIFI";

// Manejador de eventos Wi-Fi
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG_WIFI, "Wi-Fi desconectado, intentando reconectar...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

error_code_t Enable_wifi(Wifi_config_t *wifi_config)
{
    wifi_event_group = xEventGroupCreate();
    if (!wifi_event_group) return WiFiError;

    if (nvs_flash_init() != ESP_OK) return WiFiError;
    if (esp_netif_init() != ESP_OK) return WiFiError;
    if (esp_event_loop_create_default() != ESP_OK) return WiFiError;

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&cfg) != ESP_OK) return WiFiError;

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    wifi_config_t wifi_sta_config = {0};
    strncpy((char*)wifi_sta_config.sta.ssid, wifi_config->ssid, sizeof(wifi_sta_config.sta.ssid));
    strncpy((char*)wifi_sta_config.sta.password, wifi_config->password, sizeof(wifi_sta_config.sta.password));

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config);
    esp_wifi_start();

    ESP_LOGI(TAG_WIFI, "Esperando conexión Wi-Fi...");

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(5000));

    if (!(bits & WIFI_CONNECTED_BIT)) {
        ESP_LOGE(TAG_WIFI, "No se pudo conectar a Wi-Fi");
        return WiFiError;
    }

    ESP_LOGI(TAG_WIFI, "Conectado a Wi-Fi");
    return NoError;
}

void Disable_wifi(void)
{
    esp_wifi_stop();
    esp_wifi_deinit();
    ESP_LOGI(TAG_WIFI, "Wi-Fi deshabilitado");
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