/*
   This is a wireless joystick made for DTank.
  It uses an Arduino joystick shield and other 2 jogs to command all and to communicate it's installed ad xbee s2 on Serial1 port.

*/

#define xJog1 A1
#define yJog1 A0

#define xJog2 A8
#define yJog2 A9

#define xJog3 A10
#define yJog3 A11

#define aJoy 7
#define bJoy 6
#define cJoy 5
#define dJoy 4
#define eJoy 3
#define fJoy 8
#define gJoy 9

static int xJog1Value = 0;
static int yJog1Value = 0;
static int xJog2Value = 0;
static int yJog2Value = 0;
static int xJog3Value = 0;
static int yJog3Value = 0;
static int aJoyValue = 0;
static int bJoyValue = 0;
static int cJoyValue = 0;
static int dJoyValue = 0;
static int eJoyValue = 0;
static int fJoyValue = 0;
static int gJoyValue = 0;
static bool debug = false;

const int nofKeys =  13;

int joystick[nofKeys] = {xJog1Value, yJog1Value,
                         xJog2Value, yJog2Value,
                         xJog3Value, yJog3Value,
                         aJoyValue, bJoyValue,
                         cJoyValue, dJoyValue,
                         eJoyValue, fJoyValue,
                         gJoyValue
                        };


int _send[6] = {  0, 0,
                  0, 0, 0, 0
               };

bool toSend[nofKeys] = { false, false, false, false, false,
                         false, false, false, false, false,
                         false, false
                       };


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("initialize wireless Joystick");
  // delay(1000);

  //setup joystick digital buttons
  pinMode(aJoy, INPUT);
  pinMode(bJoy, INPUT);
  pinMode(cJoy, INPUT);
  pinMode(dJoy, INPUT);
  pinMode(eJoy, INPUT);
  pinMode(fJoy, INPUT);
  pinMode(gJoy, INPUT);

  // delay(1000);
  Serial.println();
  Serial.println("Initialization done!");
  Serial.println();
  Serial.println("Hit \"D\" if you wanto to run debug \nIMPORTANT!!! If you run joystick in debug mode it shuld lose some packets!");
  Serial.println();
  Serial.println();
}

void loop() {
  if (Serial.read() == 'D') {
    debug = !debug;
    if (debug) {
      Serial.println("Debug mode start!");
      Serial.println();
    }
    else {
      Serial.println("Debug mode stop!");
      Serial.println();
    }
  }

  readJoystickValues();

  if (debug) {
    Serial.print("Values -> ");
  }

  for (int i = 0; i < nofKeys; i++) {
    if (debug) {
      Serial.print( i );
      Serial.print( " -> ");
      Serial.print( joystick[i] );
      Serial.print( " ");
    }
    if (toSend[i]) {
      if (i < 10) {
        _send[0] = 0;
        _send[1] = i;
      }
      else {
        _send[0] = 1;
        _send[1] = i % 10;
      }
      if (joystick[i] >= 0) {
        _send[2] = 0;
        manageData(joystick[i]);

      }
      if (joystick[i] < 0) {
        _send[2] = 1;
        int adaptedValue = -joystick[i];
        manageData(adaptedValue);
      }

      String value = "";
      for (int i = 0; i < 6; ++i) {
        value += _send[i];
      }
      value += ";";
      Serial1.print(value);
      toSend[i] = false;
      delay(5);
    }
  }
  if (debug)
    Serial.println();
}


void manageData(int& value) {
  if (value <= 255 && value >= 100 ) {
    _send[3] = value / 100;
    _send[4] = (value % 100) / 10;
    _send[5] = value % 10;

  }

  if (value < 100 && value >= 10) {
    _send[3] = 0;
    _send[4] = value / 10;
    _send[5] = value % 10;
  }

  if (value < 10) {
    _send[3] = 0;
    _send[4] = 0;
    _send[5] = value;
  }
}

void readJoystickValues() {
  xJog1Value = normalizeNumber((analogRead(xJog1) - 485));
  yJog1Value = normalizeNumber((analogRead(yJog1) - 485));
  xJog2Value = normalizeNumber((analogRead(xJog2) - 502));
  yJog2Value = -normalizeNumber((analogRead(yJog2) - 490));
  xJog3Value = normalizeNumber((analogRead(xJog3) - 507));
  yJog3Value = -normalizeNumber((analogRead(yJog3) - 499));

  aJoyValue = !digitalRead(aJoy);
  bJoyValue = !digitalRead(bJoy);
  cJoyValue = !digitalRead(cJoy);
  dJoyValue = !digitalRead(dJoy);
  eJoyValue = !digitalRead(eJoy);
  fJoyValue = !digitalRead(fJoy);
  gJoyValue = !digitalRead(gJoy);

  if (xJog1Value != joystick[0]) {
    joystick[0] = xJog1Value;
    toSend[0] = true;
  }

  if (yJog1Value != joystick[1]) {
    joystick[1] = yJog1Value;
    toSend[1] = true;
  }

  if (xJog2Value !=  joystick[2]) {
    joystick[2] = xJog2Value;
    toSend[2] = true;
  }

  if (yJog2Value != joystick[3]) {
    joystick[3] = yJog2Value;
    toSend[3] = true;
  }

  if (xJog3Value !=  joystick[4]) {
    joystick[4] = xJog3Value;
    toSend[4] = true;
  }

  if (yJog3Value !=  joystick[5]) {
    joystick[5] = yJog3Value;
    toSend[5] = true;
  }

  if (aJoyValue !=  joystick[6]) {
    joystick[6] = aJoyValue;
    toSend[6] = true;
  }

  if (bJoyValue !=  joystick[7]) {
    joystick[7] = bJoyValue;
    toSend[7] = true;
  }

  if (cJoyValue !=  joystick[8]) {
    joystick[8] = cJoyValue;
    toSend[8] = true;
  }

  if (dJoyValue !=  joystick[9]) {
    joystick[9] = dJoyValue;
    toSend[9] = true;
  }

  if (eJoyValue !=  joystick[10]) {
    joystick[10] = eJoyValue;
    toSend[10] = true;
  }

  if (fJoyValue !=  joystick[11]) {
    joystick[11] = fJoyValue;
    toSend[11] = true;
  }


  if (gJoyValue !=  joystick[12]) {
    joystick[12] = gJoyValue;
    toSend[12] = true;
  }

}

int normalizeNumber(int numberToNormalize) {
  int toReturn = numberToNormalize / 2;
  if (toReturn >= -15 && toReturn <= 15)
    return 0;
  if (toReturn >= 244 && toReturn <= 255)
    return 255;
  if (toReturn > 255)
    return 255;
  if (toReturn < -255)
    return -255;
  return toReturn;
}
