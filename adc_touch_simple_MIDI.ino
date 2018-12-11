// This sketch realizes a simple USB Piano with no additional circuitery.
// it uses the build in USBMIDI possibility of the Arduino Leonardo
// Simply connect wires to Analog in A0 - A6 and keep start playing! 
// (C) Moritz Simon Geist 2018 www.sonicrobots.com

#include <ADCTouch.h>// the ADC Touch Libary has to be installed via  libary manager           
#include "MIDIUSB.h" // MIDI USB Libary for Arduino Leonardo

int ref0, ref1, ref2, ref3, ref4;       //reference values to remove offset
int note[6] = {48, 50, 52, 54, 56, 58} ; // Here you can set up which notes are wired to which Analog input. E.g. 48 --> C3
boolean isNoteOn[6];                    //Array to store data if a not has been triggered.                  
int newValues[6];                       // Array to store new sensor data from Analog inputs
int oldValues[6] = {1, 1, 1, 1, 1, 1};  // Array to store old sensor data to compare with

void noteOn(byte channel, byte pitch, byte velocity)
{
      midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
      MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) 
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// Start of actual programm
void setup()
    {
     
        Serial.begin(115200);             // Setup Midi Serial Port over USB
        ref0 = ADCTouch.read(A0, 500);    //create reference values to
        ref1 = ADCTouch.read(A1, 500);      //account for the capacitance of the pad
        ref2 = ADCTouch.read(A2, 500);    
        ref3 = ADCTouch.read(A3, 500);      
        ref4 = ADCTouch.read(A4, 500);      

    }
     
    void loop()
    {
        newValues[0] = (ADCTouch.read(A0) - ref0);     //Read Analog Inpunt from A0 and substract reference value. Put it into "newvalues" array
        newValues[1] = (ADCTouch.read(A1) - ref1);     
        newValues[2] = (ADCTouch.read(A2) - ref2);     
        newValues[3] = (ADCTouch.read(A3) - ref3);     
        newValues[4] = (ADCTouch.read(A4) - ref4);     
        
 for (int i=0; i < 5; i++){                           // For loop goes thru array corresponding to Analog input 0..5 
      if (isNoteOn [i] == false)                      // If not has already been triggered do nothing
      {                                           
          if ((newValues[i] > 200) && (oldValues[i] < 200))                 // Compare if the threshold of the ADC Value is in the range of 200 or above?
          { isNoteOn[i] = true;                   
            
            
            noteOn(0, note[i], 127);   // Send MIDI Channel 0, NOTE, normal velocity
            
          }
      }
      else                                            // Send noteOff if the ADC detected a value below 180 (thus, wire released)
      {
      if ((newValues[i] < 180) && (oldValues[i] > 180))
        {
          isNoteOn[i] = false; 
          noteOff(0, note[i], 127);   // Channel 0, Note, normal velocity  

       }
       }
      oldValues[i]  = newValues[i];                   // Store the new values in the old value array and repeat
   
   }

MidiUSB.flush();                                      // Flush all midi data
}

