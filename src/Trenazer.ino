#include "Trenazer.h"

int b;
int lasti;

int bpm;
unsigned long lastDataTime;
int arrow;

boolean direction1;
boolean direction2;
boolean direction3;

boolean adjustStart;

// calories counting related variables
bool isMale; 
int weight; 
int trainingTime; // seconds
int caloriesForLastTraining; 
// ---

int countCalories()
{
	int res = KCAL_RATIO * (weight * trainingTime/60);
	return res;
}

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Setting up...");
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Trenajer v1");

  isMale = true;
  weight = 76;

  pinMode(reg1_pin, OUTPUT);
  pinMode(reg2_pin, OUTPUT);
  pinMode(reg3_pin, OUTPUT);
  pinMode(r1_pin, OUTPUT);
  pinMode(r2_pin, OUTPUT);
  pinMode(r3_pin, OUTPUT);
  pinMode(r4_pin, OUTPUT);
  pinMode(r5_pin, OUTPUT);

  pinMode(BUTTONS_PIN, INPUT);
  pinMode(biper_pin, OUTPUT);
  pinMode(timepot_pin, INPUT);
  pinMode(speedpot_pin, INPUT);
  pinMode(relay1_pin, OUTPUT);
  pinMode(relay2_pin, OUTPUT);
  pinMode(relay3_pin, OUTPUT);
  //leds
  pinMode(ledstart_pin, OUTPUT);
  pinMode(ledstop_pin, OUTPUT);
  //led panel
  pinMode(l1_pin, OUTPUT);
  pinMode(l2_pin, OUTPUT);
  pinMode(l3_pin, OUTPUT);
  pinMode(l4_pin, OUTPUT);
  pinMode(l5_pin, OUTPUT);
  pinMode(l6_pin, OUTPUT);
  pinMode(l7_pin, OUTPUT);
  pinMode(l8_pin, OUTPUT);
  pinMode(l9_pin, OUTPUT);
  pinMode(l10_pin, OUTPUT);

  pinMode(HITSENSOR, INPUT);

  delay(2000);

  lcd.setCursor(0, 1);
  lcd.print("Preparing Bluetooth");

  delay(1500);
  lcd.setCursor(0, 2);
  lcd.print(".  ");
  delay(1500);
  lcd.setCursor(0, 2);
  lcd.print(".. ");
  delay(1500);
  lcd.setCursor(0, 2);
  lcd.print("...");
  delay(1500);
  lcd.setCursor(0, 2);
  lcd.print(".  ");
  delay(1500);
  lcd.setCursor(0, 2);
  lcd.print(".. ");
  delay(1500);

  for(int i=0; i<historyElements; i++){
	    bpmHistory[i] = 0;
	    hitHistory[i] = 0;
	}

  isTraining = false;
  tmin = 20;
  tsec = 0;
  selectMode2();
  lasti = 0;
  direction1 = true;
  prevPunchTime = 0;
  speedwold = 0;

  disp.clear();
  disp.brightness(7);

 }

int theTime()
{
	int t = tmin * 100 + tsec;
	return t;
}

int buttonRead()
{
	buttonPress = analogRead(BUTTONS_PIN);

	if (buttonPress > 190 && buttonPress <= 200) return 1; 
	if (buttonPress > 400 && buttonPress <= 420) return 2; 
	if (buttonPress > 580 && buttonPress <= 610) return 3; 
	if (buttonPress > 820 && buttonPress <= 840) return 4; 
	if (buttonPress > 400 && buttonPress <= 500) return 5; 
	if (buttonPress > 900 && buttonPress <= 1024) return 6; 
	else return 0;
}

void loop()
{
	speedw = ((map(analogRead(14), 0, 1023, SPEED_MIN, SPEED_MAX)) / 10) * 10;
	ledLevel = map(analogRead(14), 0, 1023, 0, 10);
	timew  = map(analogRead(15), 0, 1023, TIME_MIN, TIME_MAX);
	if (mode >= 2) {tmin = timew * TIME_COEF;} else {tmin = 10;} 
	if (!isTraining) {tsec = 0;}
	recvWithStartEndMarkers();
	if (newData)  {   processCommand();  }  
  	if (millis() - lastDataTime >= 3000) { bpm = 0; }
	b = buttonRead();
	switch (b) {
		case 1:
			selectMode1();
      break;
		case 2:
			selectMode2();
      break;
		case 3:
			selectMode4();
      break;
		case 4:
			selectMode3();
      break;
		case 5:
			selectMode5();
      break;
		case 6:
			timeMark = millis();
			if (timeMark - prevTimeMarkButtonLag >= 1000) training();
      break;
		default:
      break;
	}

	if (speedw != speedwold) {
		speedwold = speedw;
		setLedLevel(ledLevel);
		updateDisplay();
	}

	if (timew != timewold) {
		timewold = timew;
		updateDisplay();
	}
}

