/*
  Funciona con:
  - Pines A0-A5 para LCD
  - Nuevos pines para LED RGB y buzzer
  - Solo una alarma que se repite a diario
  - la librería TimeAlarms solo tiene 6 alarmas
*/

//===== Libraries included ======================================
#include <Time.h>
#include <TimeAlarms.h>
#include <StopWatch.h>
#include <LiquidCrystal.h>
/************  This program examines the TimeAlarm library  *******/
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
//**************** Predsefined constants  *********************
//................  LED(RGB) pins ..............................
const int redPin = 3;
const int greenPin = 4;
const int bluePin = 8;
//..................  Pin 13 .....................................
const int trecePin = 13;
// ...............  buzzer & switchPin ...................................
const int buzzPin = 9;
const int switchPin = 2;
// -----------------  duty cycle --------------------------------
const int redInt = 254;
const int greenInt = 127;
const int blueInt = 254;
//-------------------  time  ---------------------------------
long unsigned intervalo = 1000;
//*****************  selectable variables *********************
//==================== Variables ===============================
//---------------  date & time -------------------------------
String hh = "";
String mm = "";
// ---------------- working days alarms -----------------------
int alarmHora;
int alarmMinuto;
const int alarmSegundo = 0;
int k = 0;
// -------------   alarms ID ----------------------------------
AlarmId AlDay;
AlarmId syncAviso;
//------------ alarms state -------------------------------------
int alarmIdState;
int volatile alarmEnabled;
//------------------- other time related variables------------------
long unsigned currentMillis;
long unsigned previousMillis;
//------------------ debounce ----------------------------------
long debouncing_time = 15; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;
//--------------  auxiliary -------------------------------------
char c;
String instruct = "";
String input = "";
int primerNumero = 0;
String respuesta = "";
boolean finAjuste = false;
int flag;
String serialData = "";
//--------------  LCD variables  -------------------
//constants for the number of rows and columns in the LCD
const int numRows = 2;
const int numCols = 16;
int count;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);
int status = WL_IDLE_STATUS;
String ssid="";  //  your network SSID (name)
String pass="";       // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
//-------------  object creation   ------------------------------
StopWatch reloj(StopWatch::SECONDS);
/*
*/
//*******************  FUNCTIONS ****************************************
//=== function alarm Service : ===========================================
void Alarma() {
  flag = 1;
  reloj.start();
}

void lightAndSound() {
  Serial.printf("Alarma %d, %d \n", 0, flag);
  Serial.println ("Pre-oye como va mi ritmo");
  if (reloj.elapsed() <= 5) {
    digitalWrite(redPin, redInt);
    digitalWrite(trecePin, HIGH);
    analogWrite(buzzPin, 127);
    lcd.setCursor(6, 1);
    lcd.print(hour()); lcd.print(":"); lcd.print(minute()); lcd.print(":"); lcd.print(second());
    Serial.println ("oye como va mi ritmo 1");
  }
  else if (reloj.elapsed() > 5 && reloj.elapsed() <= 7 ) {
    digitalWrite(redPin, redInt / 2); digitalWrite(greenPin, greenInt / 2);
    digitalWrite(trecePin, LOW );
    analogWrite(buzzPin, 0);
    Serial.println ("oye como va mi ritmo 2");
    lcd.setCursor(6, 1);
    lcd.print(hour()); lcd.print(":"); lcd.print(minute()); lcd.print(":"); lcd.print(second()); lcd.print("     ");
  }
  else if (reloj.elapsed() > 7 && reloj.elapsed() <= 12 ) {
    digitalWrite(greenPin, greenInt);
    digitalWrite(redPin, 0);
    analogWrite(buzzPin, 254);
    digitalWrite(trecePin, HIGH);
    Serial.println ("oye como va mi ritmo 3");
    lcd.setCursor(6, 1);
    lcd.print(hour()); lcd.print(":"); lcd.print(minute()); lcd.print(":"); lcd.print(second()); lcd.print("     ");

  }
  else if (reloj.elapsed() > 12 && reloj.elapsed() <= 15 ) {
    digitalWrite(greenPin, greenInt / 2); digitalWrite(bluePin, blueInt / 2);
    digitalWrite(redPin, 0);
    analogWrite(buzzPin, 189);
    digitalWrite(trecePin,  LOW);
    Serial.println ("oye como va mi ritmo 4");
    lcd.setCursor(6, 1);
    lcd.print(hour()); lcd.print(":"); lcd.print(minute()); lcd.print(":"); lcd.print(second()); lcd.print("     ");
  }
  else if (reloj.elapsed() > 15) {
    digitalWrite(trecePin, LOW);
    digitalWrite(greenPin, 0);
    digitalWrite(bluePin, 0);
    digitalWrite(redPin, 0);
    analogWrite(buzzPin, 0);
    Serial.println ("Se acabo, se acabo !");
    reloj.reset();
    flag = 0;
    return;//control returns to main loop.
  }
}

