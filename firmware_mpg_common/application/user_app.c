/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */

static void  led_display_button(u8 * u8lastdata,u8 *u8new_data);
/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern AntSetupDataType G_stAntSetupData;                         /* From ant.c */

extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentData[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */

static u32 UserApp_u32DataMsgCount = 0;   /* ANT_DATA packets received */
static u32 UserApp_u32TickMsgCount = 0;   /* ANT_TICK packets received */
/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  u8 au8SendMsg[]="Please press buttons";
  u8 au8ReceiveMsg[]="Follow the other";
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR+2,au8ReceiveMsg);
  
  LedOff(RED);
  LedOff(ORANGE);
  LedOff(YELLOW);
  LedOff(GREEN); 
  LedOff(CYAN);
  LedOff(BLUE);
  LedOff(PURPLE);
  LedOff(WHITE);
  
  /* Configure ANT for this application */
  G_stAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  G_stAntSetupData.AntSerialLo         = ANT_SERIAL_LO_USERAPP;
  G_stAntSetupData.AntSerialHi         = ANT_SERIAL_HI_USERAPP;
  G_stAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  G_stAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  G_stAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  G_stAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  G_stAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  G_stAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;
  
   if( AntChannelConfig(ANT_SLAVE) )
  {
    AntOpenChannel();
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    UserApp_StateMachine = UserAppSM_FailedInit;
  }     
  
} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();
} 
/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  u8 au8SendMsg[]="Please press buttons";
  u8 au8ReceiveMsg[]="Follow the other";  
  u8 au8RightMsg[]="You are right!";
  u8 au8FailMsg[]="Game over!"; 
  u8 au8successMsg1[]="We are";
  u8 au8successMsg2[]="the best partner!";      
  static u8 u8flag=0;
  static u32 u32TimeCounter=0;
  static bool judge=FALSE;
  static u8 buttoni=0;
  static u8 j;
  static u8 i;
  static u8 k;
  static u8 m;
  static u8 u8DataCounter=0;
  static u8 kcounter;
  static u8 au8LastAntData[8]={0};
  static u8 Newdata=0;
  static bool u8WinMusic=TRUE;
  static u8 au8CompareMsg[8]={0};
  static u16 u16MusicCounter=0;
  //the slave variable
  static u8 au8TestMessage[8] = {0};
  static u8 au8CurrentMessage[8]= {0};
  static u8 u8Count=0;
 
  static u16 u16delay=0;
  static u16 u16Time=0;
  static u8 u8button_en=0;
 //the master varibale
  
  /*....................receive data and compare with each other...........*/
  if(u8flag==0)
  {
      u32TimeCounter++;
      if(u32TimeCounter>2000)
      {           
         u32TimeCounter=0;         
         LCDCommand(LCD_CLEAR_CMD);
         LCDMessage(LINE1_START_ADDR+2,au8ReceiveMsg);             
      }    
    if(WasButtonPressed(BUTTON0))
    {      
      ButtonAcknowledge(BUTTON0);     
      au8CompareMsg[buttoni]=1;
      buttoni++;
    }
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);    
      au8CompareMsg[buttoni]=2;
      buttoni++;
    }
     if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);   
      au8CompareMsg[buttoni]=3;
      buttoni++;
    }
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);   
      au8CompareMsg[buttoni]=4;
      buttoni++;
    }
   if(u8DataCounter!=0)
   {   
    if(buttoni==u8DataCounter)
    {
          buttoni=0; 
          for(k=0;k<u8DataCounter;k++)
          {
            if(au8CompareMsg[k]==au8LastAntData[k])
            {
               kcounter++;
            }
          }
          if(kcounter==u8DataCounter) 
          {
            judge=TRUE;
          }
        if(judge)
        {  
           u8flag=1;
           LCDCommand(LCD_CLEAR_CMD);
           LCDMessage(LINE2_START_ADDR+2,au8RightMsg); 
     
        }   
         else if(judge==FALSE)
        {
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE2_START_ADDR+4,au8FailMsg);         
          u8flag=2;             
        }        
     } 
    u8DataCounter=0;
   }//end if u8DataCounter
  }//end if u8flag =0  
 
  
  /*..........................send data to the other board........................*/
   if(u8flag==1)
  { 
      u32TimeCounter++;
      if(u32TimeCounter>2000)
      {           
         u32TimeCounter=0;         
         LCDCommand(LCD_CLEAR_CMD);
         LCDMessage(LINE1_START_ADDR,au8SendMsg);             
      }
      u16Time++;
      if(u16delay>200)
      {   
         LedOff(WHITE);
         LedOff(BLUE);
         LedOff(YELLOW);
         LedOff(RED);
      }
      else
      {
        u16delay++;
      }  
      if(WasButtonPressed(BUTTON0))
      {
        ButtonAcknowledge(BUTTON0);
        LedOn(WHITE);
        au8TestMessage[u8Count]=1;
        u8Count++;
        u16delay=0;
        u16Time=0;
        u8button_en=1;
      }
      if(WasButtonPressed(BUTTON1))
      {
        ButtonAcknowledge(BUTTON1);
        LedOn(BLUE);
        au8TestMessage[u8Count]=2;
        u8Count++;
        u16delay=0;
        u16Time=0;
        u8button_en=1;
      }
      if(WasButtonPressed(BUTTON2))
      {
        ButtonAcknowledge(BUTTON2);
        LedOn(YELLOW);
        au8TestMessage[u8Count]=3;
        u8Count++;
        u16delay=0;
        u16Time=0;
        u8button_en=1;
      }
      if(WasButtonPressed(BUTTON3))
      {
        ButtonAcknowledge(BUTTON3);
        LedOn(RED);
        au8TestMessage[u8Count]=4;
        u8Count++;
        u16delay=0;
        u16Time=0;
        u8button_en=1;
      }  
      if(u8Count==8)
      {
        u8Count=0;
      }
  }//end else if u8flag=1
  
  /*.................................press the error button........................................*/
  if(u8flag==2)
  {
   AntCloseChannel();
  }

