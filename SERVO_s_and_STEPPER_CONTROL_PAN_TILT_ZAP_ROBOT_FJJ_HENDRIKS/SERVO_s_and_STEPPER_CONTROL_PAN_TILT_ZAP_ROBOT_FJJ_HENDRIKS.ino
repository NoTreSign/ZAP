
/*
Serial Data protocol for recieving start, command, degrees, endbit for servo controll
start = # , command = A or B , degrees = 0 >= <= 255 , endbit = / 
by using if not true  if structures whe sepperate incoming data.
incoming data may not contain newline between start - command - degrees -endbit !!! 
newline add end is required!!!

SERVO'S NEED 5 VS, STEPPER NEEDS 12VS

*/
#include <Servo.h>
//waardes voor servo's //LET OP PAN EN TILT ZIJN VERWISSELD
#define  min_tilt  15//min_pan
#define  max_tilt 155//max_pan
#define  min_pan 2//min_tilt
#define  max_pan 180//max_tilt
//poorten voor hbrug stepper
#define Control 2  // for ig=f want control by switch
#define EnA 9  // In1
#define EnB 10  // In2
#define phA 11  // In3
#define phB 12  // In4
#define led 3
//------------------------------servo variabelen
//servo id's
char tiltchannel='A', panchannel = 'B';
//Add servo's
Servo ServoTilt,ServoPan;
unsigned char rxBuff[100] = {0}; //extr
//protocol stuff
const byte startByte = '#';
const byte endByte = '/';
boolean dataReceived = false;         // a string to hold incoming data
boolean receiveComplete = false;  // whether the string is complete
boolean gotStart = false;
boolean gotCommand = false;
boolean gotEnd  = false;
boolean gotMessage = false;
boolean wrongInput = false;
//inputs
char Command = '0';
unsigned long Degrees = 0;
int n = 0;
//---------------------------------hbrug variablen
int num = 50;
int Tdelay = 5;//minimum time delay for steps!
//---------------------------------functies
//functions
void receiveHandler(void);
void Down(int , int );
void Up(int , int );

void setup()
{
    Serial.begin(9600);
    Serial.println( "Serial USB to Servo opperational :");
    Serial1.begin(9600);
    Serial1.write( "Serial BT to Servo opperational :");
    pinMode(EnA, OUTPUT);
    pinMode(EnB, OUTPUT);
    pinMode(phA, OUTPUT);
    pinMode(phB, OUTPUT);
    pinMode(led, OUTPUT);
    ServoTilt.attach(2);
    ServoPan.attach(3);
    ServoTilt.write(90);
    ServoPan.write(90);
}

void loop()
{
    if(dataReceived)
    {
      receiveHandler();
      dataReceived = false;  
    }
    
    if(wrongInput)
    {
       n++;
       Serial.print("number of time's recieving went wrong: ");
       Serial.println(n);
       wrongInput = false;  
    }
 
}


//carry out if data is received
void receiveHandler()
{
   if (Command == tiltchannel)
    {
        if (Degrees >= min_tilt && Degrees <= max_tilt)
        {
            
            Serial.println("moving A");
            ServoPan.write(Degrees);
            Command ='0';
            Degrees = 0;
        }
        else if (Degrees < min_tilt)
        {
	    Degrees = min_tilt;
            Down(num, Tdelay);
            Serial.println("moving A");
            ServoPan.write(Degrees);
            Command ='0';
            Degrees = 0;
        }
	else if (Degrees > max_tilt)
        {
	    Degrees = max_tilt;
            Up(num, Tdelay);
            Serial.println("moving A");
            ServoPan.write(Degrees);
            Command ='0';
            Degrees = 0;
        } 
    }
    if (Command == panchannel)
    {
      if (Degrees >= min_pan && Degrees <= max_pan)
        {
            Serial.println("moving B");
            ServoTilt.write(Degrees);
            Command ='0';
            Degrees = 0;
        }
        else if ( Degrees < min_pan) // als tilt waarden lager is dan mogelijk is beweeg camera omlaag
        {
	    Degrees = min_pan;
            
            Serial.println("moving B");
            ServoTilt.write(Degrees);
            Command ='0';
            Degrees = 0;
        }
		else if ( Degrees > max_pan)// als tilt waarden hoger is dan mogelijk is beweeg camera omhoog
        {
	    Degrees = max_pan;
            
            Serial.println("moving B");
            ServoTilt.write(Degrees);
            Command ='0';
            Degrees = 0;
        }
    }
    return;
}

