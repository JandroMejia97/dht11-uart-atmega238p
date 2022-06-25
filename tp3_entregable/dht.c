#include "dht.h"

enum DHT_Status DHT_State;

#define DHT11_MIN_TEMP                0
#define DHT11_MAX_TEMP               50
#define DHT11_MIN_HUM                20
#define DHT11_MAX_HUM                90
#define DHT11_DELAY                  50
#define DHT11_AWAIT_MAX_RETRIES      50
#define DHT11_INIT_MAX_RETRIES       70
#define DHT11_DELAY_FOR_ZERO         35
#define DHT11_RESPONSE_MAX_RETRIES  100
#define DHT11_DELAY_RETRY_MS          2

/**
 * @brief Initializes the DHT11 sensor.
 */
void dht_init(void) {
  _delay_ms(DHT_DELAY_SETUP_MS);
  DHT_State = OK;
}

/**
 * @brief Reads the state of the DHT11 sensor
 * 
 * @return enum DHT_Status 
 */
enum DHT_Status dht_get_status() {
  return DHT_State;
}

/**
 * @brief Requests the sensor to start measuring.
 */
void dht_request_data() {
  DDRB |= (1 << DHT_PIN); // Set pin as output
  PORTB &= ~(1 << DHT_PIN); // Put a zero on the DHT_PIN port
  _delay_ms(DHT_READ_INTERVAL_MS);
  PORTB |= (1 << DHT_PIN); // Put a one on the DHT_PIN port
}

/**
 * @brief Waits for the sensor to respond.
 */
void dht_wait_for_response() {
  uint8_t retries = 0;
  DDRB &= ~(1 << DHT_PIN); // Set pin as input
  // Wait for a zero on the DHT_PIN port (20-40us)
  while (PIND & (1 << DHT_PIN)) {
    retries += 2;
    _delay_ms(DHT11_DELAY_RETRY_MS);

    if (retries > DHT11_AWAIT_MAX_RETRIES) {
      DHT_State = TIMEOUT;
      break;
    }
  }

  if (DHT_State == OK) {
    // Wait for a one on the DHT_PIN port (low for ~80us)
    retries = 0;
    while (!(PIND & (1 << DHT_PIN))) {
      retries += 2;
      _delay_ms(DHT11_DELAY_RETRY_MS);

      if (retries > DHT11_RESPONSE_MAX_RETRIES) {
        DHT_State = TIMEOUT;
        break;
      }
    }

    // Wait for a zero on the DHT_PIN port (high for ~80us)
    retries = 0;
    while (PIND & (1 << DHT_PIN)) {
      retries += 2;
      _delay_ms(DHT11_DELAY_RETRY_MS);

      if (retries > DHT11_RESPONSE_MAX_RETRIES) {
        DHT_State = TIMEOUT;
        break;
      }
    }
  }
}

/**
 * @brief Reads the data from the sensor.
 * 
 * @return uint8_t - The data read from the sensor.
 */
uint8_t dht_receive_data() {
  uint8_t i, data = 0, retries;
  for (i = 7; i >= 0; i--) {
    retries = 0;
    // There is always a zero on the DHT_PIN port for 50us
    while (!(PIND & (1 << DHT_PIN))) {
      retries += 2;
      _delay_ms(DHT11_DELAY_RETRY_MS);

      if (retries > DHT11_INIT_MAX_RETRIES) {
        DHT_State = TIMEOUT;
        i = -1;  // break the outer loop
        break;  // break while loop
      }
    }

    if (DHT_State == OK) {
      // Reading the data. 26-28us means a zero, 70us means a one
      _delay_ms(DHT11_DELAY_FOR_ZERO);
      if (PIND & (1 << DHT_PIN)) {
        data |= (1 << i);
      }

      retries = 0;
      while (PIND & (1 << DHT_PIN)) {
        retries += 2;
        _delay_ms(DHT11_DELAY_RETRY_MS);
        if (retries > DHT11_RESPONSE_MAX_RETRIES) {
          DHT_State = TIMEOUT;
          break;  // break while loop
        }
      }
    }
  }
  return data;
}

/**
 * @brief Reads the temperature and humidity from the sensor.
 * 
 * @param data - The data read from the sensor.
 * @return enum DHT_Status - The status of the sensor.
 */
enum DHT_Status dht_read_raw_data(DHT_Data_t data) {
  uint8_t buffer[5] = {0, 0, 0, 0, 0};
  uint8_t i;
  int8_t j;
  i = j = 0;
  DHT_State = OK;

  if (DHT_State == OK) {
    DHT_State = OK;
    dht_request_data();
    dht_wait_for_response();
  }

  if (DHT_State == OK) {
    // Read 5 bytes of data
    for (i = 0; i < 5; i++) {
      buffer[i] = dht_receive_data();
    }
  }

  if (DHT_State == OK) {
    // Check the checksum
    if ((uint8_t)(buffer[0] + buffer[1] + buffer[2] + buffer[3]) != buffer[4]) {
      DHT_State = CHECKSUM_ERROR;
    } else {
      // Convert the data
      data.humidity = buffer[0] + buffer[1];
      data.temperature = (buffer[2] & 0x7F) + ((buffer[3] & 0x80) >> 7);
    }

  }

  return dht_get_status();
}

/**
 * @brief Reads the temperature and humidity from the sensor.
 * 
 * @param data - The data read from the sensor.
 * @return enum DHT_Status - The status of the sensor.
 */
enum DHT_Status dht_read(DHT_Data_t data) {
  enum DHT_Status status = dht_read_raw_data(data);
  if (status == OK) {
    data.temperature = (data.temperature * 9 / 5) + 32;
    if (data.temperature < DHT11_MIN_TEMP || data.temperature > DHT11_MAX_TEMP) {
      status = TEMPERATURE_ERROR;
    }
    data.humidity = (data.humidity * 100) / 256;
    if (data.humidity < DHT11_MIN_HUM || data.humidity > DHT11_MAX_HUM) {
      status = HUMIDITY_ERROR;
    }
  }
  return status;
}

enum DHT_Status dht_get_temperature(float *temperature) {
  DHT_Data_t data;
  enum DHT_Status status = dht_read(data);
  if (status == OK) {
    *temperature = data.temperature;
  }
  return status;
}

enum DHT_Status dht_get_humidity(float *humidity) {
  DHT_Data_t data;
  enum DHT_Status status = dht_read(data);
  if (status == OK) {
    *humidity = data.humidity;
  }
  return status;
}
