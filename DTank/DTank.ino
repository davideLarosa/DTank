/*
   Tank control
   This tank has a base moved by 4 DC motors and an arm moved by 6 Servo motrs. In the top of the hand there is a tongs moved by another Servo motor.
   All the DC motors are actived by a DK Electronics motors shield powered out by 2 voltage regulator that provides 12 VDC 5A to the shield.
   The other motors are powered up by 3 other same voltage regulator.
   Finally there is a joystick shield to drive that all.

   To power up all the motors and the arduino Mega too is installed a power supply that provide 5VDC 2A (dedicated to Arduino) and 12VDC 2A (dedicated to DC motors).
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
Servo claw;

//servo pinouts definition
#define basePin 22
#define armLifterPin 23
#define armExtenderPin 24
#define forearmPin 25
#define handLifterPin 26
#define handRotatorPin 27
#define clawPin 28

static int keyboardValue = 0; //value that override movements by keyboard
static char serialInput = '\b'; //input value from serial

//variable used to parse Serial1 input (wireless joystick)
static String received = ""; //this is the entire value received from Serial1 input after parsing input

//setup joystick
const int nofKeys =  13;

int joystick[nofKeys] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

bool debug = false;
bool pause = false;

const static int baseInitialPosition = 85;
const static int armLifterInitialPosition = 100;
const static int armExtenderInitialPosition = 0;
const static int forearmInitialPosition = 70;
const static int handLifterInitialPosition = 70;
const static int handRotatorInitialPosition = 90;
const static int clawInitialPosition = 90;
const static int clawMaxPosition = 119;

const static int nofServos = 7;

int servosPosition[nofServos];

void setup() {
  delay(100);
  Serial.begin(9600); //start serial monitor communicaiton
  Serial1.begin(9600); //stat xbee joystick communication

  Serial.println("Start setup tank...");

  stopAllMotors();

  /*
    Setup servos:
      pinMode(pin, function) -> could be avoided
      Servo.attach(pin) -> this funcion attaches the selected servo to a pin
  */
  pinMode(basePin, OUTPUT);
  pinMode(armLifterPin, OUTPUT);
  pinMode(armExtenderPin, OUTPUT);
  pinMode(forearmPin, OUTPUT);
  pinMode(handLifterPin, OUTPUT);
  pinMode(handRotatorPin, OUTPUT);
  pinMode(clawPin, OUTPUT);

  base.attach(basePin);
  armLifter.attach(armLifterPin);
  armExtender.attach(armExtenderPin);
  forearm.attach(forearmPin);
  handLifter.attach(handLifterPin);
  handRotator.attach(handRotatorPin);
  claw.attach(clawPin);

  //move servos to initial position
  Serial.println("Please wait while resetting servos...");
  resetServos(baseInitialPosition, armLifterInitialPosition, armExtenderInitialPosition, forearmInitialPosition, handLifterInitialPosition, handRotatorInitialPosition, clawInitialPosition);

  /*
    Simply writes on Serial Monitor some informations
  */
  delay(1000);
  Serial.println("Setup done!");
  Serial.println("                3                ");
  delay(1000);
  Serial.println("                2                ");
  delay(1000);
  Serial.println("                1                ");
  delay(1000);
  Serial.println("------------> GO!!! <------------");

  Serial.println("Hit \"D\" if you wanto to run in debug mode!");
  Serial.println("IMPORTANT!!!");
  Serial.println("Running in debug mode everything will be too much slower and wireless connection could not work well!");
  Serial.println("To gain the normal speed exit from debug mode (Hit \"D\")");
  Serial.println();
  Serial.println();
}


