/* 

There are two methods you can use to keep track of the INT pulses. You
can use an interrupt input to monitor the INT signal in the background,
or you can monitor the INT line yourself and use the CLR signal to reset
the LTC4150 for the next pulse.

Connect the following pins to your Arduino:

VIO to VCC
GND to GND
INT to D3
POL to D4
CLR to D6

*/
#define INT 3
#define POL 4
#define CLR 6
// Change the following line to match your battery capacity in mAh.
double BATTERYCAPACITY = 1000.0;
// Change the following line to the percentage of charge your battery has at the start.
double BATTERYINITIALCHARGE = 100.0;

#define LED 13 // Standard Arduino LED
const int button = 7;

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

  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  pinMode(button, INPUT_PULLUP);

  // Enable serial output:

  Serial.begin(9600);
  Serial.println("program started");

  // One INT is this many percent of battery capacity:
  
  percent_quanta = 1.0/(battery_mAh/1000.0*5859.0/100.0);
}

void loop()
{
  static long int time, lasttime;
  double mA;
  boolean polarity;
  int buttonState = digitalRead(button);

  if (digitalRead(INT)==0) // INT has gone low
  {
    // Determine delay since last interrupt (for mA calculation)
    // Note that first interrupt will be incorrect (no previous time!)

    lasttime = time;
    time = micros();  // micros(): returns the number of microseconds since the Arduino board began running the current program.

    // Get the polarity value
    
    polarity = digitalRead(POL);
    if (polarity) // high = charging
    {
      battery_mAh += ah_quanta;
      battery_percent += percent_quanta;
    }
    else // low = discharging
    {
      battery_mAh -= ah_quanta;
      battery_percent -= percent_quanta;
    }

    // Calculate mA from time delay (optional)
  
    mA = 614.4/((time-lasttime)/1000000.0);
  
    // If charging, we'll set mA negative (optional)
    
    if (polarity) mA = mA * -1.0;

    // Clear the interrupt signal

    digitalWrite(CLR,LOW);
    delayMicroseconds(40); // CLR needs to be low > 20us
    digitalWrite(CLR,HIGH);

    // Blink the LED (optional)
    
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);

    // Print out the current battery status

    Serial.print("mAh: ");
    Serial.print(battery_mAh);
    Serial.print(" soc: ");
    Serial.print(battery_percent);
    Serial.print("% time: ");
    Serial.print((time-lasttime)/1000000.0);
    Serial.print("s mA: ");
    Serial.println(mA);
  }
}
