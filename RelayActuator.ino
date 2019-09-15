/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2018 Sensnology AB
 * Full contributor list: https://github.com/mysensors/MySensors/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Example sketch showing how to control physical relays.
 * This example will remember relay state after power failure.
 * http://www.mysensors.org/build/relay
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_RF24
//#define MY_RADIO_NRF5_ESB
//#define MY_RADIO_RFM69
//#define MY_RADIO_RFM95

// Enable repeater functionality for this node
//#define MY_REPEATER_FEATURE
#define MY_NODE_ID 12   // you have to set #define MY_NODE_ID 1 before #include <MySensors.h> ;-)

#include <MySensors.h>

#define RELAY_PIN 4  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define NUMBER_OF_RELAYS 4 // Total number of attached relays
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay

#define CHAVE_PRESSIONADA LOW
#define CHAVE_SOLTA HIGH

#define CHILD_SENSOR_ID1  1

int CHAVE_RELE_1_PIN = 8;
int CHAVE_RELE_2_PIN = A5;

bool CHAVE_RELE_1_LastState = false;
bool CHAVE_RELE_2_LastState = false;

long timePress1 = 0;

// MyMessage msg(CHILD_ID_LIGHT, V_LIGHT_LEVEL);
MyMessage msg1(CHILD_SENSOR_ID1, V_STATUS);

void before()
{
	for (int sensor=1, pin=RELAY_PIN; sensor<=NUMBER_OF_RELAYS; sensor++, pin++) {
		// Then set relay pins in output mode
		pinMode(pin, OUTPUT);
		// Set relay to last known state (using eeprom storage)
		digitalWrite(pin, loadState(sensor)?RELAY_ON:RELAY_OFF);
	}

 
}

void setup()
{
  pinMode(CHAVE_RELE_1_PIN, INPUT_PULLUP);
  pinMode(CHAVE_RELE_2_PIN, INPUT_PULLUP);
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("Relay", "1.0");

	for (int sensor=1, pin=RELAY_PIN; sensor<=NUMBER_OF_RELAYS; sensor++, pin++) {
		// Register all sensors to gw (they will be created as child devices)
		present(sensor, S_BINARY);
	}
}


void loop()
{
  MonitoraChave_1();
   
}


void MonitoraChave_1()
{
  if( digitalRead(CHAVE_RELE_1_PIN) == CHAVE_PRESSIONADA && CHAVE_RELE_1_LastState == false )
  { 
    if(timePress1 == 0)
    {
      timePress1 = millis();          // para debouncing ...
      return;    
    }
  
    if( millis() - timePress1 >= 500) // debouncing ...
    {
      int sensor = 1; // Relé 1
      bool ReleStatus = digitalRead(RELAY_PIN);
    
      digitalWrite(RELAY_PIN, ReleStatus ? RELAY_OFF : RELAY_ON ); // Alterna estado IO
      CHAVE_RELE_1_LastState = true;   

      saveState(sensor, ReleStatus ? RELAY_OFF : RELAY_ON);
    
      send(msg1.set(ReleStatus ? RELAY_OFF : RELAY_ON));
      sendHeartbeat();
        
      Serial.println("Chave 1 Pressionada..."); //void saveState(const uint8_t pos, const uint8_t value);              
    }
  }
  
  if( digitalRead(CHAVE_RELE_1_PIN) == CHAVE_SOLTA && CHAVE_RELE_1_LastState == true )
  {
    timePress1 = 0;
    CHAVE_RELE_1_LastState = false;
    Serial.println("Chave 1 solta...");
    delay(10);
  }
}

void receive(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
	if (message.type==V_STATUS) {
		// Change relay state
		digitalWrite(message.sensor-1+RELAY_PIN, message.getBool()?RELAY_ON:RELAY_OFF);
		// Store state in eeprom
		saveState(message.sensor, message.getBool());
		// Write some debug info
		Serial.print("Incoming change for sensor:");
		Serial.print(message.sensor);
		Serial.print(", New status: ");
		Serial.println(message.getBool());
	}
}
