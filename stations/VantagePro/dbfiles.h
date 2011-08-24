#ifndef INC_dbfilesh
#define INC_dbfilesh
/*---------------------------------------------------------------------------
 
  FILENAME:
        dbfiles.h
 
  PURPOSE:
        Provide the wview daemon database files definitions.
 
  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        08/16/03        M.S. Teel       0               Original
        04/12/2008      W. Krenn        1               RainCollectorType
 
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
#include <string.h>
#include <signal.h>
#include <math.h>
#include <sys/stat.h>

/*  ... Library include files
*/
#include <sysdefs.h>
#include <datadefs.h>
#include <windAverage.h>

/*  ... Local include files
*/


/*  ... API definitions
*/

/*  !!!!!!!!!!!!!!!!!!  HIDDEN, NOT FOR API USE  !!!!!!!!!!!!!!!!!!
*/
// define 'B' type records first - this is what we use internally
// 'A' < Firmware Version 04/24/2002 <= 'B'
typedef struct
{
    USHORT              date;
    USHORT              time;
    short               outTemp;
    short               highOutTemp;
    short               lowOutTemp;
    USHORT              rain;
    USHORT              highRainRate;
    USHORT              barometer;
    USHORT              radiation;
    USHORT              windSamples;
    USHORT              inTemp;
    UCHAR               inHumidity;
    UCHAR               outHumidity;
    UCHAR               avgWindSpeed;
    UCHAR               highWindSpeed;
    UCHAR               highWindDir;
    UCHAR               prevWindDir;
    UCHAR               UV;                 // extra data begins here
    UCHAR               ET;
    USHORT              highRadiation;
    UCHAR               highUV;
    UCHAR               fcstRule;
    UCHAR               leafTemp1;
    UCHAR               leafTemp2;
    UCHAR               leafWet1;
    UCHAR               leafWet2;
    UCHAR               soilTemp1;
    UCHAR               soilTemp2;
    UCHAR               soilTemp3;
    UCHAR               soilTemp4;
    UCHAR               recordType;
    UCHAR               extraHumid1;
    UCHAR               extraHumid2;
    UCHAR               extraTemp1;
    UCHAR               extraTemp2;
    UCHAR               extraTemp3;
    UCHAR               soilMoist1;
    UCHAR               soilMoist2;
    UCHAR               soilMoist3;
    UCHAR               soilMoist4;
}
__attribute__ ((packed)) ARCHIVE_RECORD;


// define the old 'A' format here
typedef struct
{
    USHORT              date;
    USHORT              time;
    short               outTemp;
    short               highOutTemp;
    short               lowOutTemp;
    USHORT              rain;
    USHORT              highRainRate;
    USHORT              barometer;
    USHORT              radiation;
    USHORT              windSamples;
    USHORT              inTemp;
    UCHAR               inHumidity;
    UCHAR               outHumidity;
    UCHAR               avgWindSpeed;
    UCHAR               highWindSpeed;
    UCHAR               highWindDir;
    UCHAR               prevWindDir;
    UCHAR               UV;                 // extra data begins here
    UCHAR               ET;
    UCHAR               invalid1;
    UCHAR               soilMoist1;
    UCHAR               soilMoist2;
    UCHAR               soilMoist3;
    UCHAR               soilMoist4;
    UCHAR               soilTemp1;
    UCHAR               soilTemp2;
    UCHAR               soilTemp3;
    UCHAR               soilTemp4;
    UCHAR               leafWet1;
    UCHAR               leafWet2;
    UCHAR               leafWet3;
    UCHAR               leafWet4;
    UCHAR               extraTemp1;
    UCHAR               extraTemp2;
    UCHAR               extraHumid1;
    UCHAR               extraHumid2;
    USHORT              reedClosed;
    USHORT              reedOpen;
    UCHAR               unused1;
}
__attribute__ ((packed)) ARCHIVE_RECORD_A;


#define DBFILES_RECORD_SIZE         88

