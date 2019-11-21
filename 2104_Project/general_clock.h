#ifndef GENERAL_CLOCK_H_
#define GENERAL_CLOCK_H_

void initializeGeneralClock();
int startTracking();
void stopTracking(int id);
void resetCounter(int id);
int getCounter(int id);
int getLoops(int id);

#endif
