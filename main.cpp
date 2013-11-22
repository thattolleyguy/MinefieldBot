#include <Arduino.h>
#include <AFMotor.h>

const int LEFT_SENSOR = A5;
const int RIGHT_SENSOR = A4;
const unsigned long MAX_REVERSE_TIME = 500;
const int TURN_TIME = 500;
const int AVOIDANCE_TIME = 2 * MAX_REVERSE_TIME + TURN_TIME;
// DC motor on M1
AF_DCMotor frontRight(1);
// DC motor on M2
AF_DCMotor backRight(2);
// DC motor on M3
AF_DCMotor backLeft(3);
// DC motor on M4
AF_DCMotor frontLeft(4);

enum Direction {
    LEFT, RIGHT, NONE
};

void avoidObstacle(Direction dir);
void backup();
void forward();
void turnRight();
void turnLeft();

unsigned long lastForwardTime = 0;
int leftDirectionBias = 0;
unsigned long lastAvoidTime = 0;
int potentialRepeatAvoids = 0;

void setup() {
    Serial.begin(9600); // set up Serial library at 9600 bps
    pinMode(LEFT_SENSOR, INPUT);
    pinMode(RIGHT_SENSOR, INPUT);


    // turn on motor #1;
    frontRight.setSpeed(255);
    frontRight.run(RELEASE);
    // turn on motor #2
    backRight.setSpeed(255);
    backRight.run(RELEASE);
    // turn on motor #3
    backLeft.setSpeed(255);
    backLeft.run(RELEASE);
    // turn on motor #4
    frontLeft.setSpeed(255);
    frontLeft.run(RELEASE);
    forward();

}

int leftSensorState = 0;
int rightSensorState = 0;



Direction lastDirection = NONE;

void loop() {

    leftSensorState = digitalRead(LEFT_SENSOR);
    rightSensorState = digitalRead(RIGHT_SENSOR);

    if (leftSensorState == HIGH && rightSensorState == HIGH) {
        Direction avoidDirection = lastDirection;
        if (leftDirectionBias != 0) {
            if (leftDirectionBias > 0)
                avoidDirection = RIGHT;
            else
                avoidDirection = LEFT;
        } else if (lastDirection == NONE) {
            int rand = millis() % 2;
            if (rand == 0)
                avoidDirection = LEFT;
            else
                avoidDirection = RIGHT;
        }
        avoidObstacle(avoidDirection);
    } else if (leftSensorState == HIGH) {
        avoidObstacle(RIGHT);
    } else if (rightSensorState == HIGH) {
        avoidObstacle(LEFT);
    }
}

void avoidObstacle(Direction potentialDirection) {
    unsigned long curTime = millis();
    if (curTime - lastAvoidTime < AVOIDANCE_TIME)
        potentialRepeatAvoids++;
    else
        potentialRepeatAvoids = 0;

    backup();
    if (leftDirectionBias > 6)
        potentialDirection = RIGHT;
    else if (leftDirectionBias<-6)
        potentialDirection = LEFT;
    if (potentialDirection == LEFT)
        turnLeft();
    else
        turnRight();
    forward();
}

void forward() {
    lastForwardTime = millis();
    frontRight.run(FORWARD);
    backRight.run(FORWARD);
    backLeft.run(FORWARD);
    frontLeft.run(FORWARD);
}

void turnRight() {
    frontRight.run(BACKWARD);
    backRight.run(BACKWARD);
    backLeft.run(FORWARD);
    frontLeft.run(FORWARD);
    lastDirection = RIGHT;
    leftDirectionBias -= 1;
    delay(TURN_TIME * (1 + potentialRepeatAvoids));
}

void turnLeft() {
    frontRight.run(FORWARD);
    backRight.run(FORWARD);
    backLeft.run(BACKWARD);
    frontLeft.run(BACKWARD);
    lastDirection = LEFT;
    leftDirectionBias += 1;

    delay(TURN_TIME * (1 + potentialRepeatAvoids));
}

void backup() {
    unsigned long curTime = millis();
    frontRight.run(BACKWARD);
    backRight.run(BACKWARD);
    backLeft.run(BACKWARD);
    frontLeft.run(BACKWARD);
    delay(min(MAX_REVERSE_TIME, curTime - lastForwardTime));
}