enum DailySummaryTimeIndexes
{
    High_Outside_Temperature        = 0,
    Low_Outside_Temperature         = 1,
    High_Inside_Temperature         = 2,
    Low_Inside_Temperature          = 3,
    High_Wind_Chill                 = 4,
    Low_Wind_Chill                  = 5,
    High_Dew_Point                  = 6,
    Low_Dew_Point                   = 7,
    High_Outside_Humidity           = 8,
    Low_Outside_Humidity            = 9,
    High_Inside_Humidity            = 10,
    Low_Inside_Humidity             = 11,
    High_Barometer                  = 12,
    Low_Barometer                   = 13,
    High_Wind_Speed                 = 14,
    High_Average_Wind_Speed         = 15,
    High_Rain_Rate                  = 16,
    High_UV                         = 17,

    High_Solar_Rad                  = 0,
    High_Outside_Heat_Index         = 1,
    Low_Outside_Heat_Index          = 2,
    High_Outside_THSW_Index         = 3,
    Low_Outside_THSW_Index          = 4,
    High_Outside_THW_Index          = 5,
    Low_Outside_THW_Index           = 6,
    High_Outside_Wet_Bulb_Temp      = 7,
    Low_Outside_Wet_Bulb_Temp       = 8
};


//  ... this describes each day in an archive file
typedef struct
{
    short       recordsInDay;       // includes the daily summary record (counts as 2)
    int         startPos;           // The index (starting at 0) of the daily summary record
}
__attribute__ ((packed)) DayIndex;

//  ... Header for each archive file
typedef struct
{
    char        idCode [16];        // = {'W','D','A','T','5','.','0',0,0,0,0,0,0,0,5,0}
    int         totalRecords;
    DayIndex    dayIndex [32];      // index records for each day. Index 0 is not used
    // (i.e. the 1'st is at index 1, not index 0)
}
__attribute__ ((packed)) HeaderBlock;


//  ... After the Header are a series of 88-byte records with one of the following
//  ... formats.  Note that each day will begin with a daily summary record.

//  ... Daily Summary Record (176 bytes)
typedef struct
{
    UCHAR   dataType1;
    UCHAR   reserved1;              // this will cause rest of fields to start on an even address

    short   dataSpan;               // total # minutes accounted for by physical recs for this day
    short   hiOutTemp, lowOutTemp;  // tenths of a degree F
    short   hiInTemp, lowInTemp;    // tenths of a degree F
    short   avgOutTemp, avgInTemp;  // tenths of a degree F (integrated over the day)
    short   hiChill, lowChill;      // tenths of a degree F
    short   hiDew, lowDew;          // tenths of a degree F
    short   avgChill, avgDew;       // tenths of a degree F
    short   hiOutHum, lowOutHum;    // tenths of a percent
    short   hiInHum, lowInHum;      // tenths of a percent
    short   avgOutHum;              // tenths of a percent
    short   hiBar, lowBar;          // thousanths of an inch Hg
    short   avgBar;                 // thousanths of an inch Hg
    short   hiSpeed, avgSpeed;      // tenths of an MPH
    short   dailyWindRunTotal;      // 1/10'th of an mile
    short   hi10MinSpeed;           // the highest average wind speed record
    UCHAR   dirHiSpeed, hi10MinDir; // direction code (0-15, 255)
    short   dailyRainTotal;         // 1/1000'th of an inch
    short   hiRainRate;             // 1/100'th inch/hr ???
    short   dailyUVDose;            // 1/10'th of a standard MED
    UCHAR   hiUV;                   // tenth of a UV Index
    UCHAR   timeValues1[27];        // space for 18 time values (see below)

    UCHAR   dataType2;              // start of 2nd summary record
    UCHAR   reserved2;              // this will cause rest of fields to start on an even address

    // this field is not used now.
    USHORT  todaysWeather;          // bitmapped weather conditions (Fog, T-Storm, hurricane, etc)

    short   numWindPackets;         // # of valid packets containing wind data,
    // this is used to indicate reception quality
    short   hiSolar;                // Watts per meter squared
    short   dailySolarEnergy;       // 1/10'th Ly
    short   minSunlight;            // number of accumulated minutes where the avg solar rad > 150
    short   dailyETTotal;           // 1/1000'th of an inch
    short   hiHeat, lowHeat;        // tenths of a degree F
    short   avgHeat;                // tenths of a degree F
    short   hiTHSW, lowTHSW;        // tenths of a degree F
    short   hiTHW, lowTHW;          // tenths of a degree F

    short   integratedHeatDD65;     // integrated Heating Degree Days (65F threshold)
    // tenths of a degree F - Day

    // Wet bulb values are not calculated
    short   hiWetBulb, lowWetBulb;  // tenths of a degree F
    short   avgWetBulb;             // tenths of a degree F

    UCHAR   dirBins[24];            // space for 16 direction bins
    // (Used to calculate monthly dominent Dir)

    UCHAR   timeValues2[15];        // space for 10 time values (see below)

    short   integratedCoolDD65;     // integrated Cooling Degree Days (65F threshold)
    // tenths of a degree F - Day
    UCHAR   reserved3[11];
}
__attribute__ ((packed)) DailySummaryRecord;

