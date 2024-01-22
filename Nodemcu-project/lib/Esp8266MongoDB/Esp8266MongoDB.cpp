#include <Esp8266MongoDB.h>

WiFiClient client;
HTTPClient http;

ESP8266WebServer server(80);

UserInfo  User;
WifiInfo  Wifi;
RelayInfo Relay;




void Esp8266MongoDB::Init_parameters()
{ 
   
  Serial.begin(115200);
  EEPROM.begin(512);
  String pin;
  readParamFromEEPROM ( 1 , Wifi.ssid                          );
  readParamFromEEPROM ( MAX_PARAM_LENGTH, Wifi.password        );
  readParamFromEEPROM ( 2 * MAX_PARAM_LENGTH , User.email      );
  readParamFromEEPROM ( 3 * MAX_PARAM_LENGTH , User.password   );
  readParamFromEEPROM ( 4 * MAX_PARAM_LENGTH , User.userId     );
  readParamFromEEPROM ( 5 * MAX_PARAM_LENGTH , Relay.houseId   );
  readParamFromEEPROM ( 6 * MAX_PARAM_LENGTH , Relay.roomId    );
  readParamFromEEPROM ( 7 * MAX_PARAM_LENGTH , Relay.relayId   );
  readParamFromEEPROM ( 8 * MAX_PARAM_LENGTH , Relay.apiLink   );
  readParamFromEEPROM ( 9 * MAX_PARAM_LENGTH , pin  );
  Relay.relayPin = pin == "2" ? 2 : 0;
 


    
  WifiConnect();
  //SignIn();
  //GetRelayStatus();
  SetRelayStatus();
}

void Esp8266MongoDB::WifiConnect()
{
  WiFi.begin(Wifi.ssid, Wifi.password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Connected to the WiFi network");
    Serial.println("----------------------------------------------");
    Serial.println("SSID       : " + WiFi.SSID());
    Serial.println("IP address : " + WiFi.localIP().toString());
    Serial.println("MAC address: " + WiFi.macAddress());
    Serial.println("Moment     : " + (WiFi.status() == WL_CONNECTED) ? "Connected" : "Disconnected");
    Serial.println("----------------------------------------------");
  }
  else
  {
    Serial.println("Connection Failed trying again ");
    WifiConnect();
  }
}
void Esp8266MongoDB::SignIn()
{
  String url = (String)Relay.apiLink + "/signin";
  Serial.println(url);
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  String postData = "{\"email\":\"" + User.email + "\",\"password\":\"" + User.password + "\"}";
  int httpCode = http.POST(postData);
  if (httpCode > 0)
  {
    DynamicJsonDocument jsonBuffer(256);
    DeserializationError error = deserializeJson(jsonBuffer, http.getString());
    String response = http.getString();
    Serial.println("HTTP response code: " + String(httpCode));
    _token = (String)jsonBuffer["token"];
    Serial.println("----------------------------------------------");
    Serial.println("Token: " + _token);
    Serial.println("----------------------------------------------");
    if (error)
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
  }
  else
  {
    Serial.println("HTTP request failed");
    Serial.println("Error code: " + String(httpCode));
  }
  http.end();
}
void Esp8266MongoDB::GetRelayStatus()
{
  String address = "/houses/" + User.userId + "/" + Relay.houseId + "/rooms/" + Relay.roomId + "/relays/" + Relay.relayId;
  http.begin(client, Relay.apiLink + address);
  http.addHeader("Authorization", "Bearer " + _token);
  int httpCode = http.GET();

  if (httpCode > 0)
  {
    DynamicJsonDocument jsonBuffer(256);
    DeserializationError error = deserializeJson(jsonBuffer, http.getString());
    if (error)
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
    }
    Serial.println(http.getString());
    Serial.println("----------------------------------------------");
    Serial.println("Status: " + (String)jsonBuffer["status"]);
    Serial.println("----------------------------------------------");
    Relay.status = (jsonBuffer["status"] == "ON") ? ON : OFF;
    Serial.println(String(Relay.status));
  }
  http.end();
  // Hata durumunda mevcut durumu koru.
}

void Esp8266MongoDB::SetRelayStatus()
{
  SignIn();
  GetRelayStatus();
  digitalWrite(Relay.relayPin, Relay.status);
  Serial.print(Relay.status);
}

