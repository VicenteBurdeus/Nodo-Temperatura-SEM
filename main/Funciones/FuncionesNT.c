#include "FuncionesNT.h"

#include <esp_mac.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "driver/dac.h"

error_code_t Init_pin_funcion(){//Revisar 


    dac_output_disable(DAC_CHANNEL_2); // Deshabilitar DAC para evitar interferencias
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

    DHT11_init(Pin_senial_sensor); // Inicializar el sensor DHT11

    return NoError;
}

error_code_t get_data(int8_t *temperature, uint8_t *humidity, uint8_t *battery_level) {
    
    gpio_set_level(Pin_enable_divisorR, 1);
    vTaskDelay(pdMS_TO_TICKS(1500)); 

    error_code_t status = Get_sensor_data(temperature, humidity);
    if (status != NoError) {
        return status; // Error en la lectura del sensor
    }

    status = Get_battery_level(battery_level);
    if (status != NoError) {
        return status; // Error en la lectura de la batería
    }

    gpio_set_level(Pin_enable_divisorR, 0);

    return NoError;
}

error_code_t Get_battery_level(uint8_t *battery_level) {
    if (battery_level == NULL) {
        return ADCError; // Error: puntero nulo
    }
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

    int raw = 0, voltage_mv = 0;
    adc_oneshot_read(adc_handle, ADC_CHANNEL_6, &raw);
    adc_cali_raw_to_voltage(cali_handle, raw, &voltage_mv);


    // Calcular el porcentaje de batería en pasos de 10 
    // 0% = 1.80 y 100% = 2.6v
    voltage_mv = voltage_mv - 1800; // Restar 1.8V
    voltage_mv = voltage_mv * 20 / 800; // Calcular porcentaje (0-100%)
    return (uint8_t) voltage_mv * 5;

    return NoError;
}

error_code_t Get_sensor_data(int8_t *temperature, uint8_t *humidity) {
    if (temperature == NULL || humidity == NULL) {
        return SensorError; // Error: puntero nulo
    }

    struct dht11_reading reading = DHT11_read();
    if (reading.status == DHT11_OK) {
        *temperature = reading.temperature;
        *humidity = reading.humidity;
        return NoError;
    }else {
        *humidity = -1; // Error en la lectura de temperatura
        *temperature = -1; // Error en la lectura de humedad
        return SensorError; // Error: lectura fallida
    }

}


error_code_t Show_status_led(error_code_t status){

    //enciende el led blanco si el estado es 0
    if(status == NoError){
        for(int i = 0; i < 3; i++){
            gpio_set_level(Pin_Led_blanco, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(Pin_Led_blanco, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }else{
        //enciende el led rojo si el estado es 1
        for(int i = 0; i < 3; i++){
            gpio_set_level(Pin_Led_rojo, 1);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gpio_set_level(Pin_Led_rojo, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    
    //lo saca por terminal
    

    return NoError; // Return a default value
}

error_code_t Deep_sleep(uint32_t time_ms){
    esp_sleep_enable_timer_wakeup((uint64_t)time_ms * 1000); // en microsegundos
    esp_deep_sleep_start();
    return DeepSleepError;
}