void setLedLevel(int i)
{
	for (int thisPin = 0; thisPin < pinCount; thisPin++) 
	{
		digitalWrite(ledPins[thisPin], LOW);

	}
	for (int thisPin = pinCount; thisPin >= i; thisPin--) 
	{
		digitalWrite(ledPins[thisPin], HIGH);
	}
}

void setIndicator(int i)
{
	for (int thisPin = 0; thisPin < 5; thisPin++) 
	{
		digitalWrite(indPins[thisPin], LOW);

	}

	switch(i) {
		case 1:
			digitalWrite(r1_pin, HIGH);
			break;
		case 2:
			digitalWrite(r2_pin, HIGH);
			break;
		case 3:
			digitalWrite(r3_pin, HIGH);
			break;
		case 4:
			digitalWrite(r4_pin, HIGH);
			break;
		case 5:
			if (direction1) digitalWrite(r5_pin, HIGH);
			if (!direction1) digitalWrite(r5_pin, LOW);
			break;
		default:
			break;
	}
}

void showResults()
{
	countCalories();
	while(1)
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		if (preliminaryLastFinish) lcd.print("Training interrupted");
		else lcd.print("Training finished");
		lcd.setCursor(0, 1);
		lcd.print("KCAL burnt: ");
		lcd.setCursor(12, 1);
		lcd.print(caloriesForLastTraining);
		if (buttonRead() > 0) break;
	}
}

void updateDisplay() 
{
	if (mode == 1)
	{
    	lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("       Warm up");
		lcd.setCursor(0, 1);
		if (!isTraining) lcd.print("10 min");
		if (isTraining) 
		{
			lcd.print(String(tmin) + "min " + String(tsec) + "s");
			disp.displayClock(tmin,tsec);
			disp.point(!(tsec % 2));
		}
	}
	if (mode == 2)
	{
    	lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("      Training");
		lcd.setCursor(0, 1);
		lcd.print(String(tmin) + "min " + String(tsec) + "s, "+ String(speedw) + "ms");
		disp.displayClock(tmin,tsec);
		disp.point(!(tsec % 2));
	}
	if (mode == 3)
	{
    	lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("   Smart Training");
		lcd.setCursor(0, 1);
		lcd.print(String(tmin) + "min " + String(tsec) + "s, "+ String(speedw) + "ms");
		disp.displayClock(tmin,tsec);
		disp.point(!(tsec % 2));
	}
	if (mode == 4)
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("    Sync punches");
		lcd.setCursor(0, 1);
		if (!isTraining) lcd.print(String(tmin) + "min " + String(tsec) + "s");
		if (isTraining) lcd.print(String(tmin) + "min " + String(tsec) + "s");
		disp.displayClock(tmin,tsec);
		disp.point(!(tsec % 2));
	}
	if (mode == 5)
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("   Chaotic punches");
		lcd.setCursor(0, 1);
		lcd.print(String(tmin) + "min " + String(tsec) + "s");
		disp.displayClock(tmin,tsec);
		disp.point(!(tsec % 2));
	}
	updateBPM();
	updateSmartLine(arrow);
}

void updateBPM()
{
	lcd.setCursor(0, 3);
	lcd.print("BPM " + String(bpm));
	lcd.setCursor(4, 3);
	lcd.print("        ");
	lcd.setCursor(4, 3);
	lcd.print(String(bpm));
}

