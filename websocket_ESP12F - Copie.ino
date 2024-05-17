/*
 Composition: ESP-01S, ADS1115 (cause meilleure résolution, et utilisation bus I2C), et 4 panneaux solaires de lampes de jardin vendus en grande surface
 Les 4 panneaux sont orientés à 20° les uns des autres en horizontale et verticale locales.
 Un écart de puissance délivrée par chaque panneau montre une orientation perfectible des panneaux solaires, le but étant d'être le plus en face du soleil.
 La moyenne de production des 4 petits panneaux permet d'estimer la cpaacité de puissance des gros panneaux.
 Mais pourquoi?
 Les gros panneaux alimentent la maison, pour utilisation sans revente.
 L'eau-chaude est produite par une chaudière à granulés, mais si la puissance produite par l'installation photovoltaïque, une résistance chauffera l'eau.
 Derrière tout ça, l'info sera récupérée par la domotique en place (protocole mqtt, et serveur home assistant).

 Une partie html lue par un client qui s'y connecte directement verra les infos sur une page html, avec script javascript et mise en page css (voir autres fichiers du projet).
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
//#include <Wire.h>
//#include "ADS1X15.h"
#include "index.h"

//ADS1115 0x48 et 0x49 (Addr à Vdd)
//SDA I02
//SCL I00
//ESP en 3,3V
//ADS1115 en 5V
// Gain 1: +- 4.096V, 32767

const char* ssid = "SSID";
const char* password = "password";

//partie rajoutée pour serveur d'essai
int val_h=0;
int val_b=2000;
int val_g=4000;
int val_d=6000;
int val_Max=24000;
//

String texteAenvoyer = "";
String commentaire = "";

//ADS1115 ADS_Haut_Bas(0x48);
//ADS1115 ADS_Gauche_Droite(0x49);

ESP8266WebServer server(80);                        // Web server on port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  Serial.printf("Num: " ,String(num));

  Serial.printf(" / Payload: ", payload);
  Serial.printf(" / Length: ", length);
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received text: %s\n", num, payload);
      // Send a response back to the client
      String echoMessage = "Received:  " + String((char*)payload);
      webSocket.sendTXT(num, echoMessage);
      break;
  }
  Serial.printf(" / Type: ", (String(type).c_str()));
}

void setup() {
  Serial.begin(9600);
  delay(500);
  //Wire.begin();
  //delay(1000);

  //ADS_Haut_Bas.begin();
  //ADS_Gauche_Droite.begin();
  
  //ADS_Haut_Bas.setGain(1);
  //ADS_Gauche_Droite.setGain(1);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Serve a basic HTML page with JavaScript to create the WebSocket connection
  server.on("/", HTTP_GET, []() {
    Serial.println("Web Server: received a web page request");
    String html = HTML_CONTENT;  // Use the HTML content from the index.h file
    server.send(200, "text/html", html);
  });

  server.begin();
  Serial.print("ESP8266 Web Server's IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Handle client requests
  server.handleClient();

  // Handle WebSocket events
  webSocket.loop();

  //val_x = correction + lecture tension
  //correction à metttre en place après premières mesures

  //int16_t val_h = 0 + ADS_Haut_Bas.readADC_Differential_0_1();
  //Serial.print("haut");
  //Serial.println(val_h);
  //delay(200);
  
  //int16_t val_b = 0 + ADS_Haut_Bas.readADC_Differential_2_3();
  //Serial.print("bas");
  //Serial.println(val_b);
  //delay(200);
  
  //int16_t val_g = 0 + ADS_Gauche_Droite.readADC_Differential_0_1();
  //Serial.print("gauche");
  //Serial.println(val_g);
  //delay(200);
  
  //int16_t val_d = 0 + ADS_Gauche_Droite.readADC_Differential_2_3();
  //Serial.print("droite:");
  //Serial.println(val_d);
  //delay(200);
  texteAenvoyer = "";
  commentaire = "";
  texteAenvoyer = texteAenvoyer + (String(val_h));
  texteAenvoyer = texteAenvoyer + "|";
  texteAenvoyer = texteAenvoyer + (String(val_b));
  texteAenvoyer = texteAenvoyer + "|";
  texteAenvoyer = texteAenvoyer + (String(val_g));
  texteAenvoyer = texteAenvoyer + "|";
  texteAenvoyer = texteAenvoyer + (String(val_d));
  texteAenvoyer = texteAenvoyer + "|";
  texteAenvoyer = texteAenvoyer + (String(commentaire));
  webSocket.sendTXT(0,texteAenvoyer );

  val_h=val_h+1000;
  val_b=val_b+1000;
  val_g=val_g+1000;
  val_d=val_d+1000;

  if (val_h>=val_Max) {
    val_h=0;
  }

  if (val_b>=val_Max) {
    val_b=0;
  }

  if (val_g>=val_Max) {
    val_g=0;
  }

  if (val_d>=val_Max) {
    val_d=0;
  }

  delay(200);
  // TODO: Your code here
}
