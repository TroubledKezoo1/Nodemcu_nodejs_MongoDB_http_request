#include <Esp8266MongoDB.h>

bool setup_bool = false;
Esp8266MongoDB myMongo ;

#define setup_bool_pin D5

void setup()
{
  // Seri iletişimi başlat
  Serial.begin ( 115200 ) ;
  pinMode( setup_bool_pin , INPUT );

  setup_bool = digitalRead ( setup_bool_pin );
  
  if ( setup_bool )
    myMongo.Get_Device_Parameters ( ) ;
  myMongo.Init_parameters ( ) ;

}

void loop()
{
  myMongo.SetRelayStatus();
  delay(500);
}
