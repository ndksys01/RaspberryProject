/*
Nguyen Hung
Ver: 1.0
Control servo
change branch
*/

#include <stdio.h>
#include <softPwm.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include <pthread.h>
#include "wiringPi.h"
//#include "softServo.h"
int posHor=3; //default value
int posVer=11; //default value
int icount=0;
int fd ;
char tmpReceive;
PI_THREAD (myThread)
{
while(1)
{
   while (serialDataAvail (fd))
    {
   tmpReceive = serialGetchar(fd);
   printf("%c", tmpReceive);
   if(tmpReceive=='*')
   printf("\n");

   
         
      //fflush (stdout) ;
    }
    delay(3);
}
}




#define   MAX_PINS   1024

// The PWM Frequency is derived from the "pulse time" below. Essentially,
//   the frequency is a function of the range and this pulse time.
//   The total period will be range * pulse time in uS, so a pulse time
//   of 100 and a range of 100 gives a period of 100 * 100 = 10,000 uS
//   which is a frequency of 100Hz.
//
//   It's possible to get a higher frequency by lowering the pulse time,
//   however CPU uage will skyrocket as wiringPi uses a hard-loop to time
//   periods under 100uS - this is because the Linux timer calls are just
//   accurate at all, and have an overhead.
//
//   Another way to increase the frequency is to reduce the range - however
//   that reduces the overall output accuracy...

#define   PULSE_TIME   100

static int marks [MAX_PINS] ;
static int range [MAX_PINS] ;

int newPin = -1 ;
static int i =0;

/*
 * softPwmThread:
 *   Thread to do the actual PWM output
 *********************************************************************************
 */

static PI_THREAD (softPwmThread)
{
  int pin, mark, space ;

  pin    = newPin ;
  newPin = -1 ;

  piHiPri (50) ;

  while(1)
  {
  //if(i<30)

    i++;
    mark  = marks [pin] ;
    space = range [pin] - mark ;

    if (mark != 0)
      {
      if(i<25)
      {
         digitalWrite (pin, HIGH) ;
         }
      else
         {
            digitalWrite (pin, LOW) ;
         }
      }
      delayMicroseconds (mark * 100) ;
   
    if (space != 0)
      digitalWrite (pin, LOW) ;
    delayMicroseconds (space * 100) ;
  }
   digitalWrite (pin, LOW) ;

  return NULL ;
}


/*
 * softPwmWrite:
 *   Write a PWM value to the given pin
 *********************************************************************************
 */

void softPwmWrite (int pin, int value)
{
  pin &= (MAX_PINS - 1) ;

  /**/ if (value < 0)
    value = 0 ;
  else if (value > range [pin])
    value = range [pin] ;

  marks [pin] = value ;
}


/*
 * softPwmCreate:
 *   Create a new PWM thread.
 *********************************************************************************
 */

int softPwmCreate (int pin, int initialValue, int pwmRange)
{
  int res ;

  pinMode      (pin, OUTPUT) ;
  digitalWrite (pin, LOW) ;

  marks [pin] = initialValue ;
  range [pin] = pwmRange ;

  newPin = pin ;
  res = piThreadCreate (softPwmThread) ;

  while (newPin != -1)
    delay (1) ;

  return res ;
}

int main (void)
{
   ///initial UART
     
   if ((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0)
     {
       fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
       return 1 ;
     }
   //inital Thread
   int x = piThreadCreate(myThread);
   if(x!=0)
   {
      printf("No ko the bat dau\n");
   }




   
    if (wiringPiSetup () == -1)
      {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
       return 1 ;
      }
    

    // initial 0 & 1 pin PWM are output 
    pinMode (1, OUTPUT) ; // camera go up/down
    pinMode(0,OUTPUT); //camera turn lerf/right
    
    // pull 2 pin PWM to GND
    digitalWrite (1, LOW) ;
    digitalWrite(0, LOW);
    
    //setup 2 pin is PWM
    softPwmCreate(0,0,200);
    softPwmCreate(1,0,200);
    
    //setup the location defaults 
    i=0;
    softPwmWrite(1,175+posHor);
    //delay(100);
    softPwmWrite(0,175+posVer);
    //delay(100);
    //i=0;
    delay(1000); 

  char posChar;

  while(1)
{
   scanf("%c",&posChar);
   switch(posChar)
   {
      case 'u': //camera go up 
         i=0;
         posHor++;
         if(posHor>20)posHor=20;
         softPwmWrite(1,175+posHor);
         printf("(^_^) Camera is going up.\n");
         //delay(50);
             //i=0;
         break;
      case 'g': //camera go down
         i=0;
         posHor--;
         if(posHor<0)posHor=0;
         softPwmWrite(1,175+posHor);
         printf("(^_^) Camera is going down.\n");
         //delay(50);
             //i=0;
         break;
      case 'l': //camera turn the lerf
         i=0;
         posVer--;
         if(posVer<0)posVer=0;
         softPwmWrite(0,175+posVer);
         printf("(^_^) Camera is turning left.\n");
         //delay(50);
             //i=0;
         break;
      case 'r': //camera turn the right
         i=0;
         posVer++;
         if(posVer>20)posVer=20;
         softPwmWrite(0,175+posVer);
         printf("(^_^) Camera is turning right.\n");
         //delay(50);
             //i=0;
         break;
      case '1':
         //get temperature data
         printf("(^_^) Send temperature data to server.\n");
         serialPuts (fd, "#1") ;
         break;
      case '2':
         // enable warning
         printf("(^_^) Enable security warning.\n");
         serialPuts (fd, "#2") ;
         break;
      case '3':
         // disable warning
         printf("(^_^) Disable security warning.\n");
         serialPuts (fd, "#3") ;
         break;
      case '4':   
         // get status light 1 2 ...
         printf("(^_^) Get status all Lights in your house.\n");
         serialPuts (fd, "#4") ;
         break;
      case '5':
         // turn on/off light 1
         serialPuts (fd, "#5") ;
         break;
      case '6':
         // turn on/off light 2
         serialPuts (fd, "#6") ;
         break;
      case '7':
         // turn on/off light 2
         serialPuts (fd, "#7") ;
         break;
      case '8':
         // turn on/off light 2
         serialPuts (fd, "#8") ;
         break;
      case '9':
         // turn on/off light 2
         serialPuts (fd, "#9") ;
         break;
      case 'a':
         // turn on/off light 2
         serialPuts (fd, "#a") ;
         break;
      case 'b':
         // turn on/off light 2
         serialPuts (fd, "#b") ;
         break;
      case 'c':
         // turn on/off light 2
         serialPuts (fd, "#c") ;
         break;
      case 'd':
         // turn on/off light 2
         serialPuts (fd, "#d") ;
         break;
      case 'e':
         // turn on/off light 2
         serialPuts (fd, "#e") ;
         break;
      case 'e':
         // turn on/off light 2
         serialPuts (fd, "#e") ;
         break;
   

   
   }

} 
   delay(1000);
    return 0 ;
}

