/*
 Project:  THC for Plasma CNC controller
 Platform: Arduino UNO R3
 Created:  April 2015
 Version:  02.00
 Updated:  Octo 25 2021 -> Version 02.1
 Added  :  divRatio to Setup Menu, value is 0-200
 By:       Pham Duy Anh - phamduyanh@gmail.com
 
 Require:
  -> input
  <- output
  
  -> serial Tx
  -> serial Rx
  -> reset
  
  -> rotary encoder      digital - interrupt
  -> rotary encoder      digital - interrupt
  -> totary button       digital

  -> plasma Torch on     digital
  -> plasma Arc Ok       analog or digital)
  -> plasma arc voltage  analog 0-5V

  <- Arc Ok              optocoupler
  <- torch Up            optocoupler
  <- torch Down          optocuopler

  <- LCD
  <- LCD
  <- LCD
  <- LCD
  <- LCD
  <- LCD
*/

#define encoderPinA     2                 //PORTD 2 - INT0
#define encoderPinB     3                 //PORTD 3 - INT1
//CKD disable to test
#define buttonPin       4                 //PORTD 4
//#define buttonPin       A1                //PORTB 5
//CKD
#define outputOkPin     A5                //PORTC 5
#define outputUpPin     A4                //PORTC 4
#define outputDnPin     A3                //PORTC 3
#define inputNA         A2
#define arcVoltPin      A0                //PORTC 0

#define defaultLCDtime  500               // s * 100

#define minSetVol       1
#define defSetVol       100
#define maxSetVol       250
#define minDelayTime    1     // > 0
#define defDalayTime    5
#define maxDelayTime    200
#define minHysreresis   1
#define defHysreresis   10
#define maxHysreresis   99
#define minStartVol     50
#define defStartVol     100
#define maxStartVol     250
#define minDivRatio     1     // > 0
#define defDivRatio     50
#define maxDivRatio     100
/*
 Parameter
 * DT  - Delay time - Delay time of output closed despite no supply voltage, when the system detects the main (cutting) arc, 0.1~19.9s.
 * HyS - Hysreresis - The hysteresis voltage - a rangge in which the voltage is measured, in which ther are signals controlling Up/Down, (+-1V ~ +-15V)
 * StU - Started Voltage - Value of the off load voltage detection in the plasma cutter - uesd to detect the main (cutting) arc: 50~300V.
 * dTime
 * divRatio
*/
#define _SetV     0
#define _DT       1
#define _HyS      2
#define _StV      3
#define _divRatio 4

int SetV, DT, HyS, StV;
//int divRatio = 43 ;
int divRatio = 51 ;

int ArcV;
int oldValue;

// Array of Param
//              SetV, DT, HYS, StV, divRatio,
// Param Address   0,  1,   2,   3,       4,
int Param[5] = {SetV, DT, HyS, StV, divRatio};
byte ParamItem = 5;

// Enable to do THC
boolean Do;
// Encoder, menu, pos
int encoderVal;
byte menu = 0;
byte pos  = 0;
byte show = 0;
unsigned int LCDtime = 0;

void setup()
{
  Setup_Encoder();
  Setup_LCD();
  Setup_THC();
  Setup_Timer2();
  setup_step();
  //Read parameter from EEPROM
  ReadData();
  //Set value for all parameter
  SetV      = Param[_SetV];
  DT        = Param[_DT];
  HyS       = Param[_HyS];
  StV       = Param[_StV];
  divRatio  = Param[_divRatio];
  //Preset value for encoder
  encoderVal = SetV;
  Serial.begin(9600);
}

void loop()
{
  //Read voltage value from ADC
  ArcV = double(analogRead(arcVoltPin)) * double(100.00 / divRatio);
  
  checkButton();
  checkMenu();
  doTHC();
  doLCD();

  //RS232();
}




  
