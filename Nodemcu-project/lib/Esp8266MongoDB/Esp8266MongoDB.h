#ifndef Esp8266MongoDB_h
#define Esp8266MongoDB_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Esp8266WebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define MAX_PARAM_LENGTH 30


// Enum tanımı
enum RelayStatus
{
  OFF = 0x0,
  ON = 0x1
};

// Struct tanımı
typedef struct RelayInfo
{
  String apiLink;
  String userId;
  String houseId;
  String roomId;
  String relayId;
  RelayStatus status;
  uint8_t relayPin;
} RelayInfo;

typedef struct UserInfo
{
  String email;
  String password;
  String userId;
} UserInfo;

typedef struct WifiInfo
{
  String ssid;
  String password;
} WifiInfo;

class Esp8266MongoDB
{
public:
  void Init_parameters();
  void WifiConnect();
  void SignIn();
  void SetRelayStatus();
  void GetRelayStatus();
  void Update_relay_status();
  void Get_Device_Parameters();
  void StayLoop();


private:
  // Buraya sınıfın gizli üyelerini ekleyebilirsiniz
  String _token;
  bool _setup_bool;
  void handleRoot();
  void writeParamToEEPROM(int startIndex, String param);
  void readParamFromEEPROM(int startIndex, String &param);
  
};

#endif