void Esp8266MongoDB::Update_relay_status()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    SetRelayStatus();
  }
  else
  {
    Serial.println("Connecting Failed trying again");
    WifiConnect();
  }
}

void Esp8266MongoDB::Get_Device_Parameters()
{
  Wifi.ssid = "Saraç";
  Wifi.password = "86583282";
  WifiConnect();
  //WiFi.softAP("SmartNest", "admin123456789");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());
  // Web sunucusunu başlat
  server.on("/parametres", [this]()
            { handleRoot(); });
  server.begin ( );
  _setup_bool = true;
  Serial.println ( "Veriler alınıyor." );
  while ( _setup_bool )
  {
    server.handleClient();
  }
  EEPROM.begin(512);
  writeParamToEEPROM ( 1 , ( String ) Wifi.ssid               );
  writeParamToEEPROM ( MAX_PARAM_LENGTH, (String)Wifi.password);
  writeParamToEEPROM ( 2 * MAX_PARAM_LENGTH , User.email      );
  writeParamToEEPROM ( 3 * MAX_PARAM_LENGTH , User.password   );
  writeParamToEEPROM ( 4 * MAX_PARAM_LENGTH , User.userId     );
  writeParamToEEPROM ( 5 * MAX_PARAM_LENGTH , Relay.houseId   );
  writeParamToEEPROM ( 6 * MAX_PARAM_LENGTH , Relay.roomId    );
  writeParamToEEPROM ( 7 * MAX_PARAM_LENGTH , Relay.relayId   );
  writeParamToEEPROM ( 8 * MAX_PARAM_LENGTH , Relay.apiLink   );
  writeParamToEEPROM ( 9 * MAX_PARAM_LENGTH , ( String ) Relay.relayPin  );
 
 
}
void Esp8266MongoDB::handleRoot()
{
  
  if (server.method() == HTTP_POST)
  {
    
    String receivedData = server.arg("plain"); // Gövdeyi al
    Serial.println(receivedData);
    // Gelen JSON verisini deserialize etmek için bir JsonDocument oluştur
    DynamicJsonDocument StartBuffer ( 512 );
    
    // Gelen JSON verisini parse et
    DeserializationError error = deserializeJson ( StartBuffer , receivedData );

    
    if ( error )
    {
      Serial.print("deserializeJson() failed: ");
      // Hata olursa, hata mesajını göster
      Serial.print("JSON parse hatası: ");
      Serial.println(error.c_str());
      server.send(400, "text/plain", "Geçersiz JSON verisi");
      return;
    }

    // JSON verisini ayrıştır
    Wifi.ssid      = ( String ) StartBuffer [     "ssid"       ] ; 
    Wifi.password  = ( String ) StartBuffer [ "ssid_password"  ] ;
    User.email     = ( String ) StartBuffer [     "email"      ] ;
    User.password  = ( String ) StartBuffer [ "email_password" ] ;
    User.userId    = ( String ) StartBuffer [     "userId"     ] ;
    Relay.apiLink  = "http://192.168.1.40:3000";
    Relay.houseId  = ( String ) StartBuffer [     "houseId"    ] ;
    Relay.roomId   = ( String ) StartBuffer [     "roomId"     ] ;
    Relay.relayId  = ( String ) StartBuffer [     "relayId"    ] ;
    Relay.relayPin = D4;
    _setup_bool = false;
    // Diğer işlemler burada gerçekleştirilir

    server.send(200, "text/html", "Veri alındı!");
  }
  else
  {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}
void Esp8266MongoDB :: writeParamToEEPROM( int startIndex, String param) {
  int currentIndex = startIndex;

  for (int i = 0; i < (int) param.length(); i++) {
    EEPROM.write(currentIndex, param[i]);
    currentIndex++;
  }
    
  EEPROM.write(currentIndex, '\0'); // NULL karakteriyle bitir
  EEPROM.commit(); // Değişiklikleri kaydet
}

void Esp8266MongoDB::readParamFromEEPROM( int startIndex, String &param) {
  param = "";
  char currentChar = EEPROM.read(startIndex);
  int currentIndex = startIndex;

  while (currentChar != '\0' && currentIndex < (startIndex + MAX_PARAM_LENGTH)) {
    param += currentChar;
    currentIndex++;
    currentChar = EEPROM.read(currentIndex);
  }
}
