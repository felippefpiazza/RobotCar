#include <NewPing.h>
#include <Servo.h>
#include <RobotCarDrive.h>
#include <Debugger.h>

//Setting pins
#define M1Sp 3
#define M2Sp 6
#define M1Dir1 2
#define M1Dir2 4
#define M2Dir1 7
#define M2Dir2 5
#define MAX_SPEED 200
#define SERVO 10
#define SONAR_TRIGGER 9
#define SONAR_ECHO 8
#define SONAR_MAX_DISTANCE 2000
#define SONAR_MIN_DISTANCE 40

//Debug parameter
int debugLevel = 3;

// Servo parameters
int servo_position = 0;
int servo_step = 20;
int start_angle = 25;
int number_of_steps = 5;
int which_step = 0;
int servo_direction = 1;

//Initializa Debugger
Debugger deb(debugLevel);

//Initialize Sonar
NewPing DistanceSensor(SONAR_TRIGGER, SONAR_ECHO, SONAR_MAX_DISTANCE);

//Initialize RobotCarDrive
RobotCarDrive Driver(M1Sp, M2Sp,M1Dir1,M1Dir2,M2Dir1,M2Dir2, MAX_SPEED, &deb);

//Initialize Servo Motor
Servo sweepSonar;

//Other variables
int first_run = 0 ;
int going_where = 0; // 0 = forward | 1 = backward | 2 = right | 3 = left
int sweeperGetAngle() {
  return start_angle + (which_step * servo_step);
}
int sweeperSet(int p) {
  if(p > number_of_steps && 0 > p) {
    deb.debug(2,"SweeperSet", "Servo Position out of range");
    p = 0;
  }
  int position_angle = start_angle + (p * servo_step);
  which_step = p;
  sweepSonar.write(position_angle); 
  return position_angle;
}

int sweeperMoveNext() {
  if(which_step == number_of_steps) {
    servo_direction = -1;
  }
  if (which_step == 0) {
    servo_direction = 1;
  }
  int new_position = servo_direction + which_step;
  return sweeperSet(new_position);
}

void test_run() {
    Driver.setForward();
    Driver.goStraight(100);
    delay(1000);
    Driver.Turn("r",20);
    delay(2000);
    Driver.Turn("l",20);
    delay(2000);
    /*Driver.ChangeDirection();
    delay(1000);
    Driver.goStraight(100);
    delay(500);
    Driver.AxleTurn("r");
    delay(1000);
    Driver.AxleTurn("l");
    delay(1000);*/
    Driver.Stop();
    delay(2000);
}

void Turn(){
  Driver.AxleTurn("r");
  delay(1000);
  deb.debug(2,"Turn","Going straight");
}


void setup() {
  Serial.begin(19200);
  deb.debug(3,"Main Setup", "Starting robot");
  sweepSonar.attach(SERVO);
  Driver.setForward();
  sweeperSet(0);
}

void loop() {
  if(first_run < 1){
    test_run();
    first_run++;
  }
  /*deb.debug(2,"Main loop","Going Straight 1");
  Driver.goStraight(100);
  delay(2000);
  deb.debug(2,"Main loop","Going AXLE TURN");
  Driver.setAxleTurnRight();
  delay(2000);
  deb.debug(2,"Main loop","Going Straight 2");
  Driver.goStraight(100);*/
  unsigned int worst_distance = 3000;
  unsigned int worst_angle = 0;
   for(int i=0; i < number_of_steps ; i++) {
      int angle = sweeperGetAngle();
      unsigned new_cm = DistanceSensor.ping_cm();
      if(new_cm < worst_distance) {
        worst_distance = new_cm;
        worst_angle = angle;
      }
      deb.debug(5,"Distance", new_cm);
      deb.debug(5,"Angle", angle);
      deb.debug(5,"Worst Distance", worst_distance);
      deb.debug(5,"Worst Angle", worst_angle);
      int next_angle = sweeperMoveNext();
      delay(30);
   }
  if(worst_distance < SONAR_MIN_DISTANCE) {
      deb.debug(5,"Distance considered", worst_distance);
      deb.debug(5,"Angle considered", worst_angle);
      deb.debug(2,"Main Loop", "You are to Close");
      Turn();
      delay(50);
    } else {
      deb.debug(2,"Main Loop", "Keep Running");
      Driver.goStraight(100);
    } 
  delay(50);
}
