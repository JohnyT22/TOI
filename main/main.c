// main.c
// TOI 1.projekt
// Autor: Jan Tomeček, FIT
// Vyvýjeno ve VSCode pomocí ESP-IDF rozšíření
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sntp.h"

#include "mqtt_client.h"

//onewire.c
void onewire_task(void *pvParameter);
void dht_task(void *pvParameter);
void wifi_init_sta(void);

//http.c
void httpRequest(const char *url);

//gettime.c 
void obtain_time(void);
void initialize_sntp(void);

//mqtt.c
esp_mqtt_client_handle_t mqtt_app_start(void);

//Neural Network
#include "neural_network.h"

extern int timeOK;
extern int dht_humidity;
extern int dht_temp;
extern float ds_temp;

void logData_task(void *pvParameter) {
    vTaskDelay(5000.0 / portTICK_PERIOD_MS);

    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char strftime_buf[16];

    // Set timezone to UTC
    setenv("TZ", "UTC", 1);
    tzset();

    esp_mqtt_client_handle_t client = mqtt_app_start();

    
    float* nnInput = getNNInput();
    float* nnPrediction;

    while(1) {
        obtain_time();
        time(&now);
        while (!timeOK) vTaskDelay(1000.0 / portTICK_PERIOD_MS);
        
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S", &timeinfo);

        nnInput[0] = ds_temp;
        nnInput[1] = (float)dht_humidity;

        nnPrediction = getNNPrediction();

        float dht_temp_f = (float)dht_temp;
        float chosen_temp = abs(dht_temp_f - nnPrediction[0]) < abs(ds_temp - nnPrediction[0]) ? dht_temp_f : ds_temp;
        ESP_LOGI("logData_task", "Chosen_temp: %.2f\n\fDS_temp: %.2f, DHT_temp: %.2f, Prediction: %.2f", chosen_temp, ds_temp, dht_temp_f, nnPrediction[0]);

        char data[160];
        sprintf(data, "{'time':'%s','temperature':'%.2f','humidity':'%d', 'predTemp':'%.2f', 'predHum':'%.2f'}", strftime_buf, chosen_temp, dht_humidity, nnPrediction[0], nnPrediction[1]);

        int msg_id = esp_mqtt_client_publish(client, "v1/devices/me/telemetry", data, 0, 0, 0);
        ESP_LOGI("logData_task", "id %d, %s", msg_id, data);

        vTaskDelay(600000 / portTICK_PERIOD_MS);
    }
}
void app_main() {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta(); //wifi init

    NNInit();
    initialize_sntp();

    xTaskCreate(&onewire_task, "one_wire", 2048, NULL, 5, NULL);
    xTaskCreate(&dht_task, "dht", 2048, NULL, 5, NULL);
    xTaskCreate(&logData_task, "logdata", 4096, NULL, 5, NULL);
}