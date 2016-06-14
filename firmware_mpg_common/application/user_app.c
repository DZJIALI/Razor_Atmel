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
  static u8 u8flag=0;
  static u32 u32TimeCounter=0;
  static bool judge=FALSE;
  
  static u8 au8CompareMsg[10]={0};
  if(u8flag==0)//receive data and compare
  {
    if(WasButtonPressed(BUTTON0))
    {      
      ButtonAcknowledge(BUTTON0);
      au8CompareMsg[0]=1;
      judge=TRUE;
    }
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      au8CompareMsg[0]=2;
      judge=TRUE;
    }
     if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      au8CompareMsg[0]=3;
      judge=TRUE;
    }
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      au8CompareMsg[0]=4;
      judge=TRUE;
    }
  }
  else if(u8flag==1)//send data
  {
    
  }

    
  if( AntReadData() )
  {
      if(G_eAntApiCurrentMessageClass == ANT_DATA)
      {             
          if(G_au8AntApiCurrentData[0]==1)
        {
          LedOn(WHITE);
          LedOff(BLUE);
          LedOff(YELLOW);
          LedOff(RED);       
        }

        if(G_au8AntApiCurrentData[0]==2)
        {
          LedOn(BLUE);
          LedOff(WHITE);
          LedOff(YELLOW);
          LedOff(RED);           
        }

        if(G_au8AntApiCurrentData[0]==3)
        {
          LedOn(YELLOW);
          LedOff(BLUE);
          LedOff(WHITE);
          LedOff(RED);          
        }

        if(G_au8AntApiCurrentData[0]==4)
        {
          LedOn(RED);
          LedOff(BLUE);
          LedOff(YELLOW);
          LedOff(WHITE);                   
        }

      if(judge)
     {  
           u32TimeCounter++;      
        //compare
        if(G_au8AntApiCurrentData[0]==au8CompareMsg[0])
        {   
          u8flag=1;
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR+2,au8RightMsg); 
        }
        else
        {          
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR+2,au8FailMsg);
          AntCloseChannel();
        }
       
         if(u32TimeCounter==12 )
        {           
            u32TimeCounter=0;
            judge=FALSE;
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR,au8SendMsg);             
         } 
       }//end of judge    
     }
      else if(G_eAntApiCurrentMessageClass == ANT_TICK)
     {
        
     }     
  } //end if ANTREAD
              
 

} /* end UserAppSM_Idle() */
 



/*-------------------------------------------------------------------------------------------------------------------*/





static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
