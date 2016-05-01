/*
   Tank control by Davide Larosa
   This tank has a base moved by 4 DC motors and an arm moved by 6 Servo motrs. In the top of the hand there is a tongs moved by another Servo motor.
   All the DC motors are actived by a DK Electronics motors shield powered out by 2 voltage regulator that provides 12 VDC 5A to the shield.
  <   The other motors are powered up by 3 other same voltage regulator.
   Finally there is a joystick shield to drive that all.

   To power up all the motors and the arduino Mega too in installed a power supply that provide 5VDC 2A (dedicated to Arduino) and 12VDC 2A (dedicated to DC motors).
*/


#include <AFMotor.h>
#include <Servo.h>

//instantiate 4 AF_DCMotor thath defines the DC motors
static AF_DCMotor rearLeftMotor(1); //near to arduino power plug
static AF_DCMotor rearRightMotor(2);
static AF_DCMotor frontRightMotor(3);
static AF_DCMotor frontLeftMotor(4);

//servos definition
Servo base;
Servo armLifter;
Servo armExtender;
Servo forearm;
Servo handLifter;
Servo handRotator;

//joystick pinouts definition
#define xJog1 A8
#define yJog1 A9

#define xJog2 A10
#define yJog2 A11

#define xJog3 A12
#define yJog3 A13

#define aJoy 47
#define bJoy 48
#define cJoy 49
#define dJoy 50
#define eJoy 51
#define fJoy 52
#define gJoy 53
//#define centerButton 22
//#define buttonJog2 23
//#define buttonJog3 24

static int keyboardValue = 0; //value that override movements by keyboard
static char serialInput = '\b'; //input value from serial

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
//static int centerButtonValue = 0;
//static int buttonJog2Value = 0;
//static int buttonJog3Value = 0;

const int nofKeys =  14;
int joystick[nofKeys] = {0, xJog1Value, yJog1Value,
                         xJog2Value, yJog2Value,
                         xJog3Value, yJog3Value,
                         aJoyValue, bJoyValue,
                         cJoyValue, dJoyValue,
                         eJoyValue, fJoyValue,
                         gJoyValue
                         //, centerButtonValue,
                         // buttonJog2Value, buttonJog3Value,
                        };

bool debug = false;

const static int baseInitialPosition = 180;
const static int armLifterInitialPosition = 130;
const static int armExtenderInitialPosition = 20;
const static int forearmInitialPosition = 90;
const static int handLifterInitialPosition = 90;
const static int handRotatorInitialPosition = 180;

void setup() {
  delay(100);
  Serial.begin(9600); //start monitor communicaiton
  Serial.println("Start setup tank...");

  stopAllMotors();

  //setup joystick digital buttons
  pinMode(aJoy, INPUT);
  pinMode(bJoy, INPUT);
  pinMode(cJoy, INPUT);
  pinMode(dJoy, INPUT);
  pinMode(eJoy, INPUT);
  pinMode(fJoy, INPUT);
  pinMode(gJoy, INPUT);
  //pinMode(centerButton, INPUT);
  //pinMode(buttonJog2Value, INPUT);
  //pinMode(buttonJog3Value, INPUT);

  //setup servos
  base.attach(22);
  armLifter.attach(23);
  armExtender.attach(24);
  forearm.attach(25);
  handLifter.attach(26);
  handRotator.attach(27);

  //move servos to initial position
  Serial.println("Please wait while resetting servos...");
  resetServos(baseInitialPosition, armLifterInitialPosition, armExtenderInitialPosition, forearmInitialPosition, handLifterInitialPosition, handRotatorInitialPosition);

  delay(1000);
  Serial.println("Setup done!");
  /*
      Serial.println("                3                ");
      delay(1000);
      Serial.println("                2                ");
      delay(1000);
      Serial.println("                1                ");
      delay(1000);
      Serial.println("------------> GO!!! <------------");
  */
  Serial.println("Hit \"D\" if you wanto to run in debug mode!");
  Serial.println("IMPORTANT!!!");
  Serial.println("Running in debug mode everything will be too much slower!");
  Serial.println("To gain the normal speed exit from debug mode (Hit \"D\")");
}