void loop() {
  readKeyboardValues();

  if (!pause) {
    //override commands by keyboard
    if (keyboardValue) {}

    else {
      //get all the joystick values: a better description below
      readJoystickValues();

      //if no jogs are moveing stop all motors
      if ( !xIsMoveing() && !yIsMoveing()) {
        stopAllMotors();
      }
      //get y values
      if (yIsMoveing() && !xIsMoveing() ) {
        //if y is moveing on positive values move front
        if (joystick[1] > 1) {
          rearLeftMotor.setSpeed(joystick[1]);
          rearLeftMotor.run(FORWARD);

          frontLeftMotor.setSpeed(joystick[1]);
          frontLeftMotor.run(BACKWARD);

          rearRightMotor.setSpeed(joystick[1]);
          rearRightMotor.run(BACKWARD);

          frontRightMotor.setSpeed(joystick[1]);
          frontRightMotor.run(FORWARD);
        }
        //if y is moveing on negative values move back
        else if (joystick[1] < -1) {
          rearLeftMotor.setSpeed(-joystick[1]);
          rearLeftMotor.run(BACKWARD);

          frontLeftMotor.setSpeed(-joystick[1]);
          frontLeftMotor.run(FORWARD);

          rearRightMotor.setSpeed(-joystick[1]);
          rearRightMotor.run(FORWARD);

          frontRightMotor.setSpeed(-joystick[1]);
          frontRightMotor.run(BACKWARD);
        }
      }

      //get x values
      else if (!yIsMoveing() && xIsMoveing()) {
        //if x is moveing on positive values move right
        if (joystick[0] > 1) {
          rearLeftMotor.setSpeed(joystick[0]);
          rearLeftMotor.run(BACKWARD);

          rearRightMotor.setSpeed(joystick[0]);
          rearRightMotor.run(BACKWARD);

          frontLeftMotor.setSpeed(joystick[0]);
          frontLeftMotor.run(FORWARD);

          frontRightMotor.setSpeed(joystick[0]);
          frontRightMotor.run(FORWARD);
        }
        //if x is moveing on negative values move left
        if (joystick[0] < -1) {
          rearLeftMotor.setSpeed(-joystick[0]);
          rearLeftMotor.run(FORWARD);

          rearRightMotor.setSpeed(-joystick[0]);
          rearRightMotor.run(FORWARD);

          frontLeftMotor.setSpeed(-joystick[0]);
          frontLeftMotor.run(BACKWARD);

          frontRightMotor.setSpeed(-joystick[0]);
          frontRightMotor.run(BACKWARD);
        }
      }
      //get both x and y values
      else if (yIsMoveing() && xIsMoveing()) {
        if (joystick[0] > 1) {
          //first cartesian quadrant
          if (joystick[1] > 1) {
            rearLeftMotor.setSpeed(joystick[1] / (512 / joystick[0]) * 2);
            rearLeftMotor.run(FORWARD);

            frontLeftMotor.setSpeed(joystick[1] / (512 / joystick[0]) * 2);
            frontLeftMotor.run(BACKWARD);

            rearRightMotor.setSpeed(joystick[1]);
            rearRightMotor.run(BACKWARD);

            frontRightMotor.setSpeed(joystick[1]);
            frontRightMotor.run(FORWARD);
          }
          //fourth cartesian quadrant
          if (joystick[1] < -1) {
            rearLeftMotor.setSpeed(-joystick[1] / (512 / joystick[0]) * 2);
            rearLeftMotor.run(BACKWARD);

            frontLeftMotor.setSpeed(-joystick[1] / (512 / joystick[0]) * 2);
            frontLeftMotor.run(FORWARD);

            rearRightMotor.setSpeed(-joystick[1]);
            rearRightMotor.run(FORWARD);

            frontRightMotor.setSpeed(-joystick[1]);
            frontRightMotor.run(BACKWARD);
          }
        }
        if (joystick[0] < -1) {
          //second cartesian quadrant
          if (joystick[1] > 1) {
            rearLeftMotor.setSpeed(joystick[1]);
            rearLeftMotor.run(FORWARD);

            frontLeftMotor.setSpeed(joystick[1]);
            frontLeftMotor.run(BACKWARD);

            rearRightMotor.setSpeed(joystick[1] / (512 / -joystick[0]) * 2);
            rearRightMotor.run(BACKWARD);

            frontRightMotor.setSpeed(joystick[1] / (512 / -joystick[0]) * 2);
            frontRightMotor.run(FORWARD);
          }
          //third cartesian quadrant
          if (joystick[1] < -1) {
            rearLeftMotor.setSpeed(-joystick[1]);
            rearLeftMotor.run(BACKWARD);

            frontLeftMotor.setSpeed(-joystick[1]);
            frontLeftMotor.run(FORWARD);

            rearRightMotor.setSpeed(-joystick[1] / (512 / -joystick[0]) * 2);
            rearRightMotor.run(FORWARD);

            frontRightMotor.setSpeed(-joystick[1] / (512 / -joystick[0]) * 2);
            frontRightMotor.run(BACKWARD);
          }
        }
      }
      //if something wrong is happening stop all motors
      else {
        stopAllMotors();
      }
    }

    //use xJog2 > 0 to rotate arm right
    if (joystick[2] > 0) {
      if (joystick[2] >= 220) {
        base.write(base.read() - 1);
      }
    }

    //use xJog2 < 0 to rotate arm left
    if (joystick[2] < 0) {
      if (joystick[2] <= -220) {
        base.write(base.read() + 1);
      }
    }

    //use yJog2 > 0 to lift arm up
    if (joystick[3] > 0) {
      if (joystick[3] > 220) {
        pullUpArm();
      }
    }

    //use yJog2 < 0 to pull arm down
    if (joystick[3] < 0) {
      if (joystick[3] < -220) {
        pullDownArm();
      }
    }

    //use xJog3 < 0 to rotate counterClockwise forearm
    if (joystick[4] > 0) {
      if (joystick[4] > 220) {
        rotateForearmCounterClockwise();
      }
    }

    //use xJog3 > 0 to rotate clockwise forearm
    if (joystick[4] < 0) {
      if (joystick[4] < -220) {
        rotateForearmClockwise();
      }
    }

    //use yJog3 > 0 to extend arm
    if (joystick[5] > 0) {
      if (joystick[5] > 220 ) {
        extentArm();
      }
    }

    //use yJog3 < 0 to retract arm
    if (joystick[5] < 0) {
      if (joystick[5] < -220) {
        retractArm();
      }
    }

    //use aJoy button to lift hand up
    if (joystick[6] == 1) {
      liftHandUp();
    }

    //use bJoy button to pull hand down
    if (joystick[7] == 1) {
      pullHandDown();
    }

    //use gJoy to rotate clockwise hand
    if (joystick[12] == 1) {
      rotateHandClockwise();
    }

    //use fJoy to rotate counterClockwise hand
    if (joystick[11] == 1) {
      rotateHandCounterClockwise();
    }

    //use eJoy to close claw
    if (joystick[10] == 1) {
      openClaw();
    }

    //use dJoy to open claw
    if (joystick[9] == 1) {
      closeClaw();
    }

    //reset all to initial position
    if (joystick[6] == 1 && joystick[7] == 1 && joystick[12] == 1 && joystick[11] == 1) {
      resetServos();
    }
  }
  //Debug funcionction
  if (debug) {
    printDebug();
  }
  //use this delay to get wireless data with no loss
  delay(10);
}

