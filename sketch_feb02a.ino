/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 created 16 Mars 2018
 by TEMAL Chakib
 
 */
//IPAddress ip(10, 8, 14, 250);


#include <SPI.h>
#include <Ethernet.h>

#include <TimerOne.h>

#define INT_N 1  // Numéro de l'interruption
#define PIN_E 3  // Numéro de la broche d'entrée (pour
                 // l'interruption 1, broche 3)
#define PIN_S 7  // Numéro de la broche de commande
#define T 10000  // Demi-période du signal en µs (ici secteur 50Hz)

volatile unsigned long top=0;
volatile unsigned long start=0;
volatile unsigned long duree=0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 8, 14, 248);


EthernetServer server(80);
String HTTPget = "0";
String puissance = "0";
boolean reading = false;
int puiss = 0;

//int ledPin = 9; 

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  //pinMode(2, OUTPUT);
  //pinMode(ledPin, OUTPUT);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  
  //Serial.print("server is at ");
  //Serial.println(Ethernet.localIP());
  Timer1.initialize(30);
  Timer1.attachInterrupt(trigger);
  pinMode(PIN_E, INPUT);
  pinMode(PIN_S, OUTPUT);
  attachInterrupt(INT_N, callback, FALLING);
}


void loop() {
  
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        Serial.write(c);
        
        if(reading && c == ' ') reading = false;
        if(c == '?') reading = true;
        if(reading && c != '?') HTTPget += c;
         
        if (c == '\n' && currentLineIsBlank) {
          //// send a standard http response header
          //Serial.println("**********");
          //Serial.println("puissance = " + HTTPget);
          puissance = HTTPget;
          HTTPget ="";
          duree = puissance.toInt();
          //Serial.println("**********");
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");

          
          client.println("<html>");
          client.println("<head>");
          client.println("<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>");
          
          //client.println("<script src='myscripts.js'> </script>");

          client.println("<script> $(document).ready(function(){ " + "var puissance = 0; $('#rangePuissance').on('input', function(){ console.log($(this).val()); var datas = $(this).val();  ;$.get('sketch_feb02a.ino?'+datas , function(data){ }); }) }) </script>");
                  
          client.println("</head>");
          client.println("<body>");
          // output the value of each analog input pin
          
            //int sensorReading = analogRead(analogChannel);
          client.print("<p id='paragr' style='color:red;font-style: italic;'> choose your value of power </p>");
          //client.print(analogChannel);
          
          //client.print(sensorReading);
          client.print("<input type='range' value='0' max='8000' min='0' step='100' id='rangePuissance' style='display: block; margin-left: auto;margin-right: auto;width: 50%;cursor: pointer;background: transparent;border-color: transparent;color: transparent; direction: rtl;'>");
          

          client.println("</body>");
          
          client.println("</html>");

          //Serial.println("puissance** = " + puissance );
          
          //analogWrite(ledPin, puiss);

          
          break;
        }

        // http request inded 
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
          
          
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
        
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void callback () {
  if (top != 1)
  {
    start = micros();
    top=1;
  }
}

void trigger (){
  if (top=1 && micros() - start >= duree)
  {
    digitalWrite(PIN_S, HIGH);
    delayMicroseconds(5);
    digitalWrite(PIN_S, LOW);
    top=0;
  }
}
