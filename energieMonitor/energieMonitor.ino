/* 
Connect the following pins to your seeeduino xiao:

VIO to 3V3
GND to GND
INT to D3
POL to D4
CLR to D6
button to D8 and GND
display:
SDA to A4
SCL to A5
GND to GND
VCC to 5V
*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define INT 3
#define POL 4
#define CLR 6

LiquidCrystal_I2C lcd(0x20,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display

// Change the following line to match your battery capacity in mAh.
double BATTERYCAPACITY = 1000.0;
// Change the following line to the percentage of charge your battery has at the start.
double BATTERYINITIALCHARGE = 100.0;

const int button = 8;
double battery_mAh = BATTERYCAPACITY; // milliamp-hours (mAh)
double battery_percent = BATTERYINITIALCHARGE;  // state-of-charge (percent)

// Global variables:
double ah_quanta = 0.17067759; // mAh for each INT
double percent_quanta; // calculated below

void setup()
{
  // Set up I/O pins:
  pinMode(INT,INPUT);
  pinMode(POL,INPUT);
  pinMode(CLR,OUTPUT);
  digitalWrite(CLR,HIGH);
  pinMode(button, INPUT_PULLUP);

  lcd.init();                      // initialize the lcd 

  // Enable serial output:
  Serial.begin(9600);
  Serial.println("program started");

  // One interupt is this many percent of battery capacity: 
  percent_quanta = 1.0/(battery_mAh/1000.0*5859.0/100.0);
}


void loop()
{
  int buttonState = digitalRead(button);
  while (buttonState == 0){
    static long int time = 0, lasttime;
    double current;
    boolean polarity;
    
  
    if (buttonState)
    {
      resetCapacity();
      break;
    }
  
    if (digitalRead(INT)==0)            // INT has gone low
    {
      // Determine delay since last interrupt (for mA calculation)
      // Note that first interrupt will be incorrect (no previous time!)
      lasttime = time;
      time = micros();  // micros(): returns the number of microseconds since the Arduino board began running the current program.
  
      polarity = digitalRead(POL);      // Get the polarity value
      if (polarity)                     // high = charging
      {
        battery_mAh += ah_quanta;
        battery_percent += percent_quanta;
      }
      else                              // low = discharging
      {
        battery_mAh -= ah_quanta;
        battery_percent -= percent_quanta;
      }
  
      current = calculateCurrent(time, lasttime);
    
      // If charging, we'll set mA negative (optional)    
      if (polarity) current = current * -1.0;
  
      clearInterupt();
  
      printStatus(battery_percent, current);
      // Print a message to the LCD.
      lcd.backlight();
      String batPerc = String(battery_percent);
      String percentage = "% ";
      String curr = String(current);
      String miliA = " mA";
      String displayString = batPerc + percentage + curr + miliA;
      lcd.print(displayString);
    }
  }
}


void resetCapacity()
{
  double battery_mAh = BATTERYCAPACITY; // milliamp-hours (mAh)
  double battery_percent = 100.0;  // state-of-charge (percent)
}


double calculateCurrent(long int time, long int lasttime)   // Calculate current from time delay
{
  double current = 614.4/((time-lasttime)/1000000.0);
  return current;
}


void clearInterupt()
{
  digitalWrite(CLR,LOW);
  delayMicroseconds(40); // CLR needs to be low > 20us
  digitalWrite(CLR,HIGH);
}


void printStatus(double battery_perc, double current)  // Print out the current battery status
{
  Serial.print(battery_perc);
  Serial.print("% current: ");
  Serial.print(current);
  Serial.println("mA: ");
  
}