//print some description to Serial Monitor
void printDebug() {
  Serial.println();
  Serial.println();

  Serial.print("xJog1 -> ");
  Serial.print(joystick[0]);
  Serial.print("; xJog2 -> ");
  Serial.print(joystick[2]);
  Serial.print("; xJog3 -> ");
  Serial.println(joystick[4]);

  Serial.print("yJog1 -> ");
  Serial.print(joystick[1]);
  Serial.print("; yJog2 -> ");
  Serial.print(joystick[3]);
  Serial.print("; yJog3 -> ");
  Serial.println(joystick[5]);

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
  Serial.print("Hand rotator ");
  Serial.println(handRotator.read());
  Serial.print("Claw ");
  Serial.println(claw.read());

  Serial.println();

  Serial.println("Buttons: ");
  Serial.print("A -> ");
  Serial.print(joystick[6]);
  Serial.print("; B -> ");
  Serial.print(joystick[7]);
  Serial.print("; C -> ");
  Serial.print(joystick[8]);
  Serial.print("; D -> ");
  Serial.print(joystick[9]);
  Serial.print("; E -> ");
  Serial.print(joystick[10]);
  Serial.print("; F -> ");
  Serial.print(joystick[11]);
  Serial.print("; G -> ");
  Serial.print(joystick[12]);


  Serial.println();

  delay(500);
}

