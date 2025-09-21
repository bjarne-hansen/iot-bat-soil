//
// Battery Powered Soil Moisture Sensor Sensor with Deep Sleep and BLE.
// 
#include "main.h"
#include "debug.h"

// *****************************************************************************
// Start of Settings.
// *****************************************************************************
#define PIN_BATTERY       A0                            // The analog PIN to read battery voltage from.
#define PIN_SENSOR        A1                            // The analog PIN to read sensor data from.
#define PIN_SENSOR_POWER  D5                            // The digital PIN to turn on sensor.

const uint64_t sleepDuration = 1 * HOURS;       // Deep sleep in microseconds.
                                                        // Time the device is sleeping until waking up next time.
                                                        // Use SECONDS, MINUTES, HOURS or DAYS defines to make it easier 
                                                        // to read.

const unsigned int sensorSaturationDelay = 500;         // Sensor saturation delay in milliceconds.
                                                        // This is the time from the sensor is turned on until values 
                                                        // are read.

const unsigned long advertisingDuration = 2000;         // Advertising time in ms.
                                                        // How long time should the device broadcast read values via
                                                        // Bluetooth.
// *****************************************************************************
// End of settings.
// *****************************************************************************

const String VERSION = "1.0.1";                         // Software version.   

#define DEVICE_NAME_PREFIX "IoT-"                       // Each Bluetooth device has a name with prefix + MAC address.
                                                        // The Bluetooth Gateway uses this name to identify the devices
                                                        // it should accept data from. Do not change this value unless
                                                        // you also change the Bluetooth Gateway configuration.
                                                        // The Bluetooth Gateway is in the"iot-gateway" project.

RTC_DATA_ATTR uint16_t bootCount = 0;                   // Count of times the device has booted.
BLEServer *server = nullptr;                            // The Bluetooth server.

void setup()
{
  // Start Serial and wail for it to be available. 
  // Debugging can be turned on/off (see main.h).
  debug_begin(115200);                                  
  debug_wait(3000);                                     
  
  // Get device address.
  std::string address = getBluetoothMACAddressEx();
  debug_printf("Device Address: %s\n", address.c_str());

  //Increment boot number and print it every reboot
  ++bootCount;  
  debug_printf("Boot number: %d\n", bootCount);
  debug_printf("Wake-up reason: %s\n", wakeupReasonAsString().c_str());

  // ***************************************************************************
  // Application Specific Code.
  // ***************************************************************************

  // Prepare PINS.
  pinMode(PIN_BATTERY, INPUT);                          // Battery PIN is input.
  pinMode(PIN_SENSOR_POWER, OUTPUT);                    // Power PIN is output.

  // Turn on power to sensor.
  digitalWrite(PIN_SENSOR_POWER, HIGH);                 

  // Make sure the sensor is saturated.
  delay(sensorSaturationDelay);

  // Read battery voltage in millivolts.
  uint16_t bat = readBattery();

  // Read normal analog value of sensor.
  uint16_t moisture;
  for (int i = 0; i < 16; i++)
    moisture += analogRead(PIN_SENSOR);
  moisture /= 16;

  // Print debug information.
  debug_printf("Values: bootCount=%u, bat=%u, moisture=%u\n", bootCount, bat, moisture );
  
  // Turn power off to sensor.
  digitalWrite(PIN_SENSOR_POWER, LOW);

  //
  // Prepare struct for publishing data.
  // Sensor data can be maximum 26 bytes.
  //
  // Typical byte sizes:
  // 
  // bool, byte, char, uint8_t takes up 1 byte.
  // int16_t, uint16_t takes up 2 bytes.
  // int, long, float takes up 4 bytes.
  // long long, double, time_t takes up 8 bytes.
  //
  SensorData sensorData;
  bool success = true;  
  success &= sensorData.add(bootCount);   // uint16_t. The "bootCount" variable is 16 bit unsigned (uint16_t).
  success &= sensorData.add(bat);         // uint16_t. The "bat" (millivolts) varianle is 16 bit unsigned (uint16_t).
  success &= sensorData.add(moisture);    // uint16_t. The "moisture" variable is 16 bit unsigned (uint16_t).
  
  //
  // The total byte usage of the above is 2 + 2 + 2 = 6 bytes out of 26 available, so the below should write "true".
  //
  debug_printf("SensorData: success=%s.\n", success ? "true": "false");

  // ***************************************************************************
  // End of Application Specific Code.
  // ***************************************************************************

  if (success)
  {  
    // Create BLE server. 
    createBLEServer(); 

    // Start advertising sensor data.
    startAdvertising(sensorData);    

    // Wait some time to allow our advertisement to be heard by the gateway.
    delay(advertisingDuration);    
  }
  else
  {
    debug_printf("Unable to advertise sensor values. Capacity exeeded.\n");    
  }

  // Disable all wake-up sources.
  int error; // esp_error_t
  error = esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
  debug_printf("Disabled all wakeup sources: %d\n", error);

  // Enable the timer wake-up source.
  error = esp_sleep_enable_timer_wakeup(sleepDuration);
  debug_printf("Enabled timer wakeup in %d us: %d\n", sleepDuration, error);

  // Go to sleep.
  debug_println("Deep sleep start.");
  debug_wait(500);
  esp_deep_sleep_start();
  // No instructions beyond this point will be executed.
}

