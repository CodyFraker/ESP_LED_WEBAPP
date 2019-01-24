/*
  WiFi web controller for LED strips using ESP-8266
  
  $$$$
  Written by: Mateusz Jaros
    https://twitter.com/tujaros
  This Sketch is part of bigger project.
  See more details:
    EN: https://www.instructables.com/id/WiFi-LED-Strip-Controller-Based-on-ESP-8266/
    PL: https://majsterkowo.pl/kontroler-wifi-dla-tasm-ledowych/
  $$$$
  
*/

/* Liblaries */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>


/* Network settings */


/* MSC18-C. Be careful while handling sensitive data, such as passwords, in program code */
const char* ssid = "qwerty"; // SSID - your WiFi's name 
const char* password = "password5"; // Password 
const char* device_name = "led"; // you can access controller by http://led.local/

/* MSC03-J. Never hard code sensitive information */
IPAddress ip(192,168,1,111);  // static IP adress of device
IPAddress gateway(192,168,1,1); // Gatway
IPAddress subnet(255,255,255,0); // Network mask

/* Objects */
MDNSResponder mdns;
/* MSC00-J. Use SSLSocket rather than Socket for secure data exchange */
ESP8266WebServer server(80);

String webPage = R"=====(
  <!DOCTYPE HTML>
  <html lang="en">
  <head>
    <meta charset="utf-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LED CONTROLLER</title>
    <link href="https://fonts.googleapis.com/css?family=Lato:400,900&subset=latin-ext" rel="stylesheet">
    <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
    <style> 
      button {
      float: left;
      width: 60px;
      height: 60px;
      font-size: 15px;
      margin: 5px;
      border-radius: 30px;
      -webkit-transition-duration: 0.4s; /* Safari */
      transition-duration: 0.2s;
      cursor: pointer;
      text-align: center;
      font-family: 'Lato', sans-serif;
      font-weight: bold;
      outline:none;
      border: none;
      color:white;  }

    #container {
      max-width: 280px;
      margin: auto;
      margin-top: 50px;
      background-color: #BDB7AA;
      border: 2px solid #AAB0BD;
      border-radius: 15px;
      padding: 5px;
      padding-top: 20px;
      padding-bottom: 20px;
      margin-top: 20px; }

    .power {
      background-color: white;
      color: black; }
    .power:hover {
      border: 2px solid black;
      color: black; }

    #off {
      background-color: black;
      color: white; }
    #off:hover {
      background-color: white;
      border: 2px solid black;
      color: black; }

    #on {
      background-color: red;  
      color: white; }
    #on:hover {
      background-color: white;
      border: 2px solid red;
      color: black; }

    #red {  background-color: #FF191B;  }
    #red:hover {
      background-color: white;
      border: 2px solid #FF191B;
      color: black; }

    #red1 { background-color: #FF5319;  }
    #red1:hover {
      background-color: white;
      border: 2px solid #FF5319;  }

    #red2 { background-color: #FF6A5E;  }
    #red2:hover {
      background-color: white;
      border: 2px solid #FF6A5E;  }

    #red3 { background-color: #FF926B;  }
    #red3:hover {
      background-color: white;
      border: 2px solid #FF926B;  }

    #red4 { background-color: #F3F62F;  }
    #red4:hover {
      background-color: white;
      border: 2px solid #F3F62F;  }

    #green {  background-color: #288F00;  }
    #green:hover {
      background-color: white;
      border: 2px solid #288F00;
      color: black; }

    #green1 { background-color: #3CD45F;  }
    #green1:hover {
      background-color: white;
      border: 2px solid #3CD45F;
      color: black;
      }
    #green2 { background-color: #47E3C6;  }
    #green2:hover {
      background-color: white;
      border: 2px solid #47E3C6;
      color: black; }

    #green3 { background-color: #44C5DE;  }
    #green3:hover {
      background-color: white;
      border: 2px solid #44C5DE;  }

    #green4 { background-color: #289AE3;  }
    #green4:hover {
      background-color: white;
      border: 2px solid #289AE3;
      color: black; }

    #blue { background-color: #122B9C;  }
    #blue:hover {
      background-color: white;
      border: 2px solid #122B9C;
      color: black; }

    #blue1 {  background-color: #6C6CD4;  }
    #blue1:hover {
      background-color: white;
      border: 2px solid #6C6CD4;
      color: black; }

    #blue2 {  background-color: #5D2AE3;  }
    #blue2:hover {
      background-color: white;
      border: 2px solid #5D2AE3;
      color: black; }

    #blue3 {  background-color: #9025DE;  }
    #blue3:hover {
      background-color: white;
      border: 2px solid #9025DE;
      color: black; }

    #blue4 {  background-color: #BE11E3;  }
    #blue4:hover {
      background-color: white;
      border: 2px solid #BE11E3;
      color: black; }

    #white {
      background-color: white;
      color: black; }
    #white:hover {
      background-color: white;
      border: 2px solid black;
      color: black; }

    .function {
      background-color: #5A6170;
      font-size: 10px;  }
    .function:hover {
      background-color: white;
      border: 2px solid #5A6170;
      color: black;
      font-size: 10px;  }

  </style>
  </head>
  <body>
    <div id="container">
      <a href="lightup"><button class="power"><i class="material-icons">brightness_5</i></button></a>
      <a href="lightdown"><button class="power"><i class="material-icons">brightness_3</i></button></a>
      <a href="off"><button id="off"><i class="material-icons">lightbulb_outline</i></button></a>
      <a href="on"><button id="on"><i class="material-icons">wb_incandescent</i></button></a>
      <div style="clear : both"></div>

      <a href="red"><button id="red">R</button></a>
      <a href="green"><button id="green">G</button></a>
      <a href="blue"><button id="blue">B</button></a>
      <a href="white"><button id="white">W</button></a>
      <div style="clear : both"></div>

      <a href="red1"><button id="red1"></button></a>
      <a href="green1"><button id="green1"></button></a>
      <a href="blue1"><button id="blue1"></button></a>
      <a href="flash"><button class="function">FLASH</button></a>
      <div style="clear : both"></div>

      <a href="red2"><button id="red2"></button></a>
      <a href="green2"><button id="green2"></button></a>
      <a href="blue2"><button id="blue2"></button></a>
      <a href="strobe"><button class="function">STROBE</button></a>
      <div style="clear : both"></div>

      <a href="red3"><button id="red3"></button></a>
      <a href="green3"><button id="green3"></button></a>
      <a href="blue3"><button id="blue3"></button></a>
      <a href="fade"><button class="function">FADE</button></a>
      <div style="clear : both"></div>

      <a href="red4"><button id="red4"></button></a>
      <a href="green4"><button id="green4"></button></a>
      <a href="blue4"><button id="blue4"></button></a>
      <a href="smooth"><button class="function">SMOOTH</button></a>
      <div style="clear : both"></div>
    </div>
  </body>
  </html>
  )=====";

