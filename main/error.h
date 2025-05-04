#ifndef ERROR_H
#define ERROR_H

typedef enum{
    NoError = 0,
    WiFiError = 1,
    MQTTError = 2,
    SensorError = 3,
    BatteryError = 4,
    DeepSleepError = 5,
    GPIOError = 6,
    ADCError = 7,
    UnknownError = 8,
    JSONError = 9
} error_code_t;

#endif