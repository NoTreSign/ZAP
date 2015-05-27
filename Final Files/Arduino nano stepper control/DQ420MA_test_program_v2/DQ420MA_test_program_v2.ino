/*
*	Created by: Fernand Hendriks	Project: ZAP robot		Project type: Robotics	year: 2014-2015
*	This program is the control software for the DQ420MA and the FaceTracker of the ZAP robot.
*	The program fucntions in combination with a Arduino Nano. Pins numbers displayed are the DIRECTION
*	D pins. 
*/
/*
*  Hardcoded pins
*/
#define ENABLE 7
#define DIRECTION 6
#define PULSE 5
#define input_microswitch 8
#define led 9
/*
*  Hardcoded variabels
*/
#define max_steps_stepper  35

/*
*  variabels
*/
char sb = ' ';
boolean input_high = false;
boolean status_enable = false;
boolean status_direction = false;
//boolean status_pulse = false;
int current_hight =0; // variabel that is used for knowing the coorinates of the camera in hight(steps).
int n=0;
/*
*  Functions
*/
int is_legal_command( char );
void pulse (void);
void reset (void);
void check_hight (void);
void To_Middle_Position(void);


void setup()
{
  //make seruak communication with pc possible
  Serial.begin(9600);
  
  // intro message
  Serial.print( "\t****************************************************\n\n\n\t  ... HELLO WELCOME BY DQ420MA TEST PROGRAM ...\n\t\t... WRITTEN BY: F.J.J. HENDRIKS  ...\n\n\t****************************************************\n" );
  
  //define pins
  pinMode(ENABLE, OUTPUT);
  pinMode(DIRECTION, OUTPUT);
  pinMode(PULSE, OUTPUT);
  pinMode(input_microswitch,INPUT);
  pinMode(led,OUTPUT);
  //for safety make evrything low
  digitalWrite(led,LOW);
  digitalWrite(ENABLE, LOW);
  boolean status_enable = false;
  digitalWrite(DIRECTION, LOW);
  boolean status_direction = false;
  digitalWrite(PULSE, LOW);
  //status_pulse = false;
  
  //set camera to middel and get coordinates for coordinated movement.
  reset();
  
  
}


int input_value =0;

void loop()
{
  
  input_value = digitalRead(input_microswitch);
  
  if (input_value == 1 && input_high == false)
         {
           Serial.println("pin is high");
           input_high=true;
           digitalWrite(led,LOW);
          
         }
  if (input_value == 0 && input_high == true)
         {
           Serial.println("pin is low");
           input_high=false;
	   digitalWrite(led,HIGH);//gives activation visbile
           
         }
  
  if (Serial.available()) 
  {
           
           sb = Serial.read();
           if ( is_legal_command( sb ) )
           {
				 if ( sb == 'A' || sb == 'a') //ENBALE
				 {
					 Serial.print( "ENABLE\n" );
					 digitalWrite( ENABLE, HIGH );
                                         status_enable=true;
				 }  
				 if  ( sb == 'B' || sb == 'b' )//DISABLE
				 {
					Serial.print( "DISABLE\n" );
					digitalWrite( ENABLE, LOW );
					status_enable=false; 
				 }
				 if ( sb == 'C' || sb == 'c') //DIRECTION RIGHT
				 {
					 Serial.print( "DIRECTION RIGHT\n" );
					 digitalWrite( DIRECTION, HIGH );
                                         status_direction= true;

				 }   
				 if  ( sb == 'D' || sb == 'd')//DIRECTION LEFT
				 {
					Serial.print( "DIRECTION LEFT\n" );
					digitalWrite( DIRECTION, LOW );
                                        status_direction= false;
					
				 }
				 if ( sb == 'E' || sb == 'e' ) //PULSE POSITIVE
				 {
					 Serial.print( "PULSE POSITIVE\n" );
					 digitalWrite( PULSE, HIGH );
					
				 }   
				 if  ( sb == 'F' || sb == 'f' ) //PULSE NEGATIVE
				 {
					Serial.print( "PULSE NEGATIVE\n" );
					digitalWrite( PULSE, LOW );
					
				 }
				 if ( sb == 'P' || sb == 'p' ) //PULSEs
				 {
					check_hight();
				 }
				 if ( sb == 'R' || sb == 'r' ) //PULSE NEGATIVE
				 {
				   reset();
					
				 }
                                 if ( sb == 'M' || sb == 'm' ) //PULSEs
				 {
					To_Middle_Position();
				 }
				 
           }
         Serial.print(sb);
         
        
  }
     

  
  
}


