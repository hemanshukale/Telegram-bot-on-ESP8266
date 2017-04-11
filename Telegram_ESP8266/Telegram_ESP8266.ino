
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

  WiFiClientSecure client;

const char* ssid = "Winterfell";
const char* password = "AryaStark";

IPAddress ip1(10, 42, 3, 213);
IPAddress gateway1(10, 42, 3, 1);
IPAddress subnet(255, 255, 255, 0);

const char* host = "api.telegram.org";

const int httpsPort = 443;

// Use web browser to view and copy SHA1 fingerprint of the certificate
const char*  sha1 = "C6 5B FA 5B F7 57 0C 6A 02 85 C1 6F A7 19 6C 36 32 B4 28 21";

String toget = ""; // sends this request to update any recieved messages
String token = ""; // enter token here
String getupdates1 = "/getUpdates?offset=" ;
String getupdates2 = "&timeout=30";

String tosend = "";  // sends this request to send a message to any user - given that the user has initiated the conversation
String push1 = "/sendMessage?chat_id=";
String push2 = "&text=";

int timeout = 30;
unsigned long offset = 0; 
unsigned long lastp = 0;


unsigned long last_response = 0, last_send = 0;
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.config(ip1, gateway1, subnet);
  delay(2000);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(sha1, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }


  toget =  "GET /bot" + token + getupdates1 + getupdates2  + " HTTP/1.1\r\n"; 

  client.print(toget + "Host: " + host + "\r\n" +  "Connection: Keep-Alive\r\n\r\n");
  last_response = millis();     last_send = millis(); // to start counting from here


    Serial.print("Client connected -> ");
    Serial.println(client.connected());
}

void loop() {

  String rep = "";

    
    if(client.available() > 0)
        {
          rep = client.readString();
          Serial.println("Got ->");
//          Serial.println(rep);

    int oind1 = rep.lastIndexOf("update_id");
    int oind2 = rep.indexOf(':' , oind1);
    int oind3 = rep.indexOf(',' , oind1);

    int tind1 = rep.indexOf("text");
    int tind2 = rep.indexOf(':', tind1);
    int tind3 = rep.indexOf("}}", tind2);
    
    int find1 = rep.indexOf("chat");
    int find2 = rep.indexOf("id", find1);
    int find3 = rep.indexOf(":", find2);
    int find4 = rep.indexOf(",", find3);

    int tim1 = rep.indexOf("result");
    int tim2 = rep.indexOf("[]", tim1);

    if (tind2 != -1 && tind3 != -1 && oind2 != -1 && oind1 != -1 && tind1 != -1 && find1 != -1 && oind3 != -1 && find3 != -1 && find4 != -1 )
    {
    String offs = rep.substring(oind2+1, oind3);
          offset = offs.toInt();
          Serial.print("offset -> ");
          Serial.println(offset);
           offset += 1;

    String tex = rep.substring(tind2+2, tind3-1);
          
          Serial.print("text -> ");
          Serial.println(tex);

String chat_id = rep.substring(find3+1, find4);
          Serial.print("id -> ");
          Serial.println(chat_id);

     String txt_to_send = tex ;
   tosend =  "GET /bot" + token + push1 + chat_id + push2 + txt_to_send + " HTTP/1.1\r\n"; 

   client.print(tosend + "Host: " + host + "\r\n" + "Connection: Keep-Alive\r\n\r\n");
    

// text recieved is in tex - all yours :)
    
    }
    else if( tim1 != -1 && tim2 != -2)
    {
      Serial.println("timeout");
    }
    else 
    {
      Serial.println(rep);
    }
           if (!client.connect(host, httpsPort)) {
           Serial.println("connection failed");
          return;
          }

  if (client.verify(sha1, host)) {
    ;//Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  
  String offs = "";
  if (offset != 0 ) offs = String(offset);
  else offs = String("");
  
  toget =  "GET /bot" + token + getupdates1 + offs + getupdates2 + " HTTP/1.1\r\n"; 
  client.print(toget + "Host: " + host + "\r\n" + "Connection: Keep-Alive\r\n\r\n");
          
    last_response = millis();     last_send = millis();

    
    }
    
if (millis() - last_response  > ((timeout+20)*1000 ) && millis() - last_send > 20000)
{
  // in case of no response till timeout + 20 seconds, resends the request every 20 sec
Serial.println("no response for a while.. resending request..");


           if (!client.connect(host, httpsPort)) {
           Serial.println("connection failed");
          return;
          }

  if (client.verify(sha1, host)) {
    ;//Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  
  String offs = "";
  if (offset != 0 ) offs = String(offset);
  else offs = String("");

  
  toget =  "GET /bot" + token + getupdates1 + offs + getupdates2 + " HTTP/1.1\r\n"; 
  client.print(toget + "Host: " + host + "\r\n" + "Connection: Keep-Alive\r\n\r\n");
  last_send = millis();
  
}
        
if (millis() - lastp > 60000 )
{
    Serial.print("Client connected -> ");
    Serial.println(client.connected());
    lastp = millis();
}

  delay(20);
  
  if (Serial.available())
    {
      String a = Serial.readString();
      if (a.indexOf("rrr") != -1 ) 
        {
          ESP.restart(); // for debugging purposes
        }
    }
}

/*
 * 
 * 
    SAMPLE REPLY
    
{"ok":true,"result":[{"update_id":*********,
"message":{"message_id":24,"from":{"id":*********,"first_name":"****","last_name":"****"},"chat":{"id":*********,"first_name":"****","last_name":"****","type":"private"},"date":1491778860,"text":"Boooo"}} ]}

https://api.telegram.org/bot(INSERT TOKEN HERE )/sendMessage?chat_id=*********&text=Hello+World

 * 
 * 
 */