//=== function1 to print the command list:  ===========================
void printHelp1() {
  Serial.println ("*********Instrucciones para WiFi ID***************");
  Serial.println ("1. SSID: Introduzca el digito '1' y luego los caracteres de la SSID (WLAN_A31A p.e.)");
  Serial.print ("WLAN_A31A\t");
  Serial.println ("Termine con un '/' ");
}
//=== function2 to print the command list:  ===========================
void printHelp2() {
  Serial.println ("************Instrucciones WiFi clave WAP2 ******************");
  Serial.print ("2. clave WAP2: Introduzca el digito '2' y luego en orden la clave \t");
  Serial.println ("Termine con un '/' ");
}
//=== function3 to print the command list:  ===========================
void printHelp3() {
  Serial.println ("Instrucciones para ajustes de la alarma");
  Serial.print ("3. Hora e alarma: Introduzca el digito '3' y luego en orden la hora: HH y minutos: MM\t");
  Serial.println ("Termine con un '/' ");
}
//=== function to collect data throgh serial monitor:  ==================
void getEntry(String * devol1, int *devol2) {
  String salida = "";
  // Read any serial input
  while (Serial.available() > 0)
  {
    c = (char) Serial.read();
    Serial.print("input caracter "); Serial.println(c);
    input = input + c; // Read in one char at a time
    //Serial.print("inputagregado "); Serial.println(input);
    // Serial.print("longitud "); Serial.println(input.length());
    //Serial.print("Ultimo character "); Serial.println(input[input.length() - 1]);
    if (input[input.length() - 1] == '/') {
      salida = input.substring(0, input.length() - 1);
      Serial.print("input de salida "); Serial.println(salida);
      input = "";
      *devol1 = salida;
      *devol2 = salida[0] - '0';
      Serial.printf("tipo datos   %d, %d\n", *devol2, salida[0]);
      switch (*devol2) {
        case 1 :
          Serial.print("datos de SSID: "); Serial.println(salida.substring(1, input.length() - 1));
          Serial.flush();
          break;
        case 2 :
          Serial.print("datos de WAP: "); Serial.println(salida.substring(1, input.length() - 1));
          Serial.flush();
          break;
        case 3 :
          Serial.print("alarma dia laborable: "); Serial.println(salida.substring(1, 3) + ":" + salida.substring(3, 5) + ":" + alarmSegundo);
          Serial.flush();
          break;
      }
    }
  }
}
//=== function convert strings to nuneric :  ====================
void stringtoNumber(String instruct) {
  if (instruct.substring(0, 1) == "1") {
    ssid = instruct.substring(1, instruct.length());
    k = 1;
  }
  if (instruct.substring(0, 1) == "2") {
    pass = instruct.substring(1, instruct.length());
    k = 2;
  }
  if (instruct.substring(0, 1) == "3") {
    hh = instruct.substring(1, 3);
    mm = instruct.substring(3, 5);
    alarmHora = hh.toInt();
    alarmMinuto = mm.toInt();
    k = 3;
    AlDay =  Alarm.alarmRepeat( alarmHora, alarmMinuto, 0, Alarma);
  }

  if (k == 3) {// Define end of adjustment, setTime and declare Alarms.
    finAjuste = true;
  }
}
//========================   Install the interrupt routines ======================================.
//---------------------------  debounce ---------------------------------------------------------
void debounceInterrupt() {
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    ServicioBoton();
    last_micros = micros();
  }
}
//----------------------- interrupt service -------------------------------------------------
void ServicioBoton() {
  int alarmStateAnt = alarmEnabled;
  int flagAnt = flag;

  digitalWrite(trecePin, LOW);
  digitalWrite(greenPin, 0);
  digitalWrite(bluePin, 0);
  digitalWrite(redPin, 0);
  analogWrite(buzzPin, 0);
  Serial.println ("Se acabo, se acabo !");
  Serial.printf("Service boton\n"); Serial.printf("alarmEnabled:   %d\n", alarmEnabled);
  if ( alarmStateAnt == 1 && flagAnt == 0) {

    Alarm.disable(AlDay);
    alarmIdState = 0;

    // reloj.stop();
    alarmEnabled = 0;
    return;
  }
  else if ( alarmStateAnt == 0 && flagAnt == 0) {

    Alarm.enable(AlDay);
    alarmIdState = 1;

    //reloj.stop();
    alarmEnabled = 1;
    return;
  }
  else if ( alarmStateAnt == 0 && flagAnt == 1) {

    Alarm.enable(AlDay);
    alarmIdState = 1;
    //reloj.stop();
    alarmEnabled = 1;
    return;
  }
  else if ( alarmStateAnt == 1 && flagAnt == 0) {

    Alarm.enable(AlDay);
    alarmIdState = 0;

    //reloj.stop();
    flag = flagAnt;
    alarmEnabled = 0;
    return;
  }
  else if ( alarmStateAnt == 1 && flagAnt == 1) {

    Alarm.disable(AlDay);
    alarmIdState = 0;

    //reloj.stop();
    alarmEnabled = 0;
    return;
  }
  else {
    return;
  }
}
//================= WiFi =========================================
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
  reloj.reset();
}
// ............. send an NTP request to the time server at the given address ...........
unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

