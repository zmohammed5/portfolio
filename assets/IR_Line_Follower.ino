/*
 * Line Follower - Smooth Continuous v2
 * Platform: Arduino UNO R3 + HW-130 Motor Shield
 * 3 sensors: Left(A1), Center(A2), Right(A0)
 *
 * Motor directions (sensors face forward):
 *   M1 (front left):  BACKWARD = real forward
 *   M2 (front right): FORWARD  = real forward
 *   M3 (rear left):   FORWARD  = real forward
 *   M4 (rear right):  BACKWARD = real forward
 */

#include <AFMotor.h>

AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

const int sensorLeft   = A0;
const int sensorRight  = A1;
const int sensorCenter = A2;

// --- ADJUST THESE TO TUNE ---
int straightSpeed = 85;   // default forward speed
int turnSpeed = 105;      // correction/turn speed
int curveSpeed = 0;       // inner wheel stops on curves for sharper turns
int lastTurn = 0;

void setup() {
  pinMode(sensorLeft, INPUT);
  pinMode(sensorCenter, INPUT);
  pinMode(sensorRight, INPUT);

  motor1.run(RELEASE); motor2.run(RELEASE);
  motor3.run(RELEASE); motor4.run(RELEASE);

  Serial.begin(9600);
  Serial.println("Smooth Line Follower v2");
  delay(3000);
}

void loop() {
  int L = !digitalRead(sensorLeft);
  int C = !digitalRead(sensorCenter);
  int R = !digitalRead(sensorRight);

  Serial.print("L="); Serial.print(L);
  Serial.print(" C="); Serial.print(C);
  Serial.print(" R="); Serial.println(R);

  if (C && !L && !R) {
    // Center only - slight right bias for circular track
    driveMotors(straightSpeed + 10, straightSpeed);
    lastTurn = 0;
  }
  else if (C && L && !R) {
    // Center + left - gentle left (right wheels faster)
    driveMotors(curveSpeed, turnSpeed);
    lastTurn = -1;
  }
  else if (C && !L && R) {
    // Center + right - gentle right (left wheels faster)
    driveMotors(turnSpeed, curveSpeed);
    lastTurn = 1;
  }
  else if (L && !C && !R) {
    // Left only - line goes left, follow it
    driveMotors(0, turnSpeed);
    lastTurn = -1;
  }
  else if (R && !C && !L) {
    // Right only - line goes right, follow it
    driveMotors(turnSpeed, 0);
    lastTurn = 1;
  }
  else if (!L && !C && !R) {
    // Lost line - search in last known direction
    if (lastTurn == -1) {
      driveMotors(0, turnSpeed);
    } else if (lastTurn == 1) {
      driveMotors(turnSpeed, 0);
    } else {
      driveMotors(turnSpeed, turnSpeed);
    }
  }
  else if (L && C && R) {
    // All on line - stop
    stopMotors();
  }
  else {
    driveMotors(straightSpeed, straightSpeed);
  }

  delay(5);
}

// Single function - set both sides, always forward
void driveMotors(int leftSpeed, int rightSpeed) {
  // M2/M3 = physical left side (front left + rear left)
  if (leftSpeed > 0) {
    motor2.setSpeed(leftSpeed); motor2.run(FORWARD);
    motor3.setSpeed(leftSpeed); motor3.run(FORWARD);
  } else {
    motor2.run(RELEASE);
    motor3.run(RELEASE);
  }

  // M1/M4 = physical right side (front right + rear right)
  if (rightSpeed > 0) {
    motor1.setSpeed(rightSpeed); motor1.run(BACKWARD);
    motor4.setSpeed(rightSpeed); motor4.run(BACKWARD);
  } else {
    motor1.run(RELEASE);
    motor4.run(RELEASE);
  }
}

void stopMotors() {
  motor1.run(RELEASE); motor2.run(RELEASE);
  motor3.run(RELEASE); motor4.run(RELEASE);
}