void loop() {
  readKeyboardValues();

  //override commands by keyboard
  if (keyboardValue != 0) {
    moveFront(keyboardValue, keyboardValue);
  }
  else {

    readJoystickValues();

    //joystick center position
    if ( !xIsMoveing() && !yIsMoveing()) {
      stopAllMotors();
    }
    if (yIsMoveing() && !xIsMoveing() ) {
      if (yJog1Value > 1) {
        rearLeftMotor.setSpeed(yJog1Value);
        rearLeftMotor.run(FORWARD);

        frontLeftMotor.setSpeed(yJog1Value);
        frontLeftMotor.run(BACKWARD);

        rearRightMotor.setSpeed(yJog1Value);
        rearRightMotor.run(BACKWARD);

        frontRightMotor.setSpeed(yJog1Value);
        frontRightMotor.run(FORWARD);
      }
      else if (yJog1Value < -1) {
        rearLeftMotor.setSpeed(-yJog1Value);
        rearLeftMotor.run(BACKWARD);

        frontLeftMotor.setSpeed(-yJog1Value);
        frontLeftMotor.run(FORWARD);

        rearRightMotor.setSpeed(-yJog1Value);
        rearRightMotor.run(FORWARD);

        frontRightMotor.setSpeed(-yJog1Value);
        frontRightMotor.run(BACKWARD);
      }
    }
    else if (!yIsMoveing() && xIsMoveing()) {
      if (xJog1Value > 1) {
        rearLeftMotor.setSpeed(xJog1Value);
        rearLeftMotor.run(BACKWARD);

        rearRightMotor.setSpeed(xJog1Value);
        rearRightMotor.run(BACKWARD);

        frontLeftMotor.setSpeed(xJog1Value);
        frontLeftMotor.run(FORWARD);

        frontRightMotor.setSpeed(xJog1Value);
        frontRightMotor.run(FORWARD);
      }
      if (xJog1Value < -1) {
        rearLeftMotor.setSpeed(-xJog1Value);
        rearLeftMotor.run(FORWARD);

        rearRightMotor.setSpeed(-xJog1Value);
        rearRightMotor.run(FORWARD);

        frontLeftMotor.setSpeed(-xJog1Value);
        frontLeftMotor.run(BACKWARD);

        frontRightMotor.setSpeed(-xJog1Value);
        frontRightMotor.run(BACKWARD);
      }
    }
    else if (yIsMoveing() && xIsMoveing()) {
      if (xJog1Value > 1) {
        if (yJog1Value > 1) {
          rearLeftMotor.setSpeed(yJog1Value / (512 / xJog1Value) * 2);
          rearLeftMotor.run(FORWARD);

          frontLeftMotor.setSpeed(yJog1Value / (512 / xJog1Value) * 2);
          frontLeftMotor.run(BACKWARD);

          rearRightMotor.setSpeed(yJog1Value);
          rearRightMotor.run(BACKWARD);

          frontRightMotor.setSpeed(yJog1Value);
          frontRightMotor.run(FORWARD);
        }
        if (yJog1Value < -1) {
          rearLeftMotor.setSpeed(-yJog1Value / (512 / xJog1Value) * 2);
          rearLeftMotor.run(BACKWARD);

          frontLeftMotor.setSpeed(-yJog1Value / (512 / xJog1Value) * 2);
          frontLeftMotor.run(FORWARD);

          rearRightMotor.setSpeed(-yJog1Value);
          rearRightMotor.run(FORWARD);

          frontRightMotor.setSpeed(-yJog1Value);
          frontRightMotor.run(BACKWARD);
        }
      }
      if (xJog1Value < -1) {
        if (yJog1Value > 1) {
          rearLeftMotor.setSpeed(yJog1Value);
          rearLeftMotor.run(FORWARD);

          frontLeftMotor.setSpeed(yJog1Value);
          frontLeftMotor.run(BACKWARD);

          rearRightMotor.setSpeed(yJog1Value / (512 / -xJog1Value) * 2);
          rearRightMotor.run(BACKWARD);

          frontRightMotor.setSpeed(yJog1Value / (512 / -xJog1Value) * 2);
          frontRightMotor.run(FORWARD);
        }
        if (yJog1Value < -1) {
          rearLeftMotor.setSpeed(-yJog1Value);
          rearLeftMotor.run(BACKWARD);

          frontLeftMotor.setSpeed(-yJog1Value);
          frontLeftMotor.run(FORWARD);

          rearRightMotor.setSpeed(-yJog1Value / (512 / -xJog1Value) * 2);
          rearRightMotor.run(FORWARD);

          frontRightMotor.setSpeed(-yJog1Value / (512 / -xJog1Value) * 2);
          frontRightMotor.run(BACKWARD);
        }
      }

    }
    else {
      stopAllMotors();
    }
  }

  //use xJog2 > 0 to rotate arm right
  if (xJog2Value > 0) {
    if (xJog2Value >= 220) {
      base.write(base.read() - 1);
    }
  }

  //use xJog2 < 0 to rotate arm left
  if (xJog2Value < 0) {
    if (xJog2Value <= -220) {
      base.write(base.read() + 1);
    }
  }

  //use yJog2 > 0 to lift arm up
  if (yJog2Value > 0) {
    if (yJog2Value > 220) {
      pullUpArm();
    }
  }

  //use yJog2 < 0 to pull arm down
  if (yJog2Value < 0) {
    if (yJog2Value < -220) {
      pullDownArm();
    }
  }

  //use yJog3 > 0 to extend arm
  if (yJog3Value > 0) {
    if (yJog3Value > 220 ) {
      extentArm();
    }
  }

  //use yJog3 < 0 to retract arm
  if (yJog3Value < 0) {
    if (yJog3Value < -220) {
      retractArm();
    }
  }

  //use xJog3 < 0 to rotate counterClockwise forearm
  if (xJog3Value > 0) {
    if (xJog3Value > 220) {
      rotateForearmCounterClockwise();
    }
  }

  //use xJog3 > 0 to rotate clockwise forearm
  if (xJog3Value < 0) {
    if (xJog3Value < -220) {
      rotateForearmClockwise();
    }
  }

  //use aJoy button to lift hand up
  if (aJoyValue == 1) {
    liftHandUp();
  }

  //use bJoy button to pull hand down
  if (bJoyValue == 1) {
    pullHandDown();
  }

  //use gJoy to rotate clockwise hand
  if (gJoyValue == 1) {
    rotateHandClockwise();
  }

  //use fJoy to rotate counterClockwise hand
  if (fJoyValue == 1) {
    rotateHandCounterClockwise();
  }

  //Debug funcionction
  if (debug) {
    Serial.println();
    Serial.println();

    Serial.print("xJog1 -> ");
    Serial.print(xJog1Value);
    Serial.print("; xJog2 -> ");
    Serial.print(xJog2Value);
    Serial.print("; xJog3 -> ");
    Serial.println(xJog3Value);

    Serial.print("yJog1 -> ");
    Serial.print(yJog1Value);
    Serial.print("; yJog2 -> ");
    Serial.print(yJog2Value);
    Serial.print("; yJog3 -> ");
    Serial.println(yJog3Value);

    Serial.println();

    Serial.print("Base -> ");
    Serial.println(base.read());
    Serial.print("Lifter ");
    Serial.println(armLifter.read());
    Serial.print("Extender ");
    Serial.println(armExtender.read());
    Serial.print("Forearm ");
    Serial.println(forearm.read());
    Serial.print("Hand lifter ");
    Serial.println(handLifter.read());
    Serial.println("Hand rotator");
    Serial.println(handRotator.read());

    Serial.println();

    Serial.println("Buttons: ");
    Serial.print("A -> ");
    Serial.print(aJoyValue);
    Serial.print("; B -> ");
    Serial.print(bJoyValue);
    Serial.print("; C -> ");
    Serial.print(cJoyValue);
    Serial.print("; D -> ");
    Serial.print(dJoyValue);
    Serial.print("; E -> ");
    Serial.print(eJoyValue);
    Serial.print("; F -> ");
    Serial.print(fJoyValue);
    Serial.print("; G -> ");
    Serial.print(gJoyValue);


    Serial.println();

    delay(500);
  }

  delay(25);
}


