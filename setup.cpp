

/*
 * setup.cpp -- part of the BME66Silk project.
 * Setup of SerialUI and menu system
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



/* MySUI
 * Our SerialUI Instance, through which we can send/receive
 * data from users. Actually instantiated here, for global use.
 */
SUI::SerialUI MySUI(9);


/*
 * The container for MyInputs, which
 * holds all the variables for the various inputs.
 * Actually instantiated here, for global use.
 */
MyInputsContainerSt MyInputs;


/* MyTracked
 * A structure to hold the tracked variables, which will 
 * automatically update the druid UI when modified by this 
 * program.
 * Actually instantiated here, for global use.
 */
MyTrackedVarsContainerSt MyTracked;



/* MyViews
 * The container for tracking variable views.
 * It's mostly only declared statically so we 
 * can reserve memory up-front, to keep fragmentation
 * low and so tell how much memory we're eating up...
 */
MyTrackedViewsContainerSt MyViews;







bool SetupSerialUI() {
	MySUI.setName("BME66Silk"); // only useful for BLE
	
	MySUI.setGreeting(F(serial_ui_greeting_str));
	// SerialUI acts just like (is actually a facade for)
	// Serial.  Use _it_, rather than Serial, throughout the
	// program.
	// basic setup of SerialUI:
	MySUI.begin(serial_baud_rate); // serial line open/setup
	// MySUI.setTimeout(serial_readtimeout_ms);   // timeout for reads (in ms), same as for Serial.
	MySUI.setMaxIdleMs(serial_maxidle_ms);    // timeout for user (in ms)
	// how we are marking the "end-of-line" (\n, by default):
	MySUI.setReadTerminator(serial_input_terminator);
	// project UID -- will be used to remember state in Druid...
	MySUI.setUID(SUI_STR("548ddebe1f5340cf8f3e29bff8679ca84whNWGCPYp"));

	// have a "heartbeat" function to hook-up
	MySUI.setUserPresenceHeartbeat(CustomHeartbeatCode); 
	// heartbeat_function_period_ms set in main settings header
	MySUI.setUserPresenceHeartbeatPeriod(heartbeat_function_period_ms);
	
	
	
	// Add variable state tracking 
	
	MySUI.trackState(MyTracked.InputPump);
	
	MySUI.trackState(MyTracked.OutputPump);
	
	MySUI.trackState(MyTracked.ChambFull);
	
	MySUI.trackState(MyTracked.WasteFull);
	
	
	
	// Associate tracked vars with views
	MyViews.Sensor.associate(MyTracked.InputPump);
	MyViews.Sensor.associate(MyTracked.OutputPump);
	MyViews.Sensor.associate(MyTracked.ChambFull);
	MyViews.Sensor.associate(MyTracked.WasteFull);
	
	
	
	// a few error messages we hopefully won't need
	
	SUI_FLASHSTRING CouldntAddItemErr = F("Could not add item?");
	
	// get top level menu, to start adding items
	SerialUI::Menu::Menu * topMenu = MySUI.topLevelMenu();
	if (! topMenu ) {
		// well, that can't be good...
		MySUI.returnError(F("Very badness in sEriALui!1"));
		return false;
	}
	
	
	
	/* *** Silk Dialysis *** */

	
	if( ! topMenu->addText(
		SUI_STR("Automatic Control"),
		NULL)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addRequest(
		MyInputs.SenseConductivity)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addRequest(
		MyInputs.ChangeIntervals)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addCommand(
		SUI_STR("Start Dialysis"),
		SilkDialysis::doStartDialysis)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addText(
		SUI_STR("Manual Control"),
		NULL)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addRequest(
		MyInputs.InputPumpOn)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addRequest(
		MyInputs.OutputPumpOn)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addText(
		SUI_STR("System Status"),
		NULL)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	

	
	if( ! topMenu->addView(
		MyViews.Sensor)) {
		MySUI.returnError(CouldntAddItemErr);
		return false;
	}
	


	
	
	
	return true;
	
}