//  ... standard archive record (88 bytes)
typedef struct
{
    UCHAR   dataType;
    UCHAR   archiveInterval;        // number of minutes in the archive
    UCHAR   iconFlags;              // Icon associated with this record, plus Edit flags
    UCHAR   moreFlags;              // Tx Id, etc.

    short   packedTime;             // minutes past midnight of the end of the archive period
    short   outsideTemp;            // tenths of a degree F
    short   hiOutsideTemp;          // tenths of a degree F
    short   lowOutsideTemp;         // tenths of a degree F
    short   insideTemp;             // tenths of a degree F
    short   barometer;              // thousanths of an inch Hg
    short   outsideHum;             // tenths of a percent
    short   insideHum;              // tenths of a percent
    USHORT  rain;                   // number of clicks + rain collector type code
    short   hiRainRate;             // clicks per hour
    short   windSpeed;              // tenths of an MPH
    short   hiWindSpeed;            // tenths of an MPH
    UCHAR   windDirection;          // direction code (0-15, 255)
    UCHAR   hiWindDirection;        // direction code (0-15, 255)
    short   numWindSamples;         // number of valid ISS packets containing wind data
    // this is a good indication of reception
    short   solarRad, hisolarRad;   // Watts per meter squared
    UCHAR   UV, hiUV;               // tenth of a UV Index

    UCHAR   leafTemp[4];            // (whole degrees F) + 90

    short   newSensors[7];          // reserved for future use
    UCHAR   forecast;               // forecast code during the archive interval

    UCHAR   ET;                     // thousandths of an inch

    UCHAR   soilTemp[6];            // (whole degrees F) + 90
    UCHAR   soilMoisture[6];        // centibars of dryness
    UCHAR   leafWetness[4];         // Leaf Wetness code (0-15, 255)
    UCHAR   extraTemp[7];           // (whole degrees F) + 90
    UCHAR   extraHum[7];            // whole percent
}
__attribute__ ((packed)) ArchiveRecord;

/*  !!!!!!!!!!!!!!!!!!!!  END HIDDEN SECTION  !!!!!!!!!!!!!!!!!!!!!
*/


/*  ... API methods
*/

/*  ... 0 <= dir <= 15
*/
extern char *dbfBuildWindDirString (int dir);


/*  ... provide a routine to generate an ASCII browser view file
    ... 'outputFilename' containing archive records;
    ... 'start' and 'stop' are of the form: (("year" - 2000) * 100) + "month"
    ... and are inclusive;
    ... returns OK or ERROR
*/
extern int dbfExportArchiveFile
(
    char            *archivePath,
    USHORT          start,
    USHORT          stop,
    char            *outputFilename
);