void pullUpArm() {
  //do not go over the maxiumum position with arm extender
  armLifter.write(armLifter.read() - 1);
  if (armExtender.read() + armLifter.read() > 200) {
    armExtender.write(armExtender.read() - 1);
  }
  if (armExtender.read() + armLifter.read() < 100) {
    armExtender.write(armExtender.read() + 1);
  }
  delayMicroseconds(15);
}
void pullDownArm() {
  //do not go over the maxiumum position with arm extender
  armLifter.write(armLifter.read() + 1 );
  if (armExtender.read() + armLifter.read() > 200) {
    armExtender.write(armExtender.read() - 1);
  }
  if (armExtender.read() + armLifter.read() < 100) {
    armExtender.write(armExtender.read() + 1);
  }
  delayMicroseconds(15);
}

void extentArm() {
  armExtender.write(armExtender.read() - 1);
  if (armExtender.read() + armLifter.read() < 100) {
    armLifter.write(armLifter.read() + 1);
  }
  if (armExtender.read() + armLifter.read() > 200) {
    armLifter.write(armLifter.read() + 1);
  }
  delayMicroseconds(15);
}

void retractArm() {
  if ( armLifter.read() +  armExtender.read() < 200) {
    armExtender.write(armExtender.read() + 1);
  }
  if (armExtender.read() + armLifter.read() > 200) {
    armLifter.write(armLifter.read() + 1);
  }
  delayMicroseconds(15);
}