int is_legal_command(char sb)
{
   if ((sb >= 'A') && (sb <= 'F') || (sb >= 'a') && (sb <= 'f') || (sb == 'P') || (sb == 'p')|| (sb == 'R') || (sb == 'r')|| (sb == 'M') || (sb == 'm') )
   {
       return(true);
   }
   else
   {
       return(false);
   }
}



void pulse (void)
{
    Serial.print( "PULSE\n" );
    digitalWrite( PULSE, LOW );
    for (int i=0;i<200;i++)
    {
  
      digitalWrite( PULSE, HIGH );
      delay(2);
      digitalWrite( PULSE, LOW );
    }
    digitalWrite( PULSE, LOW );
    return;
}

void check_hight (void)
{
	if (current_hight >=0 && current_hight <=max_steps_stepper)
	{
                Serial.print("hight is correct: ");
		if (current_hight==0 && status_direction== true)
		{
                        Serial.println("option1");
			pulse();
			current_hight++;
		}
		
		else if (current_hight==max_steps_stepper && status_direction== false)
		{
                        Serial.println("option2");
			pulse();
			current_hight--;
		}
		else if ( current_hight >0 && current_hight <max_steps_stepper)
		{
                        Serial.println("option3");
			pulse();
			if (status_direction== true) current_hight++;
			else current_hight--;
		}
                Serial.println(current_hight);
	}
	return;
}

void reset (void)
{
	  Serial.println("resetting the hight of the camera");
	  /*
	  *  find lowest point frame
	  */
	  
	  while (   digitalRead( input_microswitch)  )
	  {
		  Serial.print("moving one step down, steps taken ");
		  Serial.println(n);
		  if (status_enable==false )
		  {
			 digitalWrite(ENABLE,HIGH);
			 status_enable= true;
		  }
		  if (status_direction== true)
		  {
			digitalWrite(DIRECTION,LOW);
			status_direction= false;
		  }
		  pulse();
                  n++;
	  }
          n=0;
	  Serial.println("arrived at lowest point");
	  /* 
	  *    Bring unit to the middel of the frame
	  */
	  for (current_hight=0;current_hight<max_steps_stepper/2;current_hight++)
	  {
		  Serial.print("moving steps up to centre, step to be taken ");
		  Serial.println((max_steps_stepper/2)-current_hight);
		  if (status_enable==false)   
		   {
			 digitalWrite(ENABLE,HIGH);
			 status_enable= true;
		  }
		  if (status_direction== false)
		  {
			digitalWrite(DIRECTION,HIGH);
			status_direction= true;
		  }
		  
		  pulse();
	  }
          pulse(); //one last pulse in order to not get to the bottom --> same has been done with top point
	  Serial.println("arrived at centre point");
          digitalWrite( PULSE, LOW );
	  return;
  
}

//max_steps_stepper == 35 / 2 == 17.5->18
void To_Middle_Position (void)
{
  while (current_hight!=18)
  {
    if (current_hight > 18)
    {
      Serial.print( "DIRECTION LEFT\n" );
      digitalWrite( DIRECTION, LOW );
      status_direction= false;
      pulse();
      current_hight--;
    }
    else if (current_hight <18)
    {
      Serial.print( "DIRECTION RIGHT\n" );
      digitalWrite( DIRECTION, HIGH );
      status_direction= true;
      pulse();
      current_hight++;
    }
  }
  return;
  
  
  
}

