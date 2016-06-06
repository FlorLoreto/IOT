

/*
  Time Check

  Gets the date and time from Linux via Bridge then parses out hours,
  minutes and seconds for the Arduino using an Arduino Yún.
  It can be used to synchronise Arduino clock with the Linino side.
  If not, the clock - on Arduino -  will have a different time than the Linino side.
*/


#include <Process.h>
#include <Time.h>
#include <TimeAlarms.h>

Process date;                 // process used to get the date
int hours, minutes, seconds; 
int months, days, years;// for the results
int lastSecond = -1;          // need an impossible value for comparison

void setup() {
  Bridge.begin();        // initialize Bridge
  Serial.begin(9600);    // initializeSerial

  while (!Serial);              // wait forSerial Monitor to open
  Serial.println("Time Check");  // Title of sketch

  // run an initial date process. Should return:
  // hh:mm:ss :
  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%H:%M:%S %m/%d/%y%n"); 
   date.run();
  }
}

void loop() {

  if (lastSecond != seconds) { // if a second has passed
    // print the time:
    if (hours <= 9) {
      Serial.print("0");  // adjust for 0-9
    }
    Serial.print(hours);
    Serial.print(":");
    if (minutes <= 9) {
      Serial.print("0");  // adjust for 0-9
    }
    Serial.print(minutes);
    Serial.print(":");
    if (seconds <= 9) {
      Serial.print("0");  // adjust for 0-9
    }
    Serial.println(seconds);
    Serial.print(months);Serial.print("/");
    Serial.print(days);Serial.print("/");
    Serial.println(years);
    Serial.printf("la hora despues de setTime :%d H:%d M:%d S\n",hour(),minute(),second());
    // restart the date process:
    if (!date.running()) {
      date.begin("date");
      date.addParameter("+%H:%M:%S %m/%d/%y%n"); 
       date.run();
    }
  }

  //if there's a result from the date process, parse it:
  while (date.available() > 0) {
    // get the result of the date process (should be hh:mm:ss):
    String todoString = date.readString();
    String timeString = todoString.substring(0,8);
     String dateString = todoString.substring(9);
    // find the colons:
    int firstColon =timeString.indexOf(":");
    int secondColon = timeString.lastIndexOf(":");
    int firstColonDate =dateString.indexOf("/");
    int secondColonDate = dateString.lastIndexOf("/");

    // get the substrings for hour, minute second:
    String hourString = timeString.substring(0, firstColon);
    String minString = timeString.substring(firstColon + 1, secondColon);
    String secString = timeString.substring(secondColon + 1);
    String monString = dateString.substring(0, firstColonDate);
    String dayString = dateString.substring(firstColonDate + 1, secondColonDate);
    String yearString = dateString.substring(secondColonDate + 1);

    // convert to ints,saving the previous second:
    hours = hourString.toInt();
    minutes = minString.toInt();
    lastSecond = seconds;          // save to do a time comparison
    seconds = secString.toInt();
    months=monString.toInt();
    days=dayString.toInt();
    years=yearString.toInt();
  }
setTime(hours,minutes,seconds,days,months,years);

}
