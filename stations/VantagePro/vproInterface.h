#ifndef INC_vproInterfaceh
#define INC_vproInterfaceh
/*---------------------------------------------------------------------------

  FILENAME:
        vproInterface.h

  PURPOSE:
        Provide non-medium-specific utilities for the Davis Vantage Pro
        interface protocol.

  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        06/07/2005      M.S. Teel       0               Original
        04/12/2008      W. Krenn        1               rainTicksPerInch
                                                        RainCollectorType

  NOTES:


  LICENSE:
        Copyright (c) 2004, Mark S. Teel (mark@teel.ws)

        This source code is released for free distribution under the terms
        of the GNU General Public License.

----------------------------------------------------------------------------*/

/*  ... System include files
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <math.h>

/*  ... Library include files
*/
#include <sysdefs.h>
#include <radtimeUtils.h>
#include <radmsgLog.h>
#include <radsysutils.h>

/*  ... Local include files
*/
#include <datadefs.h>
#include <dbsqlite.h>
#include <dbfiles.h>
#include <config.h>
#include <computedData.h>
#include <daemon.h>
#include <station.h>
#include <serial.h>
#include <ethernet.h>
#include <sensor.h>

/*  ... some definitions
*/

#define VP_BYTE_LENGTH_MAX              SERIAL_BYTE_LENGTH_MAX

#define VP_ACK                          0x06
#define VP_NAK                          0x21
#define VP_CANCEL                       0x1B
#define VP_BADCRC                       0x18
#define VP_CR                           0x0D
#define VP_LF                           0x0A


#define VP_RESPONSE_TIMEOUT(isIP)       ((isIP) ? 10000L : 5000L)

#define VP_PARM_DO_RXCHECK              "DO_RXCHECK"


//  ... define the message types we receive
typedef enum
{
    SER_MSG_ACK             = 1,
    SER_MSG_ARCHIVE         = 2,
    SER_MSG_DMPAFT_HDR      = 3,
    SER_MSG_LOOP            = 4,
    SER_MSG_NONE            = -1
} SER_MSG_TYPES;


//  ... define the messages we receive
typedef struct
{
    UCHAR               interval;
    USHORT              crc;
}__attribute__ ((packed)) ARCHIVE_INTERVAL;

typedef struct
{
    USHORT              pages;
    USHORT              firstRecIndex;
    USHORT              crc;
}__attribute__ ((packed)) DMPAFT_HDR;

typedef struct
{
    UCHAR               seqNo;
    ARCHIVE_RECORD      record[5];
    UCHAR               unused[4];
    USHORT              crc;
}__attribute__ ((packed)) ARCHIVE_PAGE;

//  ... serial format
/*  ... define the LOOP (current readings) serial data format
*/
typedef struct
{
    UCHAR               name[4];
    UCHAR               type;
    USHORT              nextRecord;
    USHORT              barometer;
    short               inTemp;
    UCHAR               inHumidity;
    short               outTemp;
    UCHAR               windSpeed;
    UCHAR               tenMinuteAvgWindSpeed;
    USHORT              windDir;
    UCHAR               extraTemp1;
    UCHAR               extraTemp2;
    UCHAR               extraTemp3;
    UCHAR               resvExtraTemps[4];
    UCHAR               soilTemp1;
    UCHAR               soilTemp2;
    UCHAR               soilTemp3;
    UCHAR               soilTemp4;
    UCHAR               leafTemp1;
    UCHAR               leafTemp2;
    UCHAR               resvLeafTemp[2];
    UCHAR               outHumidity;
    UCHAR               extraHumid1;
    UCHAR               extraHumid2;
    UCHAR               resvHumidity[5];
    USHORT              rainRate;
    UCHAR               UV;
    USHORT              radiation;
    USHORT              stormRain;
    USHORT              stormStartDate;
    USHORT              dayRain;
    USHORT              monthRain;
    USHORT              yearRain;
    USHORT              dayET;
    USHORT              monthET;
    USHORT              yearET;
    UCHAR               soilMoist1;
    UCHAR               soilMoist2;
    UCHAR               resvSoilMoist[2];
    UCHAR               leafWet1;
    UCHAR               leafWet2;
    UCHAR               resvLeafWet[2];
    UCHAR               resvAlarms[16];
    UCHAR               txBatteryStatus;
    USHORT              consBatteryVoltage;
    UCHAR               forecastIcon;
    UCHAR               forecastRule;
    USHORT              sunrise;
    USHORT              sunset;
    UCHAR               lf;
    UCHAR               cr;
    USHORT              crc;
}__attribute__ ((packed)) LOOP_DATA;