void Down(int i, int j) {

  // Pin 8 Enable A Pin 13 Enable B on
 
  j = j + 10;
  while (1)   {
    digitalWrite(EnB, HIGH);
    digitalWrite(EnA, LOW);
    digitalWrite(phA, HIGH);
    delay(j);  
    i--;
    if (i < 1) break;

    digitalWrite(EnA, HIGH);
    digitalWrite(EnB, LOW);
    digitalWrite(phB, HIGH);
    delay(j);
    i--;
    if (i < 1) break;

    digitalWrite(EnB, HIGH);
    digitalWrite(EnA, LOW);
    digitalWrite(phA, LOW);
    delay(j);  
    i--;
    if (i < 1) break;

    digitalWrite(EnA, HIGH);
    digitalWrite(EnB, LOW);
    digitalWrite(phB, LOW);
    delay(j);
    i--;
    if (i < 1) break;
  }

  // all outputs to stepper off
 digitalWrite(EnA, HIGH);
 digitalWrite(EnB, HIGH);

}  // end reverse()

void Up(int i, int j) {

  // Pin 8 Enable A Pin 13 Enable B on
  
  j = j + 10;
  while (1)   {

    digitalWrite(EnA, HIGH);
    digitalWrite(EnB, LOW);
    digitalWrite(phB, LOW);
    delay(j);
    i--;
    if (i < 1) break; 

    digitalWrite(EnB, HIGH);
    digitalWrite(EnA, LOW);
    digitalWrite(phA, LOW);
    delay(j);  
    i--;
    if (i < 1) break;

    digitalWrite(EnA, HIGH);
    digitalWrite(EnB, LOW);
    digitalWrite(phB, HIGH);
    delay(j);
    i--;
    if (i < 1) break;

    digitalWrite(EnB, HIGH);
    digitalWrite(EnA, LOW);
    digitalWrite(phA, HIGH);
    delay(j);  
    i--;
    if (i < 1) break;


  }

  // all outputs to stepper off
 digitalWrite(EnA, HIGH);
 digitalWrite(EnB, HIGH);

}  // end forward()

void serialEvent()
{
    while(Serial.available())
    {
        unsigned char newData = (unsigned char)Serial.read();
        if(!dataReceived || !wrongInput)
        {
            if(!gotStart)
            {
                if(newData==startByte)
                {
                  gotStart=true;
                }  
            } 
            else
            {
                if (!gotCommand)
                {
                     if(newData==panchannel||tiltchannel)
                     {
                       Command = (char)newData;
                       gotCommand = true;
                     }
                     else
                     {
                       Serial.println("command doesnt exist");
                       newData = 0;
                       gotStart = false;
                       wrongInput = true;
                     }
                }
                else
                {
                  
                    if (!gotEnd)
                    {
                         if (newData >= '0' && newData <= '9')
                          { 
                            Degrees = Degrees * 10 + newData - '0';
                          }
                          else
                          {
                                if (newData == endByte)
                                {
                                     //gotEnd = true;
                                     newData = 0;
                                     //gotEnd = false;
                                     gotStart = false;
                                     gotCommand = false;
                                     dataReceived = true;
                                     Serial.println("Received Data :)");
                                }
                                else
                                {
                                    Serial.println("no Degree / endBit input");
                                    newData = 0;
                                    Degrees = 0;
                                    gotStart = false;
                                    gotCommand = false;
                                    wrongInput = true;
                                }
                          } 
                    }        
                }
            }
        }
    }  
}

void serialEvent1()
{
    while(Serial1.available())
    {
        unsigned char newserData = (unsigned char)Serial1.read();
        Serial1.write("received :");
        Serial1.write( newserData );
        if (newserData == '255' && !gotStart || newserData == startByte && !gotStart )
        {
          gotStart=true;
        }
        else 
        {   if (newserData == '0' && gotStart && !gotCommand || newserData == panchannel && gotStart && !gotCommand)
            {
              Command = panchannel;
              gotCommand = true;
            }
            else if (newserData == '1'&& gotStart && !gotCommand || newserData == tiltchannel && gotStart && !gotCommand)
            {
              Command = tiltchannel;
              gotCommand = true;
            }
            else
            {
               if (newserData >= '0' && newserData <= '9')
                          { 
                            Degrees = Degrees * 10 + newserData - '0';
                          }
              else
              {
                    if (newserData == endByte)
                                {
                                     //gotEnd = true;
                                     newserData = 0;
                                     //gotEnd = false;
                                     gotStart = false;
                                     gotCommand = false;
                                     dataReceived = true;
                                     Serial.println("Received Data :)");
                                }
                                else
                                {
                                    Serial.println("no Degree / endBit input");
                                    newserData = 0;
                                    Degrees = 0;
                                    gotStart = false;
                                    gotCommand = false;
                                    wrongInput = true;
                                }
              }
            }      
          }
    }
}
