#include <Arduino.h>
#include <AFMotor.h>

const int leftSensor = A5;
const int rightSensor = A4;
const unsigned long maxReverseTime = 1000;
const int turnTime = 1000;
// DC motor on M1
AF_DCMotor motor1(1);
// DC motor on M2
AF_DCMotor motor2(2);
// DC motor on M3
AF_DCMotor motor3(3);
// DC motor on M4
AF_DCMotor motor4(4);

enum Direction {
    LEFT, RIGHT, NONE
};

void avoidObstacle(Direction dir);
void backup();
void forward();
void turnRight();
void turnLeft();

unsigned long lastForwardTime = 0;

void setup() {
    Serial.begin(9600); // set up Serial library at 9600 bps
    pinMode(leftSensor, INPUT);
    pinMode(rightSensor, INPUT);


    // turn on motor #1;
    motor1.setSpeed(255);
    motor1.run(RELEASE);
    // turn on motor #2
    motor2.setSpeed(255);
    motor2.run(RELEASE);
    // turn on motor #3
    motor3.setSpeed(255);
    motor3.run(RELEASE);
    // turn on motor #4
    motor4.setSpeed(255);
    motor4.run(RELEASE);
    forward();

}

int leftSensorState = 0;
int rightSensorState = 0;



Direction lastDirection = NONE;

void loop() {

    leftSensorState = digitalRead(leftSensor);
    rightSensorState = digitalRead(rightSensor);

    if (leftSensorState == HIGH && rightSensorState == HIGH) {
        if (lastDirection == NONE) {
            int rand = millis() % 2;
            if (rand == 0)
                lastDirection = LEFT;
            else
                lastDirection = RIGHT;
        }
        avoidObstacle(lastDirection);
    } else if (leftSensorState == HIGH) {
        avoidObstacle(RIGHT);
    } else if (rightSensorState == HIGH) {
        avoidObstacle(LEFT);
    }
}

void avoidObstacle(Direction dir) {
    backup();
    if (dir == LEFT)
        turnLeft();
    else
        turnRight();
    forward();
}

void forward() {
    lastForwardTime = millis();
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
}

void turnRight() {
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
    lastDirection = RIGHT;
    delay(turnTime);
}

void turnLeft() {
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
    lastDirection = LEFT;
    delay(turnTime);
}

void backup() {
    unsigned long curTime = millis();
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
    delay(min(maxReverseTime, curTime - lastForwardTime));
}