void setup(void) {

  delay(1000);

  /* Begin some (un)important things */
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  
  Serial.println("");

  /* Wait for WiFi connection */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  /* Show some important information on Serial Monitor */
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

   /* Give name for the device */
  if (mdns.begin(device_name, WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  /* Show website (in browser), than send IR code */
  server.on("/", []() {
    server.send(200, "text/html", webPage);
    Serial.println("Loaded main page");
    
    Serial.print("Pressed: ");
    Serial.println("on");
  });
  server.on("/lightup", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("lightup");
    delay(1000);
  });
  server.on("/lightdown", []() {
    server.send(200, "text/html", webPage);

    Serial.print("Pressed: ");
    Serial.println("lightdown");
    delay(1000);
  });
  server.on("/off", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("off");
    delay(1000);
  });
  server.on("/on", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("on");
    delay(1000);
  });

  server.on("/red", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("red");
    delay(1000);
  });
  server.on("/green", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("green");
    delay(1000);
  });
  server.on("/blue", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("blue");
    delay(1000);
  });
  server.on("/white", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("white");
    delay(1000);
  });

  server.on("/red1", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("red1");
    delay(1000);
  });
  server.on("/green1", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("green1");
    delay(1000);
  });
  server.on("/blue1", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("blue1");
    delay(1000);
  });
  server.on("/flash", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("flash");
    delay(1000);
  });

  server.on("/red2", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("red2");
    delay(1000);
  });
  server.on("/green2", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("green2");
    delay(1000);
  });
  server.on("/blue2", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("blue2");
    delay(1000);
  });
  server.on("/strobe", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("strobe");
    delay(1000);
  });

  server.on("/red3", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("red3");
    delay(1000);
  });
  server.on("/green3", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("green3");
    delay(1000);
  });
  server.on("/blue3", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("blue3");
    delay(1000);
  });
  server.on("/fade", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("fade");
    delay(1000);
  });

  server.on("/red4", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("red4");
    delay(1000);
  });
  server.on("/green4", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("green4");
    delay(1000);
  });
  server.on("/blue4", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("blue4");
    delay(1000);
  });
  server.on("/smooth", []() {
    server.send(200, "text/html", webPage);
    
    Serial.print("Pressed: ");
    Serial.println("smooth");
    delay(1000);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