//............. print info on the network you're attached to.................
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
}
//............. Synchronisation routine .................
void Synchro() {
  Serial.println("Soy una puta Alarma ");
  reloj.start();
  // send an NTP packet to a time server
  sendNTPpacket(timeServer); 
  // wait to see if a reply is available
  while (reloj.elapsed() <= 3) // this prevents the time from being constantly shown.
  {
    ;
  }
  reloj.reset();
  if ( Udp.parsePacket() ) {
    Serial.println("packet received");
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);
    setTime(epoch);
    int anno = year();
    Serial.printf("The UTC Date is %d-%d-%d\n ", day(), month(), year());
    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print(hour()); // print the hour (86400 equals secs per day)
    Serial.print(':');
    Serial.print(minute()); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    Serial.println(second()); // print the second
    //Adjusting DST for Central Europe Time
    unsigned long europeanEpoch = epoch + adjustDstEurope(anno);
    setTime(europeanEpoch);
  }
}
//............ function that adjusts time from UTC for the European TZ ...................

int adjustDstEurope(int anyo)
{
  // last sunday of march
  int beginDSTDate =  (31 - (5 * year() / 4 + 4) % 7);
  Serial.println(beginDSTDate);
  int beginDSTMonth = 3;
  //last sunday of october
  int endDSTDate = (31 - (5 * year() / 4 + 1) % 7);
  Serial.println(endDSTDate);
  int endDSTMonth = 10;
  // DST is valid as:
  if (((month() > beginDSTMonth) && (month() < endDSTMonth))
      || ((month() == beginDSTMonth) && (day() >= beginDSTDate))
      || ((month() == endDSTMonth) && (day() <= endDSTDate)))
    return 7200;  // DST europe = utc +2 hour
  else return 3600; // nonDST europe = utc +1 hour
}

//---------------- setup ---------------------------------------------
void setup() {
  while (!Serial);
  Serial.begin(9600);   // Open serial port (9600 bauds).
  //-------- Define pins modes -----------------------
  pinMode(trecePin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  digitalWrite(trecePin, LOW);
  digitalWrite(greenPin, 0);
  digitalWrite(redPin, 0);
  digitalWrite(bluePin, 0);
  //----------- LCD ----------------------------------

  attachInterrupt( digitalPinToInterrupt(switchPin), ServicioBoton, RISING);
  alarmEnabled = 1;
  //----------- Enable all alarms ------------------------

  Alarm.enable(AlDay);
  alarmIdState = 1;
  alarmEnabled = 1;
  lcd.begin(numCols, numRows);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarma: ");
  lcd.setCursor(7, 0);
  lcd.print("ON"); lcd.print("     ");
  // this string is 8characters long
  lcd.setCursor(0, 1);
  lcd.print("Hora: ");


  //----------  Initial screen -----------------------
  printHelp1();
  printHelp2();
  printHelp3();
  Serial.printf ("hora inicial %d: %d: %d y %d-%d-%d\n", hour(), minute(), second(), day(), month(), year());
  Serial.printf ("alarmEnabled %d\n", alarmEnabled);
  pinMode(switchPin, INPUT_PULLUP);
  lanConnect(ssid, pass);
  Synchro();
  Serial.printf(" Hora %d H:%d M:%d S\n", hour(), minute(), second());
  syncAviso = Alarm.alarmRepeat(3, 1, 0, Synchro);
  Serial.printf(" Hora de Alarma %d H:%d M:%d S\n",  alarmHora,alarmMinuto,alarmSegundo);
}
//---------------- loop ---------------------------------------------

void loop() {
  //----------------  Data Entry (date, time and alarm settings ----------------
  while (finAjuste == false )
  { getEntry(&serialData, &primerNumero);
    stringtoNumber(serialData);
  }
  //----------------  Alarm warning with buzzer an RGB LED --------------------
  if (flag == 1 && alarmEnabled == 1) {
    lightAndSound();
    Serial.printf ("Fecha light & sound: %d-%d-%d. Hora: %d:%d:%d\n", day(), month(), year(), hour(), minute(), second());
    Serial.print("flag state "); Serial.println(flag);
  }

  //----------------------  Others ----------------------------------------------------
  currentMillis = millis();
  if (currentMillis - previousMillis >= intervalo) // this prevents the time from being constantly shown.
  { previousMillis = currentMillis;
    Serial.printf ("Fecha: %d-%d-%d. Hora: %d:%d:%d\n", day(), month(), year(), hour(), minute(), second());
    Serial.printf ("Alarma: %d:%d:%d\n", alarmHora, alarmMinuto,  alarmSegundo);
    Serial.print("flag state "); Serial.println(flag);
    if (alarmEnabled == 1) {
      lcd.setCursor(7, 0);
      lcd.print("ON"); lcd.print("     ");
    }
    else {
      lcd.setCursor(7, 0);
      lcd.print("OFF"); lcd.print("    ");
    }
    lcd.setCursor(6, 1);
    lcd.print(hour()); lcd.print(":"); lcd.print(minute()); lcd.print(":"); lcd.print(second()); lcd.print("     ");

  }
  Alarm.delay(0);

}
