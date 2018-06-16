// Based on the work by DFRobot

#include "INDSP_I2C.h"


INDSP_I2C::INDSP_I2C(int i2c_addr):i2c_bus_addr(i2c_addr){
}

void INDSP_I2C::init(void){
    // Start I2C comms
  Wire.begin();
}
void INDSP_I2C::setMute(enum trueFalse val){
  i2c_indsp_write_byte(DSP_Mute, val);
}

void INDSP_I2C::setNoiseReduction(unsigned char val){
  if(val > 45) val = 45;
  i2c_indsp_write_byte(DSP_Noise_setNoiseReduction, val);
}

void INDSP_I2C::noiseApply(enum trueFalse val){
  i2c_indsp_write_byte(DSP_Noise_apply, val);
}

void INDSP_I2C::setBandGain(enum bandGain b,unsigned char g){
  if (g > EQ_MAX_BAND_ATTEN) g = EQ_MAX_BAND_ATTEN;
  i2c_indsp_write_byte (b,g);
}

void INDSP_I2C::setMasterGain(unsigned char val){
  if(val > EQ_MAX_MASTER_GAIN) val = EQ_MAX_MASTER_GAIN;
  i2c_indsp_write_byte(DSP_EQU_MasterGain, val);
}

void INDSP_I2C::EQUapply(enum trueFalse val){
    i2c_indsp_write_byte(DSP_EQU_apply, val);
  }

void INDSP_I2C::setAutoNotch(enum trueFalse val){
  i2c_indsp_write_byte(DSP_AutoNotch_apply, val);
}

void INDSP_I2C::selectFilter(enum DSP_FILTER val){
  i2c_indsp_write_byte(DSP_FIL_filterSelect, val);
}

void INDSP_I2C::filterApply(enum trueFalse val){
  i2c_indsp_write_byte(DSP_Filter_apply, val);
}
 

void INDSP_I2C::i2c_indsp_write_byte( unsigned char indspaddress, unsigned char data ) {

  Wire.beginTransmission(i2c_bus_addr);
  Wire.write(indspaddress);
  Wire.write(data);
  Wire.endTransmission();
  delay(20);
}
unsigned char INDSP_I2C::i2c_indsp_read_byte( unsigned char indspaddress ) {
  unsigned char rdata;
  Wire.beginTransmission(i2c_bus_addr);
  Wire.write(indspaddress);
  Wire.endTransmission(false);
  // Wire.beginTransmission(i2c_bus_addr);
  Wire.requestFrom(i2c_bus_addr, 1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

	
