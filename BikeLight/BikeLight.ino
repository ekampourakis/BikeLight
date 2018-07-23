/*
 Name:		BikeLight.ino
 Created:	23-Jul-18 8:25:55 PM
 Author:	Manos
*/

#define HighBeamButton 2	// D2 A1
#define LowBeamButton 3		// D3 A3
#define BatteryPin 5		// D5 A0
#define HighBeamPin 0	// D0 PWM0
#define LowBeamPin 1	// D1 PWM1
#define LEDPin 4		// D4 PWM4

void PinModes() {
	pinMode(HighBeamButton, INPUT);
	digitalWrite(HighBeamButton, HIGH);
	pinMode(LowBeamButton, INPUT);
	digitalWrite(LowBeamButton, HIGH);
	pinMode(BatteryPin, INPUT);
	pinMode(HighBeamPin, OUTPUT);
	analogWrite(HighBeamPin, 0);
	pinMode(LowBeamPin, OUTPUT);
	analogWrite(LowBeamPin, 0);
	pinMode(LEDPin, OUTPUT);
	digitalWrite(LEDPin, LOW);
}

void setup() {
	// Setup pins
	PinModes();
}

byte ReadBattery() {
	return 0;
}

void loop() {
	// Async LED blink based on battery level
	// Power off on low battery and fast LED blink

	// High beam output wired to high beam button state
	// High beam lock with 2 sec press
	// High beam unlock with press

	// Low beam state change based on toggle state

	// Somehow brightness control and menu on secret combination
}