/*  ... provide a routine to generate an ASCII browser view file
    ... 'outputFilename' containing daily summary records;
    ... 'start' and 'stop' are of the form: (("year" - 2000) * 100) + "month"
    ... and are inclusive;
    ... returns OK or ERROR
*/
extern int dbfExportDailySummaryFile
(
    char            *archivePath,
    USHORT          start,
    USHORT          stop,
    char            *outputFilename
);

/*  ... calculate averages over a given period of time
    ... (given in arcInterval samples);
    ... this will zero out the HISTORY_DATA store before beginning
    ... the processing;
    ... returns OK or ERROR
*/
extern int dbfGetAverages
(
    int             isMetricUnits,
    int             isMetricMM,
    int             arcInterval,
    char            *archivePath,
    HISTORY_DATA    *store,
    int             startmin,
    int             starthour,
    int             startday,
    int             startmonth,
    int             startyear,
    int             numSamples
);


/*  ... dbfGetNoaaDay: summarize day records for NOAA reports;
    ... this will zero out the NOAA_DAY_REC store before beginning;
    ... returns OK or ERROR if day not found in archives
*/
extern int dbfGetNoaaDay
(
    char            *archivePath,
    NOAA_DAY_REC    *store,
    int             year,
    int             month,
    int             day
);

extern int dbfWriteDailyArchiveReport
(
    char            *archivePath,
    char            *filename,
    time_t          timeval,
    int             isMetric,
    int             isMetricMM,
    int             arcInterval,
    void            (*writeHeader) (FILE *file)
);

extern int dbfUpdateDailyArchiveReport
(
    char            *file,
    HISTORY_DATA    *data,
    void            (*writeHeader) (FILE *file),
    int             isMetric
);


/*  ... ######!!! The following utilities are for the daemon only !!!######
*/
/*  ... append an archive record to the appropriate dbfile;
    ... if the date extends into a new month, this will create the new dbfile
    ... and insert this archive record;
    ... returns OK or ERROR
*/
extern int dbfStoreArchiveRecord
(
    char            *archivePath,
    ARCHIVE_RECORD  *record,
    int             archiveInterval,
    USHORT          RainCollectorType
);

/*  ... delete all day records starting on a certain day for that month;
    ... returns OK or ERROR
*/
int dbfDeleteArchiveRecords
(
    char            *archivePath,
    int             year,
    int             month,
    int             day
);

/*  ... search the archive path for the most recent archive record date;
    ... places the most recent date and time in 'date' and 'time' or all
    ... zero's if no archive record found;
    ... returns OK or ERROR if no archives found
*/
extern int dbfGetNewestArchiveTime
(
    char            *archivePath,
    USHORT          curMonth,
    USHORT          curYear,
    USHORT          *date,
    USHORT          *time,
    ArchiveRecord   *newestRecord
);

/*  ... search the archive path for the next archive record after 'date' and
    ... 'time' (in packed format);
    ... places the most recent date and time in 'date' and 'time' if archive
    ... record is found;
    ... returns OK or ERROR if no archive found
*/
extern int dbfGetNextArchiveRecord
(
    char            *archivePath,
    USHORT          *date,
    USHORT          *time,
    ArchiveRecord   *recordStore
);

/*  ... dbfCopyArchiveRecordToConsoleRecord - copy an archive file record to
    ... the station message record format
*/
extern int dbfCopyArchiveRecordToConsoleRecord
(
    ArchiveRecord       *src,
    ARCHIVE_RECORD      *dest,
    USHORT              date,
    USHORT              time
);

/*  ... dbfIncrementConsoleTime - increment the station time representation by
    ... 'numMinutes' minutes
*/
extern int dbfIncrementConsoleTime
(
    ARCHIVE_RECORD      *record,
    int                 numMinutes
);

// dbfComputePackedDelta - compute the delta in minutes between 2 packed times
extern int dbfComputePackedDelta
(
    USHORT          oldDate,
    USHORT          oldTime,
    USHORT          newDate,
    USHORT          newTime
);

#endif

