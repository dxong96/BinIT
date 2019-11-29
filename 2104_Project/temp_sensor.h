// start reading temperature sensor
void readTempSensor(float * result);
// read the received data and extract the temperature
float parseCelcius(unsigned char * data);
// read the received data and extract the humidity
float parseHumidity(unsigned char * data);