void training()
{
	isTraining = true;
	preliminaryLastFinish = false;
	lastPunchNumber = 0;
	Serial.println("Nachata trenirovka");
	prevTimeMarkButtonLag = millis();

	trainingTime = 0;

	while (tmin > 0 || tsec > 0) 
	{
		timeMark = millis();

		if (speedw != speedwold) {
		speedwold = speedw;
		ledLevel = map(speedw, 0, speedLLimit, 0, 9);
		setLedLevel(ledLevel);
		}

		if (buttonRead() == 6 && timeMark - prevTimeMarkButtonLag >= 1000) 
		{
			preliminaryLastFinish = true;
			break;
		}

		if(timeMark - prevTimeMark >= 1000) 
		{
			prevTimeMark = timeMark;
			tick();
		}
		if(timeMark - prevPunchTime >= speedw) 
		{
			nextPunch();
		}


		if (mode == 3)
		{
			hitSensorData = digitalRead(HITSENSOR);

			if(adjustFinished)
			{
				if (!encreasePunchSpeedNeeded & (hitHistory[historyElements - 1] < hitAverage - hitBounds)) {encreasePunchSpeedNeeded = true; decTime = millis(); Serial.println("ENC NEEDED");}
				else if (!decreasePunchSpeedNeeded & (hitHistory[historyElements - 1] > hitAverage + hitBounds)) {decreasePunchSpeedNeeded = true; decTime = millis(); Serial.println("DEC NEEDED");}
			}

			if(encreasePunchSpeedNeeded) arrow = 1;
			if (decreasePunchSpeedNeeded) arrow = 2;
			if (!decreasePunchSpeedNeeded & !encreasePunchSpeedNeeded) arrow = 0;
		
			if(!hitSensorData & isTraining)
			{
				int t = millis() - lasthit;
				if (t >= 400)
				{
					lasthit = millis();
					hitcount++;
					Serial.print("HIT "); Serial.print(hitcount); Serial.print(" times\n");
					pushHitHistory(t);

					for(int i=0; i<historyElements; i++)
					{
					    Serial.print(hitHistory[i]);
					    Serial.print("\t\t");
					}
					Serial.print("S");
					Serial.print(speedw);
					Serial.print("\n");

				}
			}
	
			checkForDecrease();
		}


		
	}

	Serial.println("Zakonchena trenirovka");
	isTraining = false;


	adjustFinished = false;
	encreasePunchSpeedNeeded = false;
	decreasePunchSpeedNeeded = false;
	for(int i = 0; i < historyElements - 1; i++)
	{
	    hitHistory[i] = 0;
	}

	prevTimeMarkButtonLag = millis();
	showResults();

}

void selectMode1()
{
	mode = 1;
	tsec = 0;
	Serial.println("Rezim: Razminka");
  	Serial.println("Vremja: " + String(tmin) + ":" + String(tsec) + "speed " + String(speedw));
  	setIndicator(1);
  	updateDisplay();
}

void selectMode2()
{
	mode = 2;
	tsec = 0;
	Serial.println("Rezim: Trenirovka");
  	Serial.println("Vremja: " + String(tmin) + ":" + String(tsec) + "speed " + String(speedw));
  	setIndicator(2);
  	updateDisplay();
}

void selectMode3()
{
  mode = 3;
  tsec = 0;
  Serial.println("Rezim: Umnaja Trenirovka");
  Serial.println("Vremja: " + String(tmin) + ":" + String(tsec) + "speed " + String(speedw));
  setIndicator(3);
  updateDisplay();
}

void selectMode4()
{
  mode = 4;
  tsec = 0;
  Serial.println("Rezim: Sinhronnyje udary");
  Serial.println("Vremja: " + String(tmin) + ":" + String(tsec) + "speed " + String(speedw));
  setIndicator(4);
  updateDisplay();
}

void selectMode5()
{
  mode = 5;
  tsec = 0;
  Serial.println("Rezim: Haoticnyje udary");
  Serial.println("Vremja: " + String(tmin) + ":" + String(tsec) + "speed " + String(speedw));
  updateDisplay();
}

void updateSmartLine(int n)
{
	if (n == 0)
	{
		lcd.setCursor(0, 2);
		lcd.print("                    ");
	}
	if (n == 1)
	{
		lcd.setCursor(0, 2);
		lcd.print("        fast    ->->");
	}
	if (n == 2)
	{
		lcd.setCursor(0, 2);
		lcd.print("<-<-    slow        ");
	}
}

void nextPunch()
{
		if (mode == 5) // Haotichnyje
		{
			if (direction1 == true) {punch(1);} else {release(1);}
			speedw = random(1000, 2000);
		}

		if (mode == 2) // Trenirovka
		{
			if (direction1 == true) {punch(1);} else {release(1);}
		}
		if (mode == 1) // Razminka
		{
			if (direction1 == true) {punch(1);} else {release(1);}
			speedw = 2500 - 5 * lastPunchNumber; 
			if (speedw < 500) speedw = 500; 
		}
		if (mode == 4) // Sinhronnyje
		{
			if (direction1 == true) {punch(1);} else {release(1);}
			speedw = 2500 - 5 * lastPunchNumber; 
			if (speedw < SPEED_MIN) speedw = SPEED_MIN; 
		}
		if (mode == 3) // Umnyje
		{
			if (direction1 == true) {punch(1);} else {release(1);}
		}
	
}

void punch(int i) 
{
	lasti = i;
	if (i == 1) {
		digitalWrite(relay1_pin, HIGH);
		digitalWrite(r5_pin, HIGH);
		Serial.println("Punch 1");
	}
	lastPunchNumber++;
	direction1 = false;
	prevPunchTime = millis();
	Serial.print(" direction1 = false\n");
}

void release(int i)
{
	if (i == 1) {
		digitalWrite(relay1_pin, LOW);
		digitalWrite(r5_pin, LOW);
		Serial.println("Release 1");
	}
	direction1 = true;
	Serial.print(" direction1 = true\n");
	prevPunchTime = millis();
}

void tick()
{
	trainingTime += 1;
	if (tsec == 0) 
	{
		tsec = 59;
		if (tmin>0) tmin--;
	}
	else {tsec--;}
  	updateDisplay();
  	Serial.println(String(tmin) + ":" + String(tsec));
}

void checkForDecrease()
{
	int min, max;
	unsigned long sum = 0;

	min = hitHistory[0]; max = hitHistory[0];
	for (int i = 0; i<historyElements; i++)
	{
		if (hitHistory[i] < min) min = hitHistory[i];
		if (hitHistory[i] > max) max = hitHistory[i];
		sum += hitHistory[i];
	}

	if (!adjustStart & (hitHistory[historyElements-1] != 0))
	{
		adjustStart = true;
		hitHistory[historyElements-1] = 0;
		Serial.println("ADJUST STARTED");
	}

	if (adjustStart & !adjustFinished & (hitHistory[0] > 0)) 
	{
		adjustFinished = true;
		hitAverage = sum / historyElements;
		Serial.println("HIT AVERAGE: " + String(hitAverage));
	}

	if(decreasePunchSpeedNeeded)
	{
		if(millis() - decTime >= 2000)
		{
			if (speedw < speedLLimit) 
			{
				speedw +=100; 
				decreasePunchSpeedNeeded = false; 
				Serial.println("DECREASING HIT FREQUENCY");
			}
		}
		
	}
	if(encreasePunchSpeedNeeded)
	{
		if(millis() - decTime >= 2000)
		{
			if (speedw > speedHLimit) 
			{
				speedw -=100; 
				encreasePunchSpeedNeeded = false; 
				Serial.println("ENCREASING HIT FREQUENCY");
			}
		}
		
	}
}

void pushHitHistory(int num)
{
	for(int i = 0; i < historyElements - 1; i++)
	{
	    hitHistory[i] = hitHistory[i + 1];
	}
	hitHistory[historyElements - 1] = num;
}

void recvWithStartEndMarkers() 
{
     static boolean recvInProgress = false;
     static byte ndx = 0;
     char startMarker = '<';
     char endMarker = '>';
 
     if (Serial1.available() > 0) 
     {
          char rc = Serial1.read();
          if (recvInProgress == true) 
          {
               if (rc != endMarker) 
               {
                    if (ndx < maxDataLength) { receivedChars[ndx] = rc; ndx++;  }
               }
               else 
               {
                     receivedChars[ndx] = '\0';
                     recvInProgress = false;
                     ndx = 0;
                     newData = true;
               }
          }
          else if (rc == startMarker) { recvInProgress = true; }
          
     }
 
}

void processCommand()
{
    newData = false;
    bpm = parseInt(receivedChars);
    
    lastDataTime = millis();
    updateBPM();
}

int parseInt(char* chars)
{
    int sum = 0;
    int len = strlen(chars);
    for (int x = 0; x < len; x++)
    {
        int n = chars[len - (x + 1)] - '0';
        sum = sum + powInt(n, x);
    }
    return sum;
}

int powInt(int x, int y)
{
    for (int i = 0; i < y; i++)
    {
        x *= 10;
    }
    return x;
}