/*...............................music sounds...........................................*/
  if(u8flag==3)
  {       
      u32TimeCounter++;
      if(u32TimeCounter>100)
      {
        u32TimeCounter=0;
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR+6,au8successMsg1);
        LCDMessage(LINE2_START_ADDR+2,au8successMsg2); 
      }
      if(u8WinMusic)
      { 
         static u8 n=0;
        char music1[100] = "51351353613613637247247777710";
        u16MusicCounter++; 
  
        if((u16MusicCounter-500)%200 == 0 && music1 != '\0')
        {
        
            switch(music1[n])
          {
            case '1': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 533);
                      break;
            case '2': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 587);
                      break;
            case '3': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 659);
                      break;
            case '4': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 677);
                      break;
            case '5': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 784);;
                      break;
            case '6': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 880);
                      break; 
            case '7': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 988);
                      break; 
            case '8': PWMAudioOn(BUZZER1);
                      PWMAudioSetFrequency(BUZZER1, 392);
                      break; 
            case '0': PWMAudioOff(BUZZER1);
                      break;                  
          }
          n++;       
      }
      
     if(u16MusicCounter == 10000)
     {
       PWMAudioOff(BUZZER1);
       u8WinMusic=FALSE;
       u16MusicCounter=480;
       n = 0;
     }  
    }
  }
  
  /*..........................show LED sequence................................*/
  if(Newdata==1&& u8flag==0)
  {    
     led_display_button(au8LastAntData,&Newdata) ;
     
  }
    
  /*the counter of the DATA*/
  for(u8 m=0;m<8;m++)
  {
    if(au8LastAntData[m]!=0)
    {
      u8DataCounter++;
    } 
  }   
  
  /*..............READ the information of the message................. */
  if( AntReadData() )
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    { 
       for(u8 i = 0; i <8; i++)
      {       
        if(G_au8AntApiCurrentData[i] != au8LastAntData[i])
        {
          au8LastAntData[i] = G_au8AntApiCurrentData[i];
          Newdata=1;
          if(au8LastAntData[0]==5)
          {
            u8flag=3;
          }
        }
      }

    }//end if AND_DATA
    else if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      if((u16Time>2000)&&(u8button_en==1))
       { 
           u16Time =0;
           u8button_en=0;
           u8Count=0;
           u8flag=0;
           for(u8 u8count=0;u8count<8;u8count++)
           {
             au8CurrentMessage[u8count]=au8TestMessage[u8count];
             au8TestMessage[u8count]=0;
           }
       }
     AntQueueBroadcastMessage(au8CurrentMessage);      
    }     
  } //end if ANTREAD
} /* end UserAppSM_Idle() */
 



/*-------------------------------------------------------------------------------------------------------------------*/
/*...............................show led sequence........................................*/
static void  led_display_button(u8 *u8lastdata,u8 *u8new_data)
{
  static u32 u32dataCount=0;
  static u8  datai=0;  

  if(u32dataCount==0)
  {   
    if(u8lastdata[datai]==0)
    {      
        if( datai++>6)
        {          
          datai=0;
          *u8new_data=0;
          LedOff(WHITE);
          LedOff(BLUE);
          LedOff(YELLOW);
          LedOff(RED);           
        }
        return;     
    }
    if(u8lastdata[datai]==1)
    {
        LedOn(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);                      
    }
    if(u8lastdata[datai]==2)
    {
          LedOff(WHITE);
          LedOn(BLUE);
          LedOff(YELLOW);
          LedOff(RED); 
    }
    if(u8lastdata[datai]==3)
    {
          LedOff(WHITE);
          LedOff(BLUE);
          LedOn(YELLOW);
          LedOff(RED); 
     }
     if(u8lastdata[datai]==4)
     {
          LedOff(WHITE);
          LedOff(BLUE);
          LedOff(YELLOW);
          LedOn(RED); 
     }

  }//end if u32dataCount==0
  if(u32dataCount++>200)
  {   
    u32dataCount=0;
    if( datai++>7)
    {         
        datai=0;
        *u8new_data=0;
        LedOff(WHITE);
        LedOff(BLUE);
        LedOff(YELLOW);
        LedOff(RED);           
    }  
  }//end delay
 
}//end led_disply_button


static void UserAppSM_Error(void)          
{
  
} 

static void UserAppSM_FailedInit(void)          
{
    
} 


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
