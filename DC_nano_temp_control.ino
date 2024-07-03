//Board nano
// wiring for SSD1306 128x32 0v 5v SCL-to-A5 SDA-to-A4

//stuff for SSD1306 OLED I2C screen
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


int count = 1;
int digout2 = 2; //set digital output pin 2 to pin #2
int button1 = 3; //set digital I/O pin 3 to button input #1
int butt1state = 0; //button 1 state (on/off)
int butt1status = 0; //button 1 status (0 = no press, 1 = short press, 2 = long press)
unsigned long  butt1count = 0; //button 1 on time counter
int butt1laststate = 0; //button 1 last status (0 or 1)
int button2 = 4; //set digital I/O pin 4 to button input #2
int butt2state = 0;  //button 2 state (on/off)
int butt2status = 0; //button 2 status (0 = no press, 1 = short press, 2 = long press)
unsigned long  butt2count = 0; //button 2 on time counter
int butt2laststate = 0; //button 1 last status (0 or 1)

int analog1 = A0; //set analogue input pin
int temp_reading = 0; //set temperature ADC reading variable
int temp_reading1 = 0;
float hist = 0.1;           //histerestis value

int avg_size = 20;      //array length for sample averaging
float avg_array[20];    //array of variable length, must manually adjust array length to match avg_size
int avg_pointer = 0;    //array pointer

float temp = 0; //set temperature variable
float setpoint = 20.0;
String heater_status = "OFF";


//steinhart-hart calculation stuff
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;


// the setup function runs once when you press reset or power the board*****************************************************
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(digout2, OUTPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  digitalWrite(digout2, LOW);
    
  //start serial for SSD1306 OLED
  Serial.begin(9600);
  Serial.println("Reset");
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  
//  testdrawrect();      // Draw rectangles (outlines)
  
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  display.print("Sharky    Thingo&Co.");
  display.display();
  delay(2000); 
  display.clearDisplay();
  display.display();
  display.ssd1306_command(0x81); display.ssd1306_command(1);
//  display.contrast(100);
//  ssd1306_command(0x81);
//  ssd1306_command(100);
}


// the loop function runs over and over again forever*******************************************************************
void loop() {

 
  readtemp();

  displaytemp();
  
  buttonstatus();

  if (butt1status == 1) {
//    Serial.println("butt status 1");
  }
  
  if (butt1status == 2) {
    settemp();
  }

  if (butt2status == 2) {
    displaymillis();
    waitforbutton();
  }
  
  butt1status = 0;  // reset button status
  butt2status = 0;  // reset button status
  
  delay(200);

}



//Subroutines***********************************************************************





// read temperature and average reading subroutine
void readtemp(void) {

  for (avg_pointer = 0; avg_pointer < avg_size; avg_pointer +=1) {  // read temperature sensor and add values to array
    temp_reading1 = analogRead(analog1);
    avg_array[avg_pointer] = temp_reading1;
  }

  temp_reading = 0;
  
  for (int x = 0; x < avg_size; x += 1) {   // add avarage readings together
    temp_reading += avg_array[x];
  }
  
  temp_reading = temp_reading/avg_size;   // avarage radings


//simple linear calculation
//  temp = ((temp_reading-780)/-10.72);  // calculate temp from anlogue reading


//steinhart-hart calculation
  R2 = R1/(1023.0 / (float)temp_reading - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  temp = T;
}


// display temperature subroutine
void displaytemp(void) {
  
  display.setCursor(0, 0);
  display.clearDisplay();
  display.print("Temp  ");
  display.print(temp,1);
  display.setCursor(0, 16);
  display.print("Heater ");
  display.print(heater_status);  



  if (temp > setpoint+hist) {
    digitalWrite(LED_BUILTIN, LOW);   
    digitalWrite(digout2, LOW);    
    heater_status = "OFF";
   } else {
   }
   if (temp < setpoint-hist) { 
    digitalWrite(LED_BUILTIN, HIGH);   
    digitalWrite(digout2, HIGH);
    heater_status = "ON";
   } else {
   }
  display.display(); 
}


// button status subroutine (no press, single press, long press)
void buttonstatus(void) {

  butt1state = digitalRead(button1); //read button 1
  butt2state = digitalRead(button2); //read button 2

  if (butt1laststate != butt1state) { //reset counter if state has changed
    butt1count = millis();
    if (butt1state == 1) {
       butt1status = 1;
    }
  }
  if (butt1state == 0) {             //start long press sub routine if button 1 pressed for 2 seconds
    if ((butt1count + 2000) <= millis()) {
      butt1status = 2;
      butt1count = millis();
    }
  }
  if (butt2laststate != butt2state) { //reset counter if state has changed
    butt2count = millis();
    if (butt2state == 1) {
       butt2status = 1;
    }
  }
  if (butt2state == 0) {             //start long press sub routine if button 2 pressed for 2 seconds
    if ((butt2count + 2000) <= millis()) {
      butt2status = 2;
      butt2count = millis();
    }
  }
//  Serial.println(butt2status);
//  Serial.println("last status");
//  Serial.println(butt1laststate);
//  Serial.println("butt 1 count");
//  Serial.println(butt1count);
  
  butt1laststate = butt1state; 
  butt2laststate = butt2state; 
}

void settemp(void) {

  digitalWrite(digout2, LOW);  //turn off heater
  display.clearDisplay();
  display.display(); 
  waitforbutton();

  while (butt1status != 2) {

    display.setCursor(0, 0);
    display.clearDisplay();
    display.print("Set ");
    display.print(setpoint,1);
    display.setCursor(0, 16);
    display.print("Point");
    display.display(); 
    buttonstatus();
    if (butt1status == 1) {
      setpoint += 0.1;
      butt1status = 0;
    }
    if (butt2status == 1) {
      setpoint -= 0.1;
      butt2status = 0;
    }
  }
  display.clearDisplay();
  display.display(); 
  waitforbutton();
}


void displaymillis(void) {

  digitalWrite(digout2, LOW);  //turn off heater
  display.clearDisplay();
  display.display(); 
  waitforbutton();

  while (butt2status != 2) {

    display.setCursor(0, 0);
    display.clearDisplay();
    display.print("On time ");
    display.setCursor(0, 16);
    display.print(millis());
    display.display(); 
    buttonstatus();
  }
  display.clearDisplay();
  display.display(); 
  waitforbutton();
  
}


void waitforbutton(void) {
  while (butt1state != 1) {
    buttonstatus();
  }
  while (butt2state != 1) {
    buttonstatus();
  }
  butt1status = 0;
  butt2status = 0;
}

//Test subroutine
void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(500);
}