uint16_t readBattery() {
  // Read an average value of 16 samples to get a more stable reading.
  uint16_t adc = 0;
  for (int i = 0; i < 16; i++)
     adc += analogRead(PIN_BATTERY);
  adc /= 16;
  return adc;
}

void createBLEServer()
{
  // Get name as a combination of a device name and the unique Bluetooth MAC address.
  std::string name = DEVICE_NAME_PREFIX + getBluetoothMACAddress();

  // Init the BLE device and create a server.
  BLEDevice::init(name);
  server = BLEDevice::createServer();
}

std::string getBluetoothMACAddress()
{
  uint8_t mac[6];
  char macAddressString[18];

  // Read MAC address for Bluetooth.
  esp_read_mac(mac, ESP_MAC_BT);

  // Format MAC address a string of hexadecimal numbers.
  snprintf(macAddressString, sizeof(macAddressString), "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return std::string(macAddressString);
}

std::string getBluetoothMACAddressEx()
{
  uint8_t mac[6];
  char macAddressString[18];

  // Read MAC address for Bluetooth.
  esp_read_mac(mac, ESP_MAC_BT);

  // Format MAC address a string of hexadecimal numbers.
  snprintf(macAddressString, sizeof(macAddressString), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return std::string(macAddressString);
}

void startAdvertising(SensorData &sensorData) 
{
  BLEAdvertising* advertising;

  // No server, no advertizing ...
  if (server == nullptr) return;
  
  // Get current advertising from server and stop it.
  advertising = server->getAdvertising();
  advertising->stop();
  
  // Convert sensor data.
  std::string md((char*)sensorData.data(), sensorData.size());

  // Print manufacturer data as hex.
  #ifdef DEBUG_PRINT  
  uint8_t *ptr = (uint8_t *)md.data();
  for (int i = 0; i < md.size(); i++) debug_printf("[%02X]", *(ptr + i)); debug_println();
  #endif

  // Create advertisement data from the data.
  BLEAdvertisementData advertisementData;
  advertisementData.setFlags(0x1A);
  advertisementData.setManufacturerData(md);

  // Add advertisementData to BLEAdvertising.
  advertising->setAdvertisementData(advertisementData);

  // Start advertising.
  advertising->start();
}

String wakeupReasonAsString()
{
  esp_sleep_wakeup_cause_t wakeup_reason;                       

  // Get wake-up reason.
  wakeup_reason = esp_sleep_get_wakeup_cause();                 

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : return "EXT0"; break;
    case ESP_SLEEP_WAKEUP_EXT1 : return "EXT1"; break;
    case ESP_SLEEP_WAKEUP_TIMER : return "TIMER"; break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : return "TOUCHPAD"; break;
    case ESP_SLEEP_WAKEUP_ULP : return "ULP"; break;
    default : 
      return "DEFAULT"; break;
  }
}

void loop()
{
  // Because of the deep sleep nature of this solution, the loop() is never called.
}