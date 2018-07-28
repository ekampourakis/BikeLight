/*
 Name:		BikeLight.ino
 Created:	23-Jul-18 8:25:55 PM
 Author:	Manos
*/

#define DEBUG
#define WelcomeMessage "BikeLight Ready"

#define BatteryPin		A0	// D5 A0		A0
#define LowBeamButton	A3	// D3 A3		A3
#define HighBeamButton	A4	// D2 A1		A4
#define LEDPin			3	// D4 PWM4		D3
#define HighBeamPin		5	// D0 PWM0		D5
#define LowBeamPin		6	// D1 PWM1		D6

#define ButtonLogic false

#define ChargedVoltage 420
#define DischargedVoltage 310
#define CutOffVoltage 290

#define BatteryBlinkInterval 4000
#define BatteryBlinkDelay 500
#define LowBatteryDelay 200
#define MaximumBatteryBlinks 4
#define MenuBlinkDelay 500

#define LowBeamToggleDelay 1000
#define HighBeamToggleDelay 1000
#define MenuToggleDelay 2000

bool LowBeam = false;
bool HighBeam = false;
bool TempHighBeam = false;
bool LowBattery = false;
bool LEDState = false;
bool Menu = false;

bool LastLowBeamState = true;
bool LastHighBeamState = true;

uint8_t BatteryLevel = 0;	// 0-100%
uint8_t BlinksLeft = 0;

unsigned long LastBatteryBlink = 0;
unsigned long LastLowBatteryBlink = 0;
unsigned long LastMenuBlink = 0;
unsigned long LastLowBeamPress = 0;
unsigned long LastHighBeamPress = 0;

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
	#ifdef DEBUG
		Serial.begin(9600);
		Serial.println(WelcomeMessage)
	#endif
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
	#ifdef DEBUG
		Serial.print("Battery pack at\t");
		Serial.print(BatteryVoltage * 2);
		Serial.print("V and at\t");
		Serial.print(BatteryLevel);
		Serial.println("%");
	#endif
}

void BatteryBlink() {
	unsigned long Current = millis();
	if (LowBattery && Current > LastLowBatteryBlink + LowBatteryDelay) {
		CheckBattery();
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
	TempHighBeam = (HighBeamState == ButtonLogic);
	if (HighBeam && HighBeamState == ButtonLogic) {
		HighBeam = false;
		#ifdef DEBUG
			Serial.println("High beams interrupt");
		#endif
	}
	if (HighBeamState != ButtonLogic) {
		if (LowBeamState != LastLowBeamState) {
			LastLowBeamPress = Current;
		}
		if ((Current - LastLowBeamPress) > LowBeamToggleDelay) {
			if (LowBeamState == ButtonLogic) {
				LastLowBeamPress = Current;
				LowBeam = !LowBeam;
				#ifdef DEBUG
					Serial.print("Low beams toggle ( ");
					Serial.println(LowBeam ? "ON )" : "OFF )");
				#endif
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
				#ifdef DEBUG
					Serial.print("High beams toggle ( ");
					Serial.println(HighBeam ? "ON )" : "OFF )");
				#endif
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
	if (TempHighBeam) {
		digitalWrite(HighBeamPin, true);
	} else {
		digitalWrite(HighBeamPin, HighBeam);
	}
	// Somehow brightness control and menu on secret combination
}
