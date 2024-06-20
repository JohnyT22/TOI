// dht.c
// TOI 1.projekt
// Autor: Jan Tomeček, FIT
// Vyvýjeno ve VSCode pomocí ESP-IDF rozšíření
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "dht11.h"

#define DHT_DEBUG 0

#define GPIO_DHT            (27)
#define SAMPLE_PERIOD       (600000)   // milliseconds

int dht_temp = 0;
int dht_humidity = 0;

void dht_task(void *pvParameter){

    vTaskDelay(2000.0 / portTICK_PERIOD_MS);

    DHT11_init(GPIO_DHT);

    while(1) {
        int num_of_tries = 0;
        struct dht11_reading reading =  DHT11_read();

        while(reading.temperature == -1 && num_of_tries++ < 5){
            vTaskDelay(2000.0 / portTICK_PERIOD_MS);
            reading =  DHT11_read();
        }

        dht_humidity = reading.humidity;
        dht_temp = reading.temperature;

        if(DHT_DEBUG){
            ESP_LOGI("dht", "Temperature is %d", reading.temperature);
            ESP_LOGI("dht", "Humidity is %d", reading.humidity);
        }

        vTaskDelay(SAMPLE_PERIOD / portTICK_PERIOD_MS); 
    }
}