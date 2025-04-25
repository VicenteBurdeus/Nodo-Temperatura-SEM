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

    gpio_config_t io_conf_input = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << Pin_bateria) | (1ULL << Pin_senial_sensor),
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf_input);

    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(Pin_enable_divisorR, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
}

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

uint8_t Get_battery_level() {
    static adc_oneshot_unit_handle_t adc_handle = NULL;
    static adc_cali_handle_t cali_handle = NULL;
    static bool initialized = false;

    if (!initialized) {
        // Configurar ADC oneshot
        adc_oneshot_unit_init_cfg_t init_config = {
            .unit_id = ADC_UNIT_1,
            .ulp_mode = ADC_ULP_MODE_DISABLE
        };
        adc_oneshot_new_unit(&init_config, &adc_handle);

        adc_oneshot_chan_cfg_t config = {
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT
        };
        adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_6, &config);

        // Inicializar calibración
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = ADC_UNIT_1,
            .atten = ADC_ATTEN_DB_12,
            .bitwidth = ADC_BITWIDTH_DEFAULT
        };
        adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle);

        initialized = true;
    }

    gpio_set_level(Pin_enable_divisorR, 1); // Activar divisor resistivo
    vTaskDelay(pdMS_TO_TICKS(50)); // Estabilización

    int raw = 0, voltage_mv = 0;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &raw);
    adc_cali_raw_to_voltage(cali_handle, raw, &voltage_mv);

    gpio_set_level(Pin_enable_divisorR, 0); // Desactivar divisor

    // Calcular el porcentaje de batería en pasos de 10 
    // 0% = 1.80 y 100% = 2.6v
    voltage_mv = voltage_mv - 1800; // Restar 1.8V
    voltage_mv = voltage_mv * 20 / 800; // Calcular porcentaje (0-100%)
    return (uint8_t) voltage_mv * 5;

}

sensor_data_t Get_sensor_data(void){
    sensor_data_t sensor_data = {0.0, 0};

    int16_t temperature = 0, humidity = 0;
    if (dht_read_data(DHT_TYPE_DHT11, Pin_senial_sensor, &humidity, &temperature) == ESP_OK) {
        sensor_data.temperature = (float)temperature;
        sensor_data.humidity = humidity;
    }
    else {
        sensor_data.temperature = -1.0; // Error en lectura
        sensor_data.humidity = -1; // Error en lectura
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

