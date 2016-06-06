
//==================  Libraries ===============================
#include <Time.h>
#include <Console.h>
//==================== Variables ===============================
//---------------  date & time -------------------------------
int dia;
int mes ;
int anyo;
int hora = 0;
int minuto = 0;
int segundo = 0;
String WiFiSSID = "";
String WAP = "";
String DD;
String MM;
String AAAA;
String HH = "";
String MIN = "";
String SEG = "";
String hh = "";
String mm = "";
//------------  time measurement -------------------------------
unsigned long currentMillis;
unsigned long previousMillis;
unsigned long intervalo = 1000;
//--------------  auxiliary -------------------------------------
char c;
String dataEntry = "";
String input = "";
int primerNumero = 0;
String respuesta = "";
boolean finAjuste = false;
int flag;
String serialData = "";
int k = 0;
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
//=== function to collect data throgh serial monitor:  ==================
void getEntry(String * devol1, int *devol2) {
  String salida = "";
  // Read any serial input
  while (Serial.available() > 0)
  {
    c = (char) Serial.read();
    Serial.print("input caracter "); Serial.println(c);
    input = input + c; // Read in one char at a time
      if (input[input.length() - 1] == '/') {
      salida = input.substring(0, input.length() - 1);
      Serial.print("input de salida "); Serial.println(salida);
      input = "";
      *devol1 = salida;
      *devol2 = salida[0] - '0';
       switch (*devol2) {
        case 1 :
          Serial.print("datos de SSID: "); Serial.println(salida.substring(1, input.length() - 1));
          Serial.flush();
          break;
        case 2 :
          Serial.print("datos de WAP: "); Serial.println(salida.substring(1, input.length() - 1));
          Serial.flush();
          break;
      }
    }
  }
}
//=== function convert strings to nuneric :  ====================
void stringtoNumber(String instruct) {

  if (instruct.substring(0, 1) == "1") {
    WiFiSSID = instruct.substring(1, instruct.length());
    k = 1;

  }
  if (instruct.substring(0, 1) == "2") {
    WAP = instruct.substring(1, instruct.length());
    k = 2;
    }
  if (k == 2) {// Define end of adjustment, setTime and declare Alarms.
    finAjuste = true;
  }

}



//******************  sketch body **************************************/
//===================  setup() ===========================================
void setup() {
  while (!Serial);
  Serial.begin(9600);

  // Open serial port (9600 bauds).
  //----------  Initial screen -----------------------
  printHelp1();
  printHelp2();
}
//=================  main loop() ============================================
void loop() {
  //----------------  Data Entry (date, time settings) ----------------
  while (finAjuste == false )
  { getEntry(&serialData, &primerNumero);
    stringtoNumber(serialData);
  }
  //----------------------  Others ----------------------------------------------------
  currentMillis = millis();
  if (currentMillis - previousMillis >=  intervalo) // this prevents the time from being constantly shown.
  { previousMillis = currentMillis;
    ;

  }

}
