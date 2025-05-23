#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include "ns_dpmi.h"
#include "ns_task.h"
#include "ns_cards.h"
#include "ns_tandy.h"
#include "ns_muldf.h"
#include "ns_fxm.h"

#include "m_misc.h"

#include "doomstat.h"

#include "options.h"

#include "i_file.h"

#define TANDY_Port 0x2C0

static int TANDY_Installed = 0;

static char *TANDY_BufferStart;
static char *TANDY_CurrentBuffer;
static int TANDY_BufferNum = 0;
static int TANDY_NumBuffers = 0;
static int TANDY_TransferLength = 0;
static int TANDY_CurrentLength = 0;

static char *TANDY_SoundPtr;
volatile int TANDY_SoundPlaying;

static task *TANDY_Timer;

void (*TANDY_CallBack)(void);

/*---------------------------------------------------------------------
   Function: TANDY_ServiceInterrupt

   Handles interrupt generated by sound card at the end of a voice
   transfer.  Calls the user supplied callback function.
---------------------------------------------------------------------*/

unsigned char *TandyLUTdb;

static void TANDY_Silence()
{
    // Silence 3 channels + noise
    outp(TANDY_Port, 0x9F); // Channel 0
    outp(TANDY_Port, 0xBF); // Channel 1
    outp(TANDY_Port, 0xDF); // Channel 2
    outp(TANDY_Port, 0xFF); // Noise
}

static void TANDY_StartStreaming()
{
    TANDY_Silence();

    // Set channel 0 to 125 KHz
    outp(TANDY_Port, 0x80);
    outp(TANDY_Port, 0x00);
}

static void TANDY_ServiceInterrupt(task *Task)
{
    unsigned char value = TandyLUTdb[*((unsigned char *)TANDY_SoundPtr)];

    // Set volume for channel 0
    outp(TANDY_Port, value);

    TANDY_SoundPtr++;

    TANDY_CurrentLength--;
    if (TANDY_CurrentLength == 0)
    {
        // Keep track of current buffer
        TANDY_CurrentBuffer += TANDY_TransferLength;
        TANDY_BufferNum++;
        if (TANDY_BufferNum >= TANDY_NumBuffers)
        {
            TANDY_BufferNum = 0;
            TANDY_CurrentBuffer = TANDY_BufferStart;
        }

        TANDY_CurrentLength = TANDY_TransferLength;
        TANDY_SoundPtr = TANDY_CurrentBuffer;

        // Call the caller's callback function
        if (TANDY_CallBack != NULL)
        {
            MV_ServiceVoc();
        }
    }
}

/*---------------------------------------------------------------------
   Function: TANDY_StopPlayback

   Ends the transfer of digitized sound to the Sound Source.
---------------------------------------------------------------------*/

void TANDY_StopPlayback(void)
{
    if (TANDY_SoundPlaying)
    {
        TS_Terminate(TANDY_Timer);
        TANDY_SoundPlaying = 0;
        TANDY_BufferStart = NULL;
    }

    TANDY_Silence();
}

/*---------------------------------------------------------------------
   Function: TANDY_BeginBufferedPlayback

   Begins multibuffered playback of digitized sound on the Sound Source.
---------------------------------------------------------------------*/

int TANDY_BeginBufferedPlayback(
    char *BufferStart,
    int BufferSize,
    int NumDivisions,
    void (*CallBackFunc)(void))
{
    if (TANDY_SoundPlaying)
    {
        TANDY_StopPlayback();
    }

    TANDY_CallBack = CallBackFunc;

    TANDY_BufferStart = BufferStart;
    TANDY_CurrentBuffer = BufferStart;
    TANDY_SoundPtr = BufferStart;
    // VITI95: OPTIMIZE
    TANDY_TransferLength = BufferSize / NumDivisions;
    TANDY_CurrentLength = TANDY_TransferLength;
    TANDY_BufferNum = 0;
    TANDY_NumBuffers = NumDivisions;

    TANDY_SoundPlaying = 1;

    TANDY_Timer = TS_ScheduleTask(TANDY_ServiceInterrupt, FX_MixRate, 1, NULL);
    TS_Dispatch();

    return (TANDY_Ok);
}

/*---------------------------------------------------------------------
   Function: TANDY_Init

   Initializes the Sound Source prepares the module to play digitized
   sounds.
---------------------------------------------------------------------*/

int TANDY_Init(int soundcard)
{
    int i;

    if (TANDY_Installed)
    {
        TANDY_Shutdown();
    }

    TandyLUTdb = I_ReadBinaryStatic("DATA\\TANDY.BIN", 256);

    TANDY_StartStreaming();

    TANDY_SoundPlaying = 0;

    TANDY_CallBack = NULL;

    TANDY_BufferStart = NULL;

    TANDY_Installed = 1;

    return (TANDY_Ok);
}

/*---------------------------------------------------------------------
   Function: TANDY_Shutdown

   Ends transfer of sound data to the Sound Source.
---------------------------------------------------------------------*/

void TANDY_Shutdown(void)
{
    TANDY_StopPlayback();

    TANDY_SoundPlaying = 0;

    TANDY_BufferStart = NULL;

    TANDY_CallBack = NULL;

    TANDY_Installed = 0;
}
