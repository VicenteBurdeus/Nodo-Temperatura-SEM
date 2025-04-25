#include "FuncionesNT.h"

 // Asegúrate de tener una lib DHT compatible con ESP-IDF

void Init_pin_funcion(void){//Revisar 
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << Pin_Led_rojo) | (1ULL << Pin_Led_blanco) | (1ULL << Pin_enable_divisorR),
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    // Configurar pin del sensor como entrada si es necesario
    gpio_set_direction(Pin_senial_sensor, GPIO_MODE_INPUT);
    gpio_set_direction(Pin_bateria, GPIO_MODE_INPUT); // para ADC también
}

uint8_t Get_battery_level(){
    gpio_set_level(Pin_enable_divisorR, 1); // activar divisor
    vTaskDelay(pdMS_TO_TICKS(50)); // esperar estabilización

    // Leer ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11); // GPIO34 = ADC1_CH6
    int raw = adc1_get_raw(ADC1_CHANNEL_6);

    gpio_set_level(Pin_enable_divisorR, 0); // desactivar divisor

    // Convertir lectura a porcentaje (suponiendo linealidad entre 2.0V y 3.3V)
    float voltage = ((float)raw / 4095.0) * 3.3;
    float percent = (voltage - 2.0) / (3.3 - 2.0) * 100.0;
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    return (uint8_t)percent;
}

sensor_data_t Get_sensor_data(int8_t pin){
    sensor_data_t sensor_data = {0.0, 0};

    int16_t temperature = 0, humidity = 0;
    if (dht_read_data(DHT_TYPE_DHT11, pin, &humidity, &temperature) == ESP_OK) {
        sensor_data.temperature = (float)temperature;
        sensor_data.humidity = humidity;
    }

    return sensor_data;
}

void Show_status_led(uint16_t status){
//intentar programarlo para que no se quede bloaquedado en hacer ciclos con las leds
 // Funcionamiento correcto (status == 0)
 if (status == 0) {
    gpio_set_level(Pin_Led_blanco, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(Pin_Led_blanco, 0);
    return;
}

// Identificar tipo de error por los miles
uint8_t parpadeos = 0;
if (status >= 1000 && status < 2000) parpadeos = 1;
else if (status >= 2000 && status < 3000) parpadeos = 2;
else if (status >= 3000 && status < 4000) parpadeos = 3;

for (uint8_t i = 0; i < parpadeos; i++) {
    gpio_set_level(Pin_Led_rojo, 1);
    vTaskDelay(pdMS_TO_TICKS(300)); // Encendido 300 ms
    gpio_set_level(Pin_Led_rojo, 0);
    vTaskDelay(pdMS_TO_TICKS(300)); // Apagado 300 ms
}

}

void Deep_sleep(uint32_t time_ms){
    esp_sleep_enable_timer_wakeup((uint64_t)time_ms * 1000); // en microsegundos
    esp_deep_sleep_start();
}

