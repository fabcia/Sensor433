#include "Sensor433.h"
#include "Arduino.h"

#include "RCSwitch.h"

namespace Sensor433
{

  
//
// Transmitter class
//

Transmitter::Transmitter(byte transmitterPin) : seq(0)
{
  rc.enableTransmit(transmitterPin); 
  rc.setRepeatTransmit(25);      
}

void Transmitter::sendWord(byte sensorId, byte typeSensor, word data)
{
  unsigned long dataToSend = encode32BitsToSend(sensorId, typeSensor, seq, data);

  // Send the data twice to reduce risk of receiver getting noise
  rc.send(dataToSend, 32);
  delay(1000);
  rc.send(dataToSend, 32);
  
  seq++;
  if (seq > 15)
  {
    seq = 0;
  }  
}

void Transmitter::sendFloat(byte sensorId, byte typeSensor, float data)
{
  word TwelveBit = encodeFloatToTwelveBit(data);
  sendWord(sensorId, typeSensor, TwelveBit);
}

unsigned long Transmitter::encode32BitsToSend(byte sensorId, byte typeSensor, byte seq, word data)
{
    byte checkSum = sensorId + typeSensor + seq + data;
    unsigned long byte3 = ((0x3F & sensorId) << 2) + (0x03 & typeSensor);
    unsigned long byte2_and_byte_1 = ((0x0F & seq) << 12) + (0x0FFF & data)
    byte byte0 = 0xFF & checkSum;
    unsigned long dataToSend = (byte3 << 24) + (byte2_and_byte_1 << 8) + byte0;
    return dataToSend;
}

word Transmitter::encodeFloatToTwelveBit(float floatValue)
{
  bool sign = false;
  
  if (floatValue < 0) 
    sign=true;  
      
  int times100 = (100*fabs(floatValue));
  unsigned int twoBytes = times100 & 0X07FF;
  
  if (sign)
    twoBytes |= 1 << 11;

  return twoBytes;
}


//
// Receiver class
//


Receiver::Receiver(byte receiverInterruptNumber)
{
  rc.enableReceive(receiverInterruptNumber);  
}

ReceivedMessage receivedMessage = { 0, 0, 0, 0.0 };
ReceivedMessage Receiver::getData()
{
  return receivedMessage;
}

unsigned long prevValue = 0;
int numIdenticalInRow = 0;

bool Receiver::hasNewData()
{
  if (!rc.available())
    return false;
  
  unsigned long newValue = rc.getReceivedValue();
  if (newValue == 0)
    return false;

  // Get the different parts of the 32-bit / 4-byte value
  // that has been read over 433MHz
  byte checksum = newValue & 0x000000FF;
	word byte2_and_byte_1 = (newValue >> 8) & 0x0000FFFF;
  word sensordata = byte2_and_byte_1 & 0x0FFF;
	byte seq = (byte2_and_byte_1 & 0xF000) >> 12;
  byte byte3 = (newValue >> 24) & 0x000000FF;
	byte typeSensor = byte3 & 0x03;
  byte sensorId = (byte3 & 0xF12) >> 6;

  byte calculatedCheckSum = 0xFF & (sensorId + typeSensor + seq + sensordata);
  
  if ((calculatedCheckSum == checksum) && (seq <= 15))
  {
    if (newValue == prevValue)
    {
      numIdenticalInRow++;
    }
    else
    {
      numIdenticalInRow = 0;
    }
    
    prevValue = newValue;

    // Require at least two readings of the same value in a row
    // to reduce risk of reading noise. Ignore any further duplicated
    // values
    if (numIdenticalInRow == 2)
    {
      receivedMessage.sensorId = sensorId;
			receivedMessagetypeSensor = typeSensor;
      receivedMessage.dataAsWord = sensordata;
      receivedMessage.dataAsFloat = decodeTwelveBitToFloat(sensordata);
      return true;
    }
  }

  rc.resetAvailable();

  return false;
}

float Receiver::decodeTwelveBitToFloat(word TwelveBit)
{
  bool sign = false;
  
  if ((TwelveBit & 0x0800) == 0x0800) 
    sign=true;  

	float fl = (TwelveBit & 0x07FF) / 100.0;

  if (sign)
    fl = -fl;

  return fl;
}

}
