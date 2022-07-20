#include <EEPROM.h>
void Default()
{
  SetV     = defSetVol;
  DT       = defDalayTime;
  HyS      = defHysreresis;
  StV      = defStartVol;
  divRatio = defDivRatio;
  
  EEPROM.write(_SetV     ,      SetV);
  EEPROM.write(_DT       ,        DT);
  EEPROM.write(_HyS      ,       HyS);
  EEPROM.write(_StV      ,       StV);
  EEPROM.write(_divRatio ,  divRatio);
}

void ReadData()
{
  //              SetV,  DT, HYS, StV, dRatio,
  // Param Address   0,   1,   2,   3,      4,
  for (int j = 0; j < ParamItem; j++)
  {
    Param[j] = EEPROM.read(j);
  }
}

void SaveData(int add, int value)
{
  EEPROM.write(add, value);
}
