// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3

#include "EmonLib.h"                   // Include Emon Library
#include "microsmooth.h"

/*** Local Macro ***/
#define fVOLTAGE (230.0)
#define fCURRENT_CALIBRATION (111.1)
#define NUMBER_OF_SAMPLE (1500)
#define WAIT_TIME (100)
#define UART_BAUDRATE (115200)

/*** Local functions declaration ***/
void vidPrintMeasure(void);
void printTime(void);
void vidApplyFilter(void);

//Serial is connected to USB
//Serial 1 is connected to GPIO PD3 and PD2

/*** Local variables ***/
static EnergyMonitor emon1;
static EnergyMonitor emon2;

static double Irms1;
static double Irms2;
static double filteredIrms1;
static double filteredIrms2;
static uint16_t *history1;
static uint16_t *history2;

/*** Global functions defintion ***/
void setup()
{  
  Serial.begin(UART_BAUDRATE);
  Serial1.begin(UART_BAUDRATE);
  
  emon1.current(1, fCURRENT_CALIBRATION);             // Current: input pin, calibration.
  emon2.current(2, fCURRENT_CALIBRATION);

  filteredIrms1 = 0;
  filteredIrms2 = 0;

  /*Init filterring with simple moving average*/
  history1 = ms_init(SMA);
  history2 = ms_init(SMA);
}

void loop()
{
  Irms1 = emon1.calcIrms(NUMBER_OF_SAMPLE);  // Calculate Irms only
  Irms2 = emon2.calcIrms(NUMBER_OF_SAMPLE);

  vidApplyFilter();
  vidPrintMeasure();
  
  delay(WAIT_TIME);
}

/*** Local functions defintion ***/

void vidPrintMeasure(void)
{
  char buffer[100];
  char str_temp1[10], str_temp2[10];

  /*This function replace the %f which is not support in Arduino's sprintf*/
  dtostrf(filteredIrms1, 4, 2, str_temp1);
  dtostrf(filteredIrms2, 4, 2, str_temp2);

  sprintf(buffer,"Irms1:%s,Irms2:%s",str_temp1, str_temp2);
  Serial.println(buffer);
  Serial1.println(buffer);
  /*
  Serial.print("Irms1:");
  Serial.print(filteredIrms1);
  Serial.print(",");
  Serial.print("Irms2:");
  Serial.println(filteredIrms2);

  Serial1.print("Irms1:");
  Serial1.print(filteredIrms1);
  Serial1.print(",");
  Serial1.print("Irms2:");
  Serial1.println(filteredIrms2);
  */
}

void vidApplyFilter(void)
{
  filteredIrms1 = sma_filter(Irms1*fVOLTAGE, history1);
  filteredIrms2 = sma_filter(Irms2*fVOLTAGE, history2);
}
