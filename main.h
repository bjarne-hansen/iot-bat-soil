#ifndef ARDUINO 
#define ARDUINO 100
#endif
#include <string>
#include <cstring>

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

#include <pins_arduino.h>

constexpr uint64_t SECONDS = 1000000ULL;          // µs in 1 second
constexpr uint64_t MINUTES = 60 * SECONDS;        // µs in 1 minute
constexpr uint64_t HOURS   = 60 * MINUTES;        // µs in 1 hour
constexpr uint64_t DAYS    = 24 * HOURS;          // µs in 1 day

/*
#define SECONDS 1000000ULL         // Number of microseconds in 1 second.
#define MINUTES 60000000ULL        // Number of microseconds in 1 minute.
#define HOURS   3600000000ULL      // Number of microseconds in 1 hour.
#define DAYS    86400000000ULL     // Number of microseconds in 1 day.
*/

//
// The following line enables debug output.
// #define DEBUG_PRINT
//
// The following line disables debug output.
// #undef DEBUG_PRINT
//
// PLEASE NOTE: 
// On the XIAO C3 wakeup from deep sleep does not work correctly if DEBUG_PRINT 
// is enabled.
//
// #define DEBUG_PRINT
#undef DEBUG_PRINT


class SensorData
{
  private:
    uint8_t bytes[26];
    int index = 0;

    bool hasCapacity(int n) { return index + n < 26; } 

    bool internalAdd(void *src, int size) 
    {
      if (hasCapacity(size)) 
      {
        memcpy((void*)(bytes + index), src, size);
        index += size;
        return true;
      }
      else
        return false;
    }
    
  public:
    void clear() { index = 0; }
    size_t size() { return index; }

    uint8_t *data() { return bytes; }
    
    bool add(bool v) { return internalAdd((void*)&v, sizeof(bool)); } 
    bool add(byte v) { return internalAdd((void*)&v, sizeof(byte)); } 
    bool add(char v) { return internalAdd((void*)&v, sizeof(char)); }     
    bool add(int v) { return internalAdd((void*)&v, sizeof(int)); } 
    bool add(unsigned int v) { return internalAdd((void*)&v, sizeof(unsigned int)); } 
    bool add(long v) { return internalAdd((void*)&v, sizeof(long)); }  
    bool add(unsigned long v) { return internalAdd((void*)&v, sizeof(unsigned long)); }
    bool add(long long v) { return internalAdd((void*)&v, sizeof(long long)); }  
    bool add(unsigned long long v) { return internalAdd((void*)&v, sizeof(unsigned long long)); }    
    bool add(float v) { return internalAdd((void*)&v, sizeof(float)); }  
    bool add(double v) { return internalAdd((void*)&v, sizeof(double)); }     
    bool add(uint16_t v) { return internalAdd((void*)&v, sizeof(uint16_t)); }      
    bool add(int16_t v) { return internalAdd((void*)&v, sizeof(int16_t)); }  
};

void setup();
void loop();

uint16_t readBattery();
void createBLEServer();
std::string getBluetoothMACAddress();
std::string getBluetoothMACAddressEx();
void startAdvertising(SensorData &sensorData);
String wakeupReasonAsString();
