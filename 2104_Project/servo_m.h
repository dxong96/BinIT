#ifndef SERVO_M_H_
#define SERVO_M_H_

// note: the servo we are support up to 120
// so 90 becomes 60
// 180 becomes 120

// initialize registers related to servo
void initServo();
// go to angle 0
void goToPosition0();
// go to angle 90
void goToPosition90();
// go to angle 180
void goToPosition180();

#endif /* SERVO_M_H_ */
