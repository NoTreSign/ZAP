#include <Servo.h> 


//#define myservoa 5
#define  min_pan  2
#define  max_pan 180
#define  min_tilt 60
#define  max_tilt 115



int n =0;
int string_length =0;
String readString;
char a;
char b;
char c;
Servo myservoa, myservob/*, myservoc, myservod*/;  // create servo object to control a servo 

void command_action ( void);
int send_a (int);
int send_b (int );

void setup() {
  Serial.begin(9600);

  //myservoa.writeMicroseconds(1500); //set initial servo position if desired

  //pinMode(myservoa, OUTPUT) ; //the pin for the servoa control
  //digitalWrite(myservoa , LOW);
  myservoa.attach(6);
  myservob.attach(7);  //the pin for the servob control
 // myservoc.attach(8);  //the pin for the servoc control
//  myservod.attach(9);  //the pin for the servod control 
  Serial.println("**************************************\n**\tServo _ Controller \t    **\n**\tBy:  F.J.J.Hendriks  \t    **\n**************************************\n"); // so I can keep track of what is loaded
  myservoa.write(100);
  myservob.write(90);
}

void loop() {

  //expect single strings like 700a, or 1500c, or 2000d,
  //or like 30c, or 90a, or 180d,
  //or combined like 30c,180b,70a,120d,

  if (Serial.available())  {
    char c = Serial.read();  //gets one byte from serial buffer
    if (c == ',') {
      command_action();
    }  
    else {     
      readString += c; //makes the string readString
    }
  }
}

void command_action ( void)
{
  string_length=readString.length();  //get length string
  if (string_length >1) {  //check if string is filled
        
        if (string_length >= 5)
        {
          
          if (readString[string_length-5] == '#' && ( readString[string_length-1] == 'A' || readString[string_length-1] == 'B'|| readString[string_length-1] == 'a' || readString[string_length-1] == 'b'))
          {
            a=readString[string_length-4]; // getting coordinates
            b= readString[string_length-3];
            c =readString[string_length-2];
           
            n=((int)a-48 ) * 100 +((int)b-48) * 10+((int)c-48); //make one number + convert from ascii to int
            Serial.println(n); //print coördinates
         
            Serial.print("writing Angle: ");
            Serial.print(n);
            Serial.print(" to servo : ");
            Serial.println(readString[string_length-1]);
            
           /* if(readString.indexOf('a') >0 && n==1) digitalWrite(myservoa , HIGH);   //extra for testing communications multiple arduino's
            if(readString.indexOf('a') >0 && n==0) digitalWrite(myservoa,LOW);*/ 
            if(readString.indexOf('A') >0 || readString.indexOf('a') >0) 
            {
              n=send_a(n);
              Serial.print("Angle send: ");
              Serial.println(n);
            }
            
            
            if(readString.indexOf('B') >0 || readString.indexOf('b') >0) {
              n=send_b(n);
              Serial.print("Angle send: ");
              Serial.println(n);
              
            }
            /*if(readString.indexOf('c') >0) myservoc.write(n);
            if(readString.indexOf('d') >0) myservod.write(n);*/
          }
          
        }
        Serial.print("command received : ");
        Serial.println(readString); //prints string to serial port out
        readString=""; //clears variable for new input
      }
  
}

int send_a (int Degrees)
{
             
        if (Degrees >= min_tilt && Degrees <= max_tilt)
        {
           myservoa.write(Degrees);
        }
        else if (Degrees < min_tilt)
        {
	    Degrees = min_tilt;
            myservoa.write(Degrees);
        }
	else if (Degrees > max_tilt)
        {
	    Degrees = max_tilt;
            myservoa.write(Degrees);
        } 
        return Degrees;
}

int send_b (int Degrees)
{

           
        if (Degrees >= min_pan && Degrees <= max_pan)
        {
            myservob.write(Degrees);           
         }
        else if ( Degrees < min_pan) // als tilt waarden lager is dan mogelijk is beweeg camera omlaag
        {
	    Degrees = min_pan;
            myservob.write(Degrees);
        }
	else if ( Degrees > max_pan)// als tilt waarden hoger is dan mogelijk is beweeg camera omhoog
        {
	    Degrees = max_pan;
            Serial.println("moving B");
        }
        return Degrees;
}


