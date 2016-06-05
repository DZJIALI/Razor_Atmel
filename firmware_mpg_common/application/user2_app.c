/**********************************************************************************************************************
File: user2_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user2_app as a template:
 1. Copy both user2_app.c and user2_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user2_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "User2App" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER2_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user2_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void User2AppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void User2AppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
                    /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern volatile u32 G_u32UserAppFlags;       
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "User2App_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type User2App_StateMachine;            /* The state machine function pointer */
static u32 User2App_u32Timeout;                      /* Timeout counter used across states */


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
Function: User2AppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void User2AppInitialize(void)
{
  LedOff(RED);
  LedOff(ORANGE);
  LedOff(YELLOW);
  LedOff(GREEN);
  LedOff(CYAN);
  LedOff(BLUE);
  LedOff(PURPLE);
  LedOff(WHITE);  
  PWMAudioSetFrequency(BUZZER1, 500);
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    User2App_StateMachine = User2AppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    User2App_StateMachine = User2AppSM_FailedInit;
  }

} /* end User2AppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function User2AppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void User2AppRunActiveState(void)
{
  User2App_StateMachine();

} /* end User2AppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void User2AppSM_Idle(void)
{
  static u8 u8counter=0x01;//Led
  static u16 u16count=60;
  static u8 u8flag=0;
  
  static u16 u16Counter=0;//Buzzer
  static u8 u8i=0;
  static u16 u16frequency[]={330,294,262,294,330,330,330,294,294,294,330,392,392,330,294,262,294,330,330,330,330,294,294,330,294,262};
  
  static u16 u16timecounter=0;
      
  if(G_u32UserAppFlags==1)
  {
      u16timecounter++; 
    
    u16Counter++;//Buzzer
    PWMAudioSetFrequency(BUZZER1,u16frequency[u8i]);
    PWMAudioOn(BUZZER1);
    if(u16Counter==500)
    {
        u8i++;
        if(u8i==26)
        {
          u8i=0;
        }       
        u16Counter=0;
    }
    //LED
      u16count--;
  if(u16count==0)
  {
    u16count=60;
    if(u8counter&0x01)
    {
      LedOn(RED);
    }
    else
    {
      LedOff(RED);
    }
    if(u8counter&0x02)
    {
      LedOn(ORANGE);
    }
    else
    {
      LedOff(ORANGE);
    }
    if(u8counter&0x04)
    {
      LedOn(YELLOW);
    }
    else
    {
      LedOff(YELLOW);
    }
    if(u8counter&0x08)
    {
      LedOn(GREEN);
    }
    else
    {
      LedOff(GREEN);
    }
    if(u8counter&0x10)
    {
      LedOn(CYAN);
    }
    else
    {
      LedOff(CYAN);
    }
    if(u8counter&0x20)
    {
      LedOn(BLUE);
    }
    else
    {
      LedOff(BLUE);
    }
    if(u8counter&0x40)
    {
      LedOn(PURPLE);
    }
    else
    {
      LedOff(PURPLE);
    }
    if(u8counter&0x80)
    {
      LedOn(WHITE);
    }
    else
    {
      LedOff(WHITE);
    }
    if(u8counter==0&&u8flag==0)
    {
      u8counter=0x80;
      u8flag=1;
    }
    if(u8counter==0&&u8flag==1)
    {
      u8counter=0x01;
      u8flag=0;
    }
      if(u8flag==0)
    {
      u8counter<<=1;
    }
    if(u8flag==1)
    {
      u8counter>>=1;
    }
  }  
     if(u16timecounter==5000)
    {
      u16timecounter=0;
      G_u32UserAppFlags=0;
      u8counter=0x01;
      LedOff(RED);
      LedOff(ORANGE);
      LedOff(YELLOW);
      LedOff(GREEN);
      LedOff(CYAN);
      LedOff(BLUE);
      LedOff(PURPLE);
      LedOff(WHITE);  
      PWMAudioOff(BUZZER1);
      
    }
   }
}  
 /* end User2AppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void User2AppSM_Error(void)          
{
  
} /* end User2AppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void User2AppSM_FailedInit(void)          
{
    
} /* end User2AppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
