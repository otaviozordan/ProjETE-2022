/**
 * Scketch.ino - ESP8266 Sketch
 * Author: Otávio Zordan < Github: @otaviozordan > < Linkedin: https://www.linkedin.com/in/otavio-zordan/ >
 * License: MIT
 * Date: 2022-07-28
 * Description:
 *      This sketch is intended to be used by Team 2106 at PROJETE 2022 held at ETE FMC. 
 *      The objective is to create an ESP8266 board working as a multimeter, for this it
 *      must be connected to an OLED display by the I2C protocol, where important information
 *      will be displayed to the user. It is also necessary that the ESP hosted a web server 
 *      that when it receives a GET request returns a Json with the necessary data for the proper
 *      functioning of our application.
**/

//Bibliotecas para utilização do Display OLED
#include <Wire.h>  
#include "SSD1306Wire.h" 
#include <brzo_i2c.h> 
#include "SSD1306Brzo.h"

// Inicia o display OLED usando brzo_i2c
// D3 (GPIO ...) -> SDA
// D5 (GPIO ...) -> SCL
#define SDA_PIN 0
#define SCL_PIN 0
SSD1306Brzo display(0x3c, SDA_PIN, SCL_PIN);

//Biblioteca para utilização do ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//Dados do WiFi
const char* ssid = "OTAVIO E OLAVO 2.4G";
const char* password = "olavo1011";

//Dados de IP
IPAddress ip(192,168,0,175); //IP do ESP8266
IPAddress gateway(192,168,0,1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO ROTEADOR)
IPAddress subnet(255,255,255,0); //MASCARA DE REDE

//Dados do Servidor
ESP8266WebServer server(80);

//Led para indicar que o ESP recebeu uma requisição
const int led = 2;

void handleRoot() { // Função para tratar a requisição GET do servidor
  digitalWrite(led, 1);
  Serial.println("Recebendo requisição...");

  String JSON;

  JSON = "{\"tensão\": ";
  JSON += analogRead(A0);
/*  
  Para medir corrente vereficar https://www.filipeflop.com/blog/ampliando-as-portas-analogicas-do-nodemcu/
  JSON += ", \"corrente\": ";
  JSON += analogRead(A0);
*/
  JSON += "}";
   
  server.send(200, "text/json", JSON);
  digitalWrite(led, 0);
}

void handleNotFound(){ // Função para tratar requisições não encontradas
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup() {
  //Led de indicação
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  
  //Inicia Monitor Serial para Debug
  Serial.begin(115200);

  //Inicializa display OLED
  display.init();

  //Conecta com o WiFi
  Serial.print("Conectando a ");
  Serial.print(ssid);
  Serial.println("");
  WiFi.mode(WIFI_STA); //Configura o ESP como WIFI_STA (Sem Filtro de Rede)
  WiFi.begin(ssid, password); //Conecta com o SSID e senha definidos anteriormente
  WiFi.config(ip, gateway, subnet); //Configura o IP, GATEWAY e MASCARA DE REDE

  while (WiFi.status() != WL_CONNECTED) { //Espera conexão com o WiFi
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connectado"); 
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //Exibe IP do ESP8266


  if (MDNS.begin("esp8266")) { //Inicia o servidor de nomes do ESP8266
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot); //Trata requisições GET do servidor

  server.on("/inline", [](){ //Trata requisições GET do servidor no endereço /inline
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound); //Trata requisições não encontradas

  server.begin(); //Inicia o servidor
  Serial.println("HTTP server iniciado"); //Exibe mensagem que o servidor está funcionando
}

void loop(void) {
  server.handleClient(); 

}