//moves arm up
void pullUpArm() {
  //do not go over the maxiumum position with arm extender
  armLifter.write(armLifter.read() - 1);
  if (armExtender.read() + armLifter.read() > 200) {
    armExtender.write(armExtender.read() - 1);
  }
  if (armExtender.read() + armLifter.read() < 100) {
    armExtender.write(armExtender.read() + 1);
  }
}

//moves arm down
void pullDownArm() {
  //do not go over the maxiumum position with arm extender
  armLifter.write(armLifter.read() + 1 );
  if (armExtender.read() + armLifter.read() > 200) {
    armExtender.write(armExtender.read() - 1);
  }
  if (armExtender.read() + armLifter.read() < 100) {
    armExtender.write(armExtender.read() + 1);
  }
}

//moves arm front
void extentArm() {
  //do not go over the maxiumum position with arm lifter
  armExtender.write(armExtender.read() - 1);
  if (armExtender.read() + armLifter.read() < 100) {
    armLifter.write(armLifter.read() + 1);
  }
  if (armExtender.read() + armLifter.read() > 200) {
    armLifter.write(armLifter.read() + 1);
  }
}

//moves arm back
void retractArm() {
  //do not go over the maxiumum position with arm lifter
  if ( armLifter.read() +  armExtender.read() < 200) {
    armExtender.write(armExtender.read() + 1);
  }
  if (armExtender.read() + armLifter.read() > 200) {
    armLifter.write(armLifter.read() + 1);
  }
}


//rotate only forearm clockwise
void rotateForearmClockwise() {
  forearm.write(forearm.read() + 1);
}

//rotate only forearm counterclockwise
void rotateForearmCounterClockwise() {
  forearm.write(forearm.read() - 1);
}

//lift hand up
void liftHandUp() {
  handLifter.write(handLifter.read() + 1);
}

//pull down hand
void pullHandDown() {
  handLifter.write(handLifter.read() - 1);
}

//rotate only hand clockwise
void rotateHandClockwise() {
  handRotator.write(handRotator.read() + 1);
}

//rotate only hand counterclockwise
void rotateHandCounterClockwise() {
  handRotator.write(handRotator.read() - 1);
}

//open the claw
void openClaw() {
  claw.write(claw.read() - 1);
}

//close the claw
void closeClaw() {
  //do not exceed the maximum position
  if (claw.read() < clawMaxPosition) {
    claw.write(claw.read() + 1);
  }
}

//return if xJoy1 is moveing
bool xIsMoveing() {
  return joystick[0] != 0;
}

//return if yJoy1 is moveing
bool yIsMoveing() {
  return joystick[1] != 0;
}

/*use this funciont to get stabilized numbers
  -> if values are near to 0 return 0
  else if values are near to the maximum return the maximum
*/
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

/*
  use this function to read all the keyboard values
*/
void readKeyboardValues() {
  if (Serial.available()) {
    serialInput = (char)Serial.read();
    switch (serialInput) {
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

      case '0':
        pause =  true;
        stopAllServos();
        break;

      case '1':
        pause =  false;
        restoreAllServos();
        break;

      case 'o':
        armLifter.write(armLifter.read() + 1);
        break;

      case 'u':
        armLifter.write(armLifter.read() - 1);
        break;

      case 'j':
        armExtender.write(armExtender.read() + 1);
        break;

      case 'l':
        armExtender.write(armExtender.read() - 1);
        break;

      case 'm':
        handRotator.write(handRotator.read() + 1);
        break;

      case '.':
        handRotator.write(handRotator.read() - 1);
        break;

      case 't':
        claw.write(claw.read() + 1);
        break;

      case 'y':
        claw.write(claw.read() - 1);
        break;

      case 'g':
        handLifter.write(handLifter.read() + 1);
        break;

      case 'h':
        handLifter.write(handLifter.read() - 1);
        break;

      case 'b':
        forearm.write(forearm.read() + 1);
        break;

      case 'n':
        forearm.write(forearm.read() - 1);
        break;

      case 'c':
        base.write(base.read() + 1);
        break;

      case 'v':
        base.write(base.read() - 1);
        break;

      default:
        Serial.println("invalid key");
        break;
    }
  }
}