// define Vantage Pro specific interface data here
typedef struct
{
    STATES_ID       stateMachine;
    SER_MSG_TYPES   reqMsgType;
    short           elevation;
    USHORT          archivePages;
    USHORT          archiveCurrentPage;
    USHORT          archiveRecOffset;
    int             rxCheckGood;
    int             rxCheckMissed;
    int             rxCheckCRC;
    int             doRXCheck;
    char            rxCheck[64];
    USHORT          rxCheckPercent;
    int             timeSyncFlag;
    int             archiveRetryFlag;
    int             doLoopFlag;
    int             sampleRain;             // to track dailyRain changes
    int             sampleET;               // to track dayET changes

    // vpconfig only
    char            fwVersion[32];
    int             rainSeasonStart;
    short           windDirectionCal;
    UCHAR           listenChannels;
    UCHAR           retransmitChannel;
    UCHAR           transmitterType[16];
    int             rainCollectorSize;
    int             windCupSize;

    // rain
    USHORT          RainCollectorType;
    float           rainTicksPerInch;

} VP_IF_DATA;


// define some extra stimulus types for our purposes
typedef enum
{
    VP_STIM_READINGS        = STIM_IO + 1,
    VP_STIM_ARCHIVE         = STIM_IO + 2
} VPStims;


// ... function prototypes

// ... flush the input buffer from the VP
extern void vpifFlush (WVIEWD_WORK *work);

// ... wake up the Vantage Pro console;
// ... returns OK or ERROR
extern int vpifWakeupConsole (WVIEWD_WORK *work);

// ... get the date and time from the Vantage Pro console;
// ... returns OK or ERROR
extern int vpifGetTime
(
    WVIEWD_WORK *work,
    USHORT      *year,
    USHORT      *month,
    USHORT      *day,
    USHORT      *hour,
    USHORT      *minute,
    USHORT      *second
);

// ... get the console's stored latitude and longitude
// ... return OK or ERROR
extern int vpifGetLatandLong
(
    WVIEWD_WORK *work
);

// ... get the console's archive interval
extern int vpifGetArchiveInterval (WVIEWD_WORK *work);

// ... get the console's rain collector size:
extern int vpifGetRainCollectorSize (WVIEWD_WORK *work);

// ... set the date and time for the Vantage Pro console;
// ... returns OK or ERROR
extern int vpifSetTime
(
    WVIEWD_WORK *work,
    USHORT      year,               // ex: 2001 1998 2004
    USHORT      month,
    USHORT      day,
    USHORT      hour,               // 24 hours
    USHORT      minute,
    USHORT      second
);

// ... set the GMT offset for the Vantage Pro console;
// ... returns OK or ERROR
extern int vpifSetGMTOffset
(
    WVIEWD_WORK *work,
    int         *offset
);

extern int vpifGetAck (WVIEWD_WORK *work, int msWait);
extern int vpifGetRXCheck (WVIEWD_WORK *work);
extern void vpifIndicateStationUp (void);
extern void vpifIndicateLoopDone (void);
extern int vpifSynchronizeConsoleClock (WVIEWD_WORK *work);


// ... this guy reads/parses msgs and updates the internal data stores;
// ... uses the "reqMsgType" of the work area to determine msg type to read;
// ... returns OK or ERROR
extern int vpifReadMessage (WVIEWD_WORK *work, int expectACKFirst);


// ... send msgs to the console;
// ... will set the "reqMsgType" of the work area appropriately;
// ... returns OK or ERROR
extern int vpifSendAck (WVIEWD_WORK *work);
extern int vpifSendNak (WVIEWD_WORK *work);
extern int vpifSendCancel (WVIEWD_WORK *work);
extern int vpifSendDumpAfterRqst (WVIEWD_WORK *work);
extern int vpifSendDumpDateTimeRqst (WVIEWD_WORK *work);
extern int vpifSendLoopRqst (WVIEWD_WORK *work, int number);


