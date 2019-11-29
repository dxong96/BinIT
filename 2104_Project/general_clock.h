#ifndef GENERAL_CLOCK_H_
#define GENERAL_CLOCK_H_

// initialize registers used by the clock
void initializeGeneralClock();
// assign and counter id and starts counting
int startTracking();
// stop counting given an id
void stopTracking(int id);
// reset the counting to 0 given an id
void resetCounter(int id);
// get the current counter given id
int getCounter(int id);
// get the the number of time looped over 2^32
int getLoops(int id);

#endif
