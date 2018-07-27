/*
 Name:		BikeLight.ino
 Created:	23-Jul-18 8:25:55 PM
 Author:	Manos
*/

#define HighBeamButton A4	// D2 A1		A4
#define LowBeamButton A3	// D3 A3		A3
#define BatteryPin A0		// D5 A0		A0
#define HighBeamPin 5		// D0 PWM0		D5
#define LowBeamPin 6		// D1 PWM1		D6
#define LEDPin 3			// D4 PWM4		D3

#define ChargedVoltage 420
#define DischargedVoltage 310
#define CutOffVoltage 290

#define BatteryBlinkInterval 4000
#define BatteryBlinkDelay 500
#define LowBatteryDelay 200
#define MaximumBatteryBlinks 4
bool LowBattery = false;
uint8_t BatteryLevel = 0;	// 0-100%
unsigned long LastBatteryBlink = 0;
unsigned long LastLowBatteryBlink = 0;
uint8_t BlinksLeft = 0;
bool LEDState = false;
bool Menu = false;
unsigned long LastMenuBlink = 0;
#define MenuBlinkDelay 500

#define ButtonLogic false

bool LowBeam = false;
bool HighBeam = false;

#define LowBeamToggleDelay 1000
#define HighBeamToggleDelay 1000

unsigned long LastLowBeamPress = 0;
unsigned long LastHighBeamPress = 0;

bool LastLowBeamState = true;
bool LastHighBeamState = true;

#define MenuToggleInterval 2000

void PinModes() {
	pinMode(HighBeamButton, INPUT_PULLUP);
	pinMode(LowBeamButton, INPUT_PULLUP);
	pinMode(BatteryPin, INPUT);
	pinMode(HighBeamPin, OUTPUT);
	analogWrite(HighBeamPin, 0);
	pinMode(LowBeamPin, OUTPUT);
	analogWrite(LowBeamPin, 0);
	pinMode(LEDPin, OUTPUT);
	digitalWrite(LEDPin, LOW);
}

void setup() {
	PinModes();
	Serial.begin(9600);
}

void CheckBattery() {
	long BatteryVoltage = map(analogRead(BatteryPin), 0, 1023, 0, 500);
	if (LowBattery && BatteryVoltage > DischargedVoltage) {
		LowBattery = false;
	}
	if (!LowBattery && BatteryVoltage < CutOffVoltage) {
		LowBattery = true;
	}
	BatteryLevel = map(constrain(BatteryVoltage, DischargedVoltage, ChargedVoltage), DischargedVoltage, ChargedVoltage, 0, 100);
	if (BatteryLevel < 0) {
		BatteryLevel = 0;
	} else if (BatteryLevel > 100) {
		BatteryLevel = 100;
	}
}

void BatteryBlink() {
	unsigned long Current = millis();
	if (LowBattery && Current > LastLowBatteryBlink + LowBatteryDelay) {
		LEDState = !LEDState;
		digitalWrite(LEDPin, LEDState);
		LastLowBatteryBlink = Current;
	} else {
		if (BlinksLeft <= 0) {
			if (Current > LastBatteryBlink + BatteryBlinkInterval) {
				CheckBattery();
				BlinksLeft = round(map(BatteryLevel, 0, 100, 10, 40) / 10.0);
				LastBatteryBlink = Current;
			}
		} else {
			if (Current > LastBatteryBlink + BatteryBlinkDelay) {
				if (LEDState) {
					BlinksLeft--;
				}
				LEDState = !LEDState;
				digitalWrite(LEDPin, LEDState);
				LastBatteryBlink = Current;
			}			
		}
	}
}

void MenuBlink() {
	unsigned long Current = millis();
	if (Menu && Current > LastMenuBlink + MenuBlinkDelay) {
		LEDState = !LEDState;
		digitalWrite(LEDPin, LEDState);
		LastMenuBlink = Current;
	}
}

void ProcessButtons() {
	unsigned long Current = millis();

	
	bool LowBeamState = digitalRead(LowBeamButton);
	bool HighBeamState = digitalRead(HighBeamButton);

	if (HighBeamState != ButtonLogic) {
		if (LowBeamState != LastLowBeamState) {
			LastLowBeamPress = Current;
		}
		if ((Current - LastLowBeamPress) > LowBeamToggleDelay) {
			if (LowBeamState == ButtonLogic) {
				LastLowBeamPress = Current;
				LowBeam = !LowBeam;
				Serial.println("Low beam toggle");
			}
		}
		LastLowBeamState = LowBeamState;
	}

	if (LowBeamState != ButtonLogic) {
		if (HighBeamState != LastHighBeamState) {
			LastHighBeamPress = Current;
		}
		if ((Current - LastHighBeamPress) > HighBeamToggleDelay) {
			if (HighBeamState == ButtonLogic) {
				LastHighBeamPress = Current;
				HighBeam = !HighBeam;
				Serial.println("High beam toggle");
			}
		}
		LastHighBeamState = HighBeamState;
	}



}

void loop() {
	if (!Menu) {
		BatteryBlink();
	} else {
		MenuBlink();
	}
	ProcessButtons();
	digitalWrite(HighBeamPin, HighBeam);
	// Async LED blink based on battery level
	// Power off on low battery and fast LED blink

	// High beam output wired to high beam button state
	// High beam lock with 2 sec press
	// High beam unlock with press

	// Low beam state change based on toggle state

	// Somehow brightness control and menu on secret combination
}