void rotateForearmClockwise() {
  forearm.write(forearm.read() + 1);
  delayMicroseconds(15);
}

void rotateForearmCounterClockwise() {
  forearm.write(forearm.read() - 1);
  delayMicroseconds(15);
}

void liftHandUp() {
  handLifter.write(handLifter.read() + 1);
  delayMicroseconds(15);
}

void pullHandDown() {
  handLifter.write(handLifter.read() - 1);
  delayMicroseconds(15);
}

void rotateHandClockwise() {
  handRotator.write(handRotator.read() + 1);
  delayMicroseconds(15);
}

void rotateHandCounterClockwise() {
  handRotator.write(handRotator.read() - 1);
  delayMicroseconds(15);
}

bool xIsMoveing() {
  return xJog1Value != 0;
}

bool yIsMoveing() {
  return yJog1Value != 0;
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
  // centerButtonValue = !digitalRead(centerButton);
  // buttonJog2Value = !digitalRead(buttonJog2Value);
  // buttonJog3Value =  !digitalRead(buttonJog3Value);

  //do not use joystick[0]
  joystick[1] = xJog1Value;
  joystick[2] = yJog1Value;
  joystick[3] = xJog2Value;
  joystick[4] = yJog2Value;
  joystick[5] = xJog3Value;
  joystick[6] = yJog3Value;

  joystick[7] = aJoyValue;
  joystick[8] = bJoyValue;
  joystick[9] = cJoyValue;
  joystick[10] = dJoyValue;
  joystick[11] = eJoyValue;
  joystick[12] = fJoyValue;
  joystick[13] = gJoyValue;

  //  joystick[13] = centerButtonValue;
  //  joystick[14] = buttonJog2Value;
  //  joystick[15] = buttonJog3Value;
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

void readKeyboardValues() {
  if (Serial.available()) {
    serialInput = (char)Serial.read();
    switch (serialInput) {
      //TODO override da tastiera di tutti i comandi
      case 'q': {
          if (keyboardValue <= 256) {
            keyboardValue += 5;
          }
          if (keyboardValue > 255) {
            keyboardValue = 0;
          }
          break;
        }

      case 'a' :
        if (keyboardValue >= 0) {
          keyboardValue -= 5;
        }
        if (keyboardValue < 0) {
          keyboardValue = 255;
        }
        break;

      case 'z':
        keyboardValue = 0;
        stopAllMotors();
        break;

      case 'D':
        debug = !debug;
        if (debug) {
          Serial.println("Start Debug!");
        } else {
          Serial.println("Stop Debug!");
        }
        break;

      default:
        Serial.println("invalid key");
        break;
    }
  }
}


/*
   whit this method the tank can move straight
   due to the mortors position in the same line one must move forward and the other must move backward
*/

void moveFront(int x, int y) {
  rearLeftMotor.setSpeed(y);
  rearLeftMotor.run(FORWARD);

  rearRightMotor.setSpeed(y);
  rearRightMotor.run(BACKWARD);

  frontLeftMotor.setSpeed(y);
  frontLeftMotor.run(BACKWARD);

  frontRightMotor.setSpeed(y);
  frontRightMotor.run(FORWARD);

}

void stopAllMotors() {
  // set all motors speed to 0 to stop motors
  rearLeftMotor.setSpeed(0);
  rearRightMotor.setSpeed(0);
  frontLeftMotor.setSpeed(0);
  frontRightMotor.setSpeed(0);

  // really stop all motors
  rearLeftMotor.run(RELEASE);
  rearRightMotor.run(RELEASE);
  frontLeftMotor.run(RELEASE);
  frontRightMotor.run(RELEASE);
}

void resetServos(const int & baseInitialPosition, const int & armLifterInitialPosition, const int & armExtenderInitialPosition, const int & forearmInitialPosition, const int & handLifterInitialPosition, const int & handRotatorInitialPosition) {
  int delayTime = 25;
  int baseCurrentPosition = base.read();
  int armLifterCurrentPosition = armLifter.read();
  int armExtenderCurrentPosition = armExtender.read();
  int forearmCurrentPosition = forearm.read();
  int handLifterCurrentPosition = handLifter.read();
  int handRotatorCurrentPosition = handRotator.read();

  //move the base to the initial position
  if (baseCurrentPosition != baseInitialPosition) {
    if (baseCurrentPosition > baseInitialPosition) {
      for (int i = baseCurrentPosition; i != baseInitialPosition; i--) {
        base.write(i);
        delay(delayTime);
      }
    } else {
      for (int i = baseCurrentPosition; i != baseInitialPosition; i++) {
        base.write(i);
        delay(delayTime);
      }
    }
  }

  //move the armLifter to the initial position
  if (armLifterCurrentPosition != armLifterInitialPosition) {
    if (armLifterCurrentPosition > armLifterInitialPosition) {
      for (int i = armLifterCurrentPosition; i != armLifterInitialPosition; i--) {
        pullUpArm();
        delay(delayTime);
      }
    } else {
      for (int i = armLifterCurrentPosition; i != armLifterInitialPosition; i++) {
        pullDownArm();
        delay(delayTime);
      }
    }
  }

  //move the armEdtender to the initial position
  if (armExtenderCurrentPosition != armExtenderInitialPosition) {
    if (armExtenderCurrentPosition > armExtenderInitialPosition) {
      for (int i = armExtenderCurrentPosition; i != armExtenderInitialPosition; i--) {
        extentArm();

        delay(delayTime);
      }
    } else {
      for (int i = armExtenderCurrentPosition; i != armExtenderInitialPosition; i++) {
        retractArm();
        delay(delayTime);
      }
    }
  }

  //move the forearm rotator to the initial position
  if (forearmCurrentPosition != forearmInitialPosition) {
    if (forearmCurrentPosition > forearmInitialPosition) {
      for (int i = forearmCurrentPosition; i != forearmInitialPosition; i--) {
        rotateForearmClockwise();
        delay(delayTime);
      }
    } else {
      for (int i = forearmCurrentPosition; i != forearmInitialPosition; i++) {
        rotateForearmCounterClockwise();
        delay(delayTime);
      }
    }
  }

  //move the hand lifter to the initial position
  if (handLifterCurrentPosition != handLifterInitialPosition) {
    if (handLifterCurrentPosition > handLifterInitialPosition) {
      for (int i = handLifterCurrentPosition; i != handLifterInitialPosition; i--) {
        liftHandUp();
        delay(delayTime);
      }
    } else {
      for (int i = handLifterCurrentPosition; i != handLifterInitialPosition; i++) {
        pullHandDown();
        delay(delayTime);
      }
    }
  }

  //move the hand rotator to the initial position
  if (handRotatorCurrentPosition != handRotatorInitialPosition) {
    if (handRotatorCurrentPosition > handRotatorInitialPosition) {
      for (int i = handRotatorCurrentPosition; i != handRotatorInitialPosition; i--) {
        rotateHandClockwise();
        delay(delayTime);
      }
    } else {
      for (int i = handRotatorCurrentPosition; i != handRotatorInitialPosition; i++) {
        rotateHandCounterClockwise();
        delay(delayTime);
      }
    }
  }
}
