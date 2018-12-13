
#include <Wire.h>              //needed for I2C communication
#include <LiquidCrystal_I2C.h> //Library used for LCD screen
#include <SparkFun_RHT03.h>    //Library for Humidity sensor
#include <SoftwareSerial.h>    //Library for bluetooth module
#include <RunningAverage.h>

//SDA A4
//SCL A5

int UVOUT = A0;   //output of UV
int REF_3V3 = A1; //3.3V power on arduino 

const int bluetoothTx = 2;
const int bluetoothRx = 3;
const int TMP_PIN = A2;
const int RHT03_DATA_PIN = 4;  //Pin to read RHT03 digital output signal
RHT03 rht;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

unsigned long time_since_last_reset = 0;
int interval = 3000;

RunningAverage myRA(20);

float floathmdty = 14.3;

byte degree[8] = {
  B00100,
  B01010,
  B10001,
  B01010,
  B00100,
  B00000,
  B00000,
};

void setup() {
  Serial.begin(9600);

  pinMode(UVOUT, INPUT);
  pinMode(REF_3V3, INPUT);
  
  lcd.begin(16,2);
  lcd.clear();
  
  pinMode(A1,INPUT);
  
  rht.begin(RHT03_DATA_PIN);    //initializes sensor and data pin 

  bluetooth.begin(115200);
  bluetooth.print("$$$");
  delay(100);
  bluetooth.println("U,9600,N");
  bluetooth.begin(9600);

  myRA.clear();
 
}

void loop() {
  lcd.createChar (0,degree);
  int updateRet = rht.update(); //gets new humidity value from sensor

  time_since_last_reset = millis();
  while((millis() - time_since_last_reset)<interval) {
    
  if (updateRet == 1)          //If update successful, update() returns 1, else it returns <0
   {
    float latestHumidity = rht.humidity();//gets the humidity value
    myRA.addValue(latestHumidity);
   }
  }
  
  float hmdty = myRA.getAverage();  

  if(isnan(hmdty)){
    hmdty = floathmdty;
  }

  else{
    floathmdty = hmdty;
  }

  int uvLevel = averageAnalogRead(UVOUT);
  int refLevel = averageAnalogRead(REF_3V3);

  float outputVoltage = 3.3/refLevel * uvLevel;

  float uvIntensity = mapfloat(outputVoltage, .99, 2.8, 0.0, 15.0);

 // Serial.println(uvIntensity);

  //Serial.println(hmdty);
  bluetooth.println();

  myRA.clear();
  //Serial.println("Blue " + String((float)bluetooth.read()));
  
  int reading = analogRead(TMP_PIN);
  float voltage = reading * 5.0;
  //Serial.println(reading);
  voltage /= 1024.0;
  //Serial.println("Voltage: " + String(voltage));
  float temperatureC = (voltage - 0.5) * 100;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  //bluetooth.print("Temperature: " +String(temperatureF,1)+" Fahrenheit      ");
 
  //bluetooth.print("Humidity: " + String(hmdty) + "%     ");
  //int tempTest = (int)temperatureF;
  //int hmdtyTest= (int)hmdty;
  //int uvIntensityTest = (int)uvIntensity;
  //Serial.println(tempTest);
  //bluetooth.println(String(tempTest));
 // bluetooth.println("UV intensity: "+String(uvIntensity)+"mW/cm^2     ");
  
  //Serial.println("TMP36: "+String(temperatureF,1));
  //Serial.println(tempTest);
  //bluetooth.println(tempTest);
  //char text(40);
  bluetooth.println(String(temperatureF)+","+String(hmdty)+","+String(uvIntensity));
  //sprintf(text, "%d,%d,%d\n",tempTest,hmdtyTest,uvIntensityTest);
  //Serial.print(text);
  //bluetooth.println(text);
  
  lcd.setCursor(0,0);
  lcd.print("Temperature:");
  lcd.setCursor(0,1);
  lcd.print(temperatureF);
  lcd.write(byte(0));
  lcd.print("F");
  delay(2000);
  lcd.clear();

  
  lcd.setCursor(0,0);
  lcd.print("UV Intensity:");
  lcd.setCursor(0,1);
  lcd.print(uvIntensity);
  lcd.print("mW/cm^2");
  delay(2000);
  lcd.clear();
  
  
  lcd.setCursor(0,0);
  lcd.print("Humidity:");
  lcd.setCursor(0,1);
  lcd.print(String(hmdty));
  lcd.print("%");
  delay(2000);
  lcd.clear();
  }

  int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
  runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
 