// ... define the VP state machine states
typedef enum
{
    VPRO_STATE_STARTPROC            = 1,
    VPRO_STATE_RUN,
    VPRO_STATE_DMPAFT_RQST,
    VPRO_STATE_DMPAFT_ACK,
    VPRO_STATE_RECV_ARCH,
    VPRO_STATE_LOOP_RQST,
    VPRO_STATE_READ_RECOVER,
    VPRO_STATE_ERROR
} VPRO_STATES;

extern int vproStartProcState (int state, void *stimulus, void *data);
extern int vproRunState (int state, void *stimulus, void *data);
extern int vproDumpAfterState (int state, void *stimulus, void *data);
extern int vproDumpAfterAckState (int state, void *stimulus, void *data);
extern int vproReceiveArchiveState (int state, void *stimulus, void *data);
extern int vproLoopState (int state, void *stimulus, void *data);
extern int vproReadRecoverState (int state, void *stimulus, void *data);
extern int vproStopState (int state, void *stimulus, void *data);
extern int vproErrorState (int state, void *stimulus, void *data);


#ifdef _VP_CONFIG_ONLY
typedef enum
{
  VPRO_SENSOR_ISS                   = 0x00,
  VPRO_SENSOR_TEMP                  = 0x01,
  VPRO_SENSOR_HUM                   = 0x02,
  VPRO_SENSOR_TEMP_HUM              = 0x03,
  VPRO_SENSOR_WIND                  = 0x04,
  VPRO_SENSOR_RAIN                  = 0x05,
  VPRO_SENSOR_LEAF                  = 0x06,
  VPRO_SENSOR_SOIL                  = 0x07,
  VPRO_SENSOR_LEAF_SOIL             = 0x08,
  VPRO_SENSOR_SENSORLINK            = 0x09,
  VPRO_SENSOR_NONE                  = 0x0A
} VPRO_SENSOR_TYPES;

typedef enum
{
  VPRO_RAIN_COLLECTOR_0_01_IN       = 0x0,
  VPRO_RAIN_COLLECTOR_0_2_MM        = 0x1,
  VPRO_RAIN_COLLECTOR_0_1_MM        = 0x2
} VPRO_RAIN_COLLECTOR_SIZE;

//  ... methods for vpconfig only
extern int vpconfigGetArchiveInterval (WVIEWD_WORK *work);
extern int vpconfigGetFWVersion (WVIEWD_WORK *work);
extern int vpconfigGetRainSeasonStart (WVIEWD_WORK *work);
extern int vpconfigGetWindDirectionCal (WVIEWD_WORK *work);
extern int vpconfigGetTransmitters (WVIEWD_WORK *work);
extern int vpconfigGetRainCollectorSize (WVIEWD_WORK *work);
extern int vpconfigGetWindCupSize (WVIEWD_WORK *work);
extern int vpconfigSetInterval (WVIEWD_WORK *work, int interval);
extern int vpconfigClearArchiveMemory (WVIEWD_WORK *work);
extern int vpconfigSetElevation (WVIEWD_WORK *work, int elevation);
extern int vpconfigSetGain (WVIEWD_WORK *work, int on);
extern int vpconfigSetLatandLong (WVIEWD_WORK *work, int latitude, int longitude);
extern int vpconfigSetRainSeasonStart (WVIEWD_WORK *work, int startMonth);
extern int vpconfigSetRainYearToDate (WVIEWD_WORK *work, float rainAmount);
extern int vpconfigSetETYearToDate (WVIEWD_WORK *work, float etAmount);
extern int vpconfigSetWindDirectionCal (WVIEWD_WORK *work, int offset);
extern int vpconfigSetSensor (WVIEWD_WORK *work, int channel, VPRO_SENSOR_TYPES sensorType);
extern int vpconfigSetRetransmitChannel (WVIEWD_WORK *work, int channel);
extern int vpconfigSetRainCollectorSize (WVIEWD_WORK *work, VPRO_RAIN_COLLECTOR_SIZE rainCollectorSize);
extern int vpconfigSetWindCupSize (WVIEWD_WORK *work, int isLarge);
#endif

#endif

