/*
  Sensor433 - An Arduino Library for sending and receiving values with simple
  433MHz transmitters and receivers between Arduino boards
  A typical use case is to let small battery powered Arduino Pro Mini boards send sensor
  values to an ESP8266 board that creates MQTT messages on the WiFi network from the values
   
  Lars Bergqvist 2017
  
  Uses rc-switch: https://github.com/sui77/rc-switch/

  The transmitter can send a float between -327.67 and + 327.67
  or an unsigned int, i.e. a word (a value between 0 and 65535)
  A sensor id (a unique value between 0 and 15) is part of the transmitted message

  The receiver polls for incoming messages and getNewData() returns true when
  a new message is available. getData() returns a struct with the data and the sensor id
  from the sender

  Typical usage:

  Sender:
  transmitter.send(SENSOR1, 1234);
  transmitter.send(SENSOR2, 12.34);

  Receiver:
  if ( receiver.hasNewData() )
  {
    ReceivedMessage data = receiver.getData();
    if ( data.sensorId == SENSOR1 and data.typeSensor == TYPESENSOR2 )
    {
      word  myTwelveBit = data.dataAsWord;
      // Do something
    }
    else if ( data.sensorId == SENSOR2 )
    {
      float myFloatVale = data.dataAsFloat;
      // Do something
    }
    
  }

 */


#ifndef Sensor433_h
#define Sensor433_h

#include "Arduino.h"
#include "RCSwitch.h"

namespace Sensor433
{
  //
  // Transmitter class
  //
  
  class Transmitter
  {
    byte seq;
    RCSwitch rc = RCSwitch();
    
    public:
      Transmitter(byte transmitterPin);
      void begin();   
      void sendWord(byte sensorId, byte typeSensor, word data); // max 12 bit x data
      void sendFloat(byte sensorId, byte typeSensor, float data); // max 12 bit x data sign included
  
    private:
      unsigned long encode32BitsToSend(byte sensorId, byte typeSensor, byte seq, word data);
      word  encodeFloatToTwelveBit(float floatValue); // max 12 bit x float value
  };

  
  //
  // Receiver class
  //

  struct ReceivedMessage
  {
    byte  sensorId;
		byte  typeSensor;
    word  dataAsWord;
    float dataAsFloat;
  };


  class Receiver
  {
    RCSwitch rc = RCSwitch();
  
    public:
      Receiver(byte receiverInterruptNumber);
      bool hasNewData();
      ReceivedMessage getData();
  
    private:
      float decodeTwelveBitToFloat(word TwelveBit);
};

}
#endif

