/*
Program by Fernand Hendriks for ZAP assitance droid
In this program all connected sensors ( HC-SR04 and predifined IR distance sensors) whil read-out. Depending on the data output of the sensors this program whil return a 1 or 0 + sensor id.

Depending on serial input the device whill select frontal or rear sensors. This will increase detection rate. The input must be either high or low.
example:
        robot travels forward : when F is received ; robot travels backwards : when R is received; 

*/
// input pin driver ditrection and set save distance value.
int savedistance = 25; //whit this variabel one sets the save distance between sensor and outer objects
char driverdirection = '0';
// sensor names and locations :
const byte IR_sensors[4] = {50,51,52,53};
const byte UR_read_sensors[12] = {22,24,26,28,30,32,34,36,38,40,42,44};//Echo
const byte UR_write_sensors[12] = {23,25,27,29,31,33,35,37,39,41,43,45};//Trig
String UR_sensors_names[12]={"UR1 ","UR2 ","UR3 ","UR4 ","UR5 ","UR6 ","UR7 ","UR8 ","UR9 ","UR10 ","UR11 ","UR12 "};
//String UR_write_sensors_names[12]={"UR1W ","UR2W ","UR3W ","UR4W ","UR5W ","UR6W ","UR7W ","UR8W ","UR9W ","UR10W ","UR11W ","UR12W "};
String IR_sensors_names[4]={"IR1 ","IR2 ","IR3 ","IR4 "};

//other variabels
String value[2]={"1 ","0 "};
String message = String();

int n =0; //teller voor UR sensoren
int q =0; //teller voor IR sensoren

int val_IR =0;
int val_UR=0;

void setup() 
{
   Serial.begin(9600);
  
   for(int i=0; i<12; i++)
   {
       pinMode (UR_read_sensors[i], INPUT);
       pinMode (UR_write_sensors[i], OUTPUT);
       if (i <4)
       {
         pinMode (IR_sensors[i], INPUT);
       }
   }
   pinMode (driverdirection, INPUT);
 }

void loop()
{
  //SCAN OUTPUT UR SENSOR (1 OF 12)
  read_UR_sensor();
  //SCAN OUTPUT IR SENSOR (1 OF 4)
  read_IR_sensor();
 // delay(200);
}

//CHECK UR SENSOR , 1 AT A TIME; 
void read_UR_sensor()
{
  if (driverdirection == 'F')
  {
     if (n == 6) n=0;
     long duration, distance;
     digitalWrite(UR_write_sensors[n], LOW);  // Added this line
     delayMicroseconds(2); // Added this line
     digitalWrite(UR_write_sensors[n], HIGH);
  // delayMicroseconds(1000); - Removed this line
     delayMicroseconds(10); // Added this line
     digitalWrite(UR_write_sensors[n], LOW);
     duration = pulseIn(UR_read_sensors[n], HIGH);
     distance = (duration/2) / 29.1;
     if (distance < savedistance)
     {
       
        message = UR_sensors_names[n] + value[0];
        Serial.println(message);
     }
     else
     {
        message = UR_sensors_names[n] + value[1];
        Serial.println(message);
     }
     Serial.println(distance);
     n++;
  }
  
  if (driverdirection == 'R')
  {
    if (n == 6) n=0;//van 12 naar 1 gebracht voor test
     long duration, distance;
     digitalWrite(UR_write_sensors[n+6], LOW);  // Added this line
     delayMicroseconds(2); // Added this line
     digitalWrite(UR_write_sensors[n+6], HIGH);
  // delayMicroseconds(1000); - Removed this line
     delayMicroseconds(10); // Added this line
     digitalWrite(UR_write_sensors[n+6], LOW);
     duration = pulseIn(UR_read_sensors[n+6], HIGH);
     distance = (duration/2) / 29.1;
     if (distance < savedistance)
     {
       
        message = UR_sensors_names[n+6] + value[0];
        Serial.println(message);
     }
     else
     {
        message = UR_sensors_names[n+6] + value[1];
        Serial.println(message);
     }
     Serial.println(distance);
     n++;
    }
}

//CHECK IF PINS OF IR SENSOR ARE HIGH OR LOW
void read_IR_sensor()
{
  if (q == 4) q=0;
  //Serial.print("read IR sensor ");
  //Serial.println(val_IR);
  val_IR  = digitalRead(IR_sensors[q]);
  //Serial.println(IR_sensors[q]);
  if ( val_IR == 1)
    {
      message = IR_sensors_names[q] + value[0];
      Serial.println(message);
      
    }
  if ( val_IR == 0)
    {
      message = IR_sensors_names[q] + value[1];
      Serial.println(message);
    }
    q++;
  
}


//GET COMMAND KEY FROM SERIAL(FW OR REVERSE MOVEMENT )
void serialEvent()
{
    while(Serial.available())
    {
      driverdirection= Serial.read();
    }
    
}