/*
  this function stop all motors when you ask for the pause
*/
void stopAllServos() {

  servosPosition[0] = base.read();
  servosPosition[1] = armLifter.read();
  servosPosition[2] = armExtender.read();
  servosPosition[3] = forearm.read();
  servosPosition[4] = handLifter.read();
  servosPosition[5] = handRotator.read();
  servosPosition[6] = claw.read();

  int baseStopPosition = 95;
  int lifterStopPosition = 180;
  int extenderStopPosition = 0;
  int forearmStopPosition = 60;
  int handLifterStopPosition = 180;
  int handRotatorStopPosition = 0;
  int clawStopPosition = 119;

  stopAllMotors();

  moveServoTo(claw, servosPosition[6], clawStopPosition, true);
  moveServoTo(handRotator, servosPosition[5], handRotatorStopPosition, true);
  moveServoTo(handLifter, servosPosition[4], handLifterStopPosition, true);
  moveServoTo(forearm, servosPosition[3], forearmStopPosition, true);
  moveServoTo(armExtender, servosPosition[2], extenderStopPosition, true);
  moveServoTo(armLifter, servosPosition[1], lifterStopPosition, true);
  moveServoTo(base, servosPosition[0], baseStopPosition, true);
}

void moveServoTo(Servo& servo, int& startPosition, int& stopPosition, bool detach) {
  if (startPosition > stopPosition) {
    for (int i = startPosition; i != stopPosition; i--) {
      servo.write(i);
      delay(20);
    }
    if (detach)
      servo.detach();
  }

  if (startPosition < stopPosition) {
    for (int i = startPosition; i != stopPosition; i++) {
      servo.write(i);
      delay(20);
    }
    if (detach)
      servo.detach();
  }
}

/*
  this funciont restart all motors in them last position when you came from a pause
*/

