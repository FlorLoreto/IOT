//The code will server the web form with radio buttons and will receive back in string form the action from the GET form 
//so as to enable the programmer to utilize the web form input

#include <TimeAlarms.h>
#include <Time.h>
#include <StopWatch.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
AlarmId syncAviso;
StopWatch reloj(StopWatch::SECONDS);
int status = WL_IDLE_STATUS;
char ssid[] = "WLAN_A31A";  //  your network SSID (name)
char pass[] = "4c50627e8669011a99ea";       // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned int localPort = 2390;      // local port to listen for UDP packets
unsigned long currentMillis;
unsigned long previousMillis;


unsigned long intervalo = 1000;

/****************  FUNCTIONS ******************************************/
// ............. Connect to WiFi LAN  ...............................
void lanConnect(String ssid, String pass) {
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    reloj.start();
    // wait 10 seconds for connection:
    while (reloj.elapsed() <= 10) // this prevents the time from being constantly shown.
    {
      ;
    } reloj.reset();
  }

  Serial.println("Connected to wifi");
  printWifiStatus();
  reloj.start();
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
  while (reloj.elapsed() <= 5) // this prevents the time from being constantly shown.
  { ;
  }
}
WiFiServer server(80);   //web server port
String HttpHeader = String(MaxHeaderLength); 


void setup(){
  //enable serial monitor
  Serial.begin(9600);
  //start Ethernet
  Ethernet.begin(mac, ip, gateway, subnet);
   //initialize variable
  HttpHeader="";

}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
         char c = client.read();
         //read MaxHeaderLength number of characters in the HTTP header
         //discard the rest until \n
         if (HttpHeader.length() < MaxHeaderLength)
         {
           //store characters to string
           HttpHeader = HttpHeader + c; 
         }
         //if HTTP request has ended
         if (c == '\n') { 
           // show the string on the monitor
           Serial.println(HttpHeader);
          // start of web page
           client.println("HTTP/1.1 200 OK");
           client.println("Content-Type: text/html");
           client.println("<html><head></head><body>");
           client.println();
           client.print("<form method=get>");
           client.print("<input type='radio' name=r value='1'> One<br>");
           client.print("<input type='radio' name=r value='2' checked> Two<br>");
           client.print("<input type='radio' name=r value='3'> Three<br>");
            client.print("<input type=submit value=submit></form>");
           client.print("</body></html>");
           //clearing string for next read
           HttpHeader="";
           //stopping client
            client.stop();
         }
       }
     }
   }

}
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
