

/*
 * callbacks.cpp -- part of the BME66Silk project.
 * Implementation of callbacks -- YOUR code goes here!
 * Frank Ma
 *  
 * 
 * Copyright (C) 2021 Frank Ma
 * 
 * Generated by DruidBuilder [https://devicedruid.com/], 
 * as part of project "548ddebe1f5340cf8f3e29bff8679ca84whNWGCPYp",
 * aka BME66Silk.
 * 
 * Druid4Arduino, Device Druid, Druid Builder, the builder 
 * code brewery and its wizards, SerialUI and supporting 
 * libraries, as well as the generated parts of this program 
 * are 
 *            Copyright (C) 2013-2019 Pat Deegan 
 * [https://psychogenic.com/ | https://inductive-kickback.com/]
 * and distributed under the terms of their respective licenses.
 * See https://devicedruid.com for details.
 * 
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
 * PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, 
 * YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
 * CORRECTION.
 * 
 * Keep in mind that there is no warranty and you are solely 
 * responsible for the use of all these cool tools.
 * 
 * Play safe, have fun.
 * 
 */

/* we need the SerialUI lib */
#include <SerialUI.h>
#include "BME66SilkSettings.h"

/* our project specific types and functions are here */
#include "BME66Silk.h"

/* 
 * In addition to any custom globals you declared,
 * here you have access to:
 * 
 * *** MySUI -- the SerialUI instance.
 * Use it as you would the Serial device, e.g.
 * 	MySUI.println(F("Helloooo..."));
 * 	
 * 	
 * *** MyInputs -- a container for 
 * values submitted by users. Contents:
 *  
 *    MyInputs.SenseConductivity (Toggle)
 *    MyInputs.ChangeIntervals (String)
 *    MyInputs.InputPumpOn (Toggle)
 *    MyInputs.OutputPumpOn (Toggle)
 *  
 *  
 *  
 * *** MyTracked -- a container for values tracked
 * by druid and displayed to users in "state" pane.  Changes to:
 * 
 *    MyTracked.InputPump (SerialUI::Tracked::Toggle)
 *    MyTracked.OutputPump (SerialUI::Tracked::Toggle)
 *    MyTracked.ChambFull (SerialUI::Tracked::String)
 *    MyTracked.WasteFull (SerialUI::Tracked::String)
 * 
 * will automatically be reported to the UI, on the next refresh/ping.
 * 
 * 
 */

// custom declarations
std::vector<int> changeIntervals;

// "heartbeat" function, called periodically while connected
void CustomHeartbeatCode()
{
	if (digitalRead(CHAMBER_SENSOR_PIN) == LOW)
	{
		MyTracked.ChambFull = "YES";
	}
	else
	{
		MyTracked.ChambFull = "NO";
	}

	if (digitalRead(WASTE_SENSOR_PIN) == LOW)
	{
		MyTracked.WasteFull = "YES";
	}
	else
	{
		MyTracked.WasteFull = "NO";
	}
}

/* ********* callbacks and validation functions ********* */

/* *** Silk Dialysis *** */
namespace SilkDialysis
{

	void SenseConductivityChanged()
	{

		/* Sense Conductivity value was modified.
    * It is a bool accessible in MyInputs.SenseConductivity
    */
		MySUI.print(F("Sense Conductivity is now:"));
		MySUI.println(MyInputs.SenseConductivity ? F("ON") : F("OFF"));
	}

	void ChangeIntervalsChanged()
	{

		/* Change Intervals value was modified.
    * It is a String accessible in MyInputs.ChangeIntervals
    */
		MySUI.print(F("Change Intervals is now:"));
		MySUI.println(MyInputs.ChangeIntervals);
	}

	void doStartDialysis()
	{
		// collect user input
		String rawChangeIntervals = MyInputs.ChangeIntervals;
		std::stringstream ss(rawChangeIntervals.c_str()); // need to convert arduino String to std::string

		while (ss.good())
		{
			std::string substr;
			std::getline(ss, substr, ',');
			if (substr != "")
			{
				changeIntervals.push_back(std::atoi(substr.c_str()));
			}
		}
		// changeIntervals = {5,5,10,10}; // need to be changed

		if (changeIntervals.empty())
			return; // if the user didn't enter anything, don't do anything
		int numChangeIntervals = changeIntervals.size();
		MySUI.print(F("Total Intervals: "));
		MySUI.println(numChangeIntervals);
		int numRemainingChangeIntervals = numChangeIntervals;

		// first fill the dialysis chamber
		MySUI.println(F("Starting initial filling of dialysis chamber"));
		digitalWrite(INPUT_RELAY_PIN, HIGH);
		MyTracked.InputPump = true;
		while (digitalRead(CHAMBER_SENSOR_PIN) == HIGH)
		{
		} // HIGH means that the sensor has not yet been lifted (LOW means lifted)
		digitalWrite(INPUT_RELAY_PIN, LOW);
		MyTracked.InputPump = false;

		uint32_t intervalStartMillis = millis();
		while (numRemainingChangeIntervals > 0)
		{
			int interval = changeIntervals[numChangeIntervals - numRemainingChangeIntervals];

			uint32_t currMillis = millis();
			// execute the water change code only if we have completed the interval
			if (currMillis - intervalStartMillis >= interval * 1000)
			{ // change conversion factor for real use
				MySUI.print(F("Interval of "));
				MySUI.print(interval);
				MySUI.println(F(" is reached. Starting water change."));

				uint32_t startDrainMillis = millis();
				digitalWrite(OUTPUT_RELAY_PIN, HIGH);
				MyTracked.OutputPump = true;
				while (digitalRead(WASTE_SENSOR_PIN) == HIGH && (millis() - startDrainMillis < 10000))
				{
				} // fill until either time is reached or waste censor pin is lifted
				digitalWrite(OUTPUT_RELAY_PIN, LOW);
				MyTracked.OutputPump = false;

				digitalWrite(INPUT_RELAY_PIN, HIGH);
				MyTracked.InputPump = true;
				while (digitalRead(CHAMBER_SENSOR_PIN) == HIGH)
				{
				} // fill until chamber sensor has been lifted
				digitalWrite(INPUT_RELAY_PIN, LOW);
				MyTracked.InputPump = false;

				MySUI.println(F("Water change completed."));
				numRemainingChangeIntervals--;
				intervalStartMillis = millis();
			}
		}
	}

	void InputPumpOnChanged()
	{
		if (MyInputs.InputPumpOn)
		{
			digitalWrite(INPUT_RELAY_PIN, HIGH);
			MySUI.println(F("Input pump is turned on."));
			MyTracked.InputPump = true;
		}
		else
		{
			digitalWrite(INPUT_RELAY_PIN, LOW);
			MySUI.println(F("Input pump is turned off."));
			MyTracked.InputPump = false;
		}
	}

	void OutputPumpOnChanged()
	{
		if (MyInputs.OutputPumpOn)
		{
			digitalWrite(OUTPUT_RELAY_PIN, HIGH);
			MySUI.println(F("Output pump is turned on."));
			MyTracked.OutputPump = true;
		}
		else
		{
			digitalWrite(OUTPUT_RELAY_PIN, LOW);
			MySUI.println(F("Output pump is turned off."));
			MyTracked.OutputPump = false;
		}
	}

} /* namespace SilkDialysis */