void restoreAllServos() {
  base.attach(basePin);
  armLifter.attach(armLifterPin);
  armExtender.attach(armExtenderPin);
  forearm.attach(forearmPin);
  handLifter.attach(handLifterPin);
  handRotator.attach(handRotatorPin);
  claw.attach(clawPin);

  int baseStartPosition = base.read();
  int armLifterStartPosition = armLifter.read();
  int armExtenderStartPosition = armExtender.read();
  int forearmStartPosition = forearm.read();
  int handLifterStartPosition = handLifter.read();
  int handRotatorStartPosition = handRotator.read();
  int clawStartPosition = claw.read();

  moveServoTo(base, baseStartPosition, servosPosition[0], false);
  moveServoTo(armLifter, armLifterStartPosition, servosPosition[1], false);
  moveServoTo(armExtender, armExtenderStartPosition, servosPosition[2], false);
  moveServoTo(forearm, forearmStartPosition, servosPosition[3], false);
  moveServoTo(handLifter, handLifterStartPosition, servosPosition[4], false);
  moveServoTo(handRotator, handRotatorStartPosition, servosPosition[5], false);
  moveServoTo(claw, clawStartPosition, servosPosition[6], false);

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


//stop all track motors
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


//use this method to recall initial position
void resetServos() {
  resetServos(baseInitialPosition, armLifterInitialPosition, armExtenderInitialPosition, forearmInitialPosition, handLifterInitialPosition, handRotatorInitialPosition, clawInitialPosition);
}

// move slowly all the servos to the initial position
void resetServos(const int & baseInitialPosition, const int & armLifterInitialPosition, const int & armExtenderInitialPosition, const int & forearmInitialPosition, const int & handLifterInitialPosition, const int & handRotatorInitialPosition, const int & clawInitialPosition) {
  Serial.println();
  Serial.println();
  Serial.println("Start reset servos");
  claw.write(clawMaxPosition);
  int delayTime = 75;
  int baseCurrentPosition = base.read();
  int armLifterCurrentPosition = armLifter.read();
  int armExtenderCurrentPosition = armExtender.read();
  int forearmCurrentPosition = forearm.read();
  int handLifterCurrentPosition = handLifter.read();
  int handRotatorCurrentPosition = handRotator.read();
  int clawCurrentPosition = claw.read();

  //move the base to the initial position
  Serial.print("-> Resetting base! ");
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
  Serial.println(" DONE! ");

  //move the armLifter to the initial position
  Serial.print("-> Resetting arm lifter! ");
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
  Serial.println( " DONE! ");

  //move the armEdtender to the initial position
  Serial.print( "-> Resetting arm extender! ");
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
  Serial.println( " DONE! ");

  //move the forearm rotator to the initial position
  Serial.print( "-> Resetting forearm! ");
  if (forearmCurrentPosition != forearmInitialPosition) {
    if (forearmCurrentPosition > forearmInitialPosition) {
      for (int i = forearmCurrentPosition; i != forearmInitialPosition; i--) {
        rotateForearmCounterClockwise();
        delay(delayTime);
      }
    } else {
      for (int i = forearmCurrentPosition; i != forearmInitialPosition; i++) {
        rotateForearmClockwise();
        delay(delayTime);
      }
    }
  }
  Serial.println( " DONE! ");

  //move the hand lifter to the initial position
  Serial.print( "-> Resetting arm lifter! ");
  if (handLifterCurrentPosition != handLifterInitialPosition) {
    if (handLifterCurrentPosition > handLifterInitialPosition) {
      for (int i = handLifterCurrentPosition; i != handLifterInitialPosition; i--) {
        pullHandDown();
        delay(delayTime);
      }
    } else {
      for (int i = handLifterCurrentPosition; i != handLifterInitialPosition; i++) {
        liftHandUp();
        delay(delayTime);
      }
    }
  }
  Serial.println( " DONE! ");

  //move the hand rotator to the initial position
  Serial.print( "-> Resetting hand rotator! ");
  if (handRotatorCurrentPosition != handRotatorInitialPosition) {
    if (handRotatorCurrentPosition > handRotatorInitialPosition) {
      for (int i = handRotatorCurrentPosition; i != handRotatorInitialPosition; i--) {
        rotateHandCounterClockwise();
        delay(delayTime);
      }
    } else {
      for (int i = handRotatorCurrentPosition; i != handRotatorInitialPosition; i++) {
        rotateHandClockwise();
        delay(delayTime);
      }
    }
  }
  Serial.println( " DONE! ");


  Serial.print( "-> Resetting claw! ");
  if (clawCurrentPosition != clawInitialPosition) {
    if (clawCurrentPosition > clawInitialPosition) {
      for (int i = clawCurrentPosition; i != clawInitialPosition; i--) {
        openClaw();
        delay(delayTime);
      }
    } else {
      for (int i = clawCurrentPosition; i != clawInitialPosition; i++) {
        closeClaw();
        delay(delayTime);
      }
    }
  }
  Serial.println( " DONE! ");

}


//read values from xbee if available
void readJoystickValues() {
  while (Serial1.available() > 0) {
    received = Serial1.readStringUntil(';');
    fillJoystickValues();
  }
}

//put received values from xbee to the joystick array
void fillJoystickValues() {
  int pos = getPos();
  int value = getValue();
  joystick[getPos()] = getValue();
}

//return position calculated from received stream
int getPos() {
  return convertToInt(received[0]) * 10 + convertToInt(received[1]);
}

//return value calculated from received stream
int getValue() {
  int toReturn = (convertToInt(received[3]) * 100) + (convertToInt(received[4]) * 10) + convertToInt(received[5]);
  if (convertToInt(received[2]) == 1)
    return toReturn * -1;
  return toReturn;
}

//convert chars received from xbee to int
int convertToInt(char toConvert) {
  int toReturn = 0;
  switch (toConvert) {
    case 48:
      toReturn = 0;
      break;

    case 49:
      toReturn = 1;
      break;

    case 50:
      toReturn = 2;
      break;

    case 51:
      toReturn = 3;
      break;

    case 52:
      toReturn = 4;
      break;

    case 53:
      toReturn = 5;
      break;

    case 54:
      toReturn = 6;
      break;

    case 55:
      toReturn = 7;
      break;

    case 56:
      toReturn = 8;
      break;

    case 57:
      toReturn = 9;
      break;

    default:
      //print the error received
      Serial.println("An error occurred while receiving wireless joystick data!");
      Serial.print("Received: ");
      Serial.println(toConvert);
      Serial.println();
      break;
  }
  return toReturn;
}
