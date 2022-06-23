#ifndef DHT_H
#define DHT_H

#include <util/delay.h>
#include <avr/io.h>

#define DHT_READ_INTERVAL_MS 100
#define DHT_DELAY_SETUP_MS 2000
#define DHT_PIN 7

enum DHT_Status {
  OK,
  HUMIDITY_ERROR,
  TEMPERATURE_ERROR,
  CHECKSUM_ERROR,
  TIMEOUT
};

typedef struct DHT_Data {
  float temperature;
  float humidity;
} DHT_Data_t;

void dht_init(void);
enum DHT_Status dht_get_status(void);
void dht_request_data(void);
void dht_wait_for_response(void);
uint8_t dht_receive_data();
enum DHT_Status dht_read_raw_data(DHT_Data_t data);
enum DHT_Status dht_get_humidity(float *humidity);
enum DHT_Status dht_get_temperature(float *temperature);
enum DHT_Status dht_read(DHT_Data_t data);
double dht_get_temperature_celsius(void);
double dht_get_temperature_fahrenheit(void);

#endif