# 2104 Project

Project BinIT

## Getting Started

The main folder is in 2104_project. Import into code composer to run it

### Ultrasonic Sensor 1

```
Trigger PIN P1.6
Echo PIN P6.7
3V3, GND
```

### Ultrasonic Sensor 2 (Internal - distance of bin)

```
Height of bin (100%) - 32cm
Max height to check - 24cm
Trigger PIN P5.0
Echo PIN P5.1
3V3, GND
```

### Temperature

```
Data PIN P1.7
3V3, GND
```

### LCD

```
PIN1 GND
PIN2 3V3
PIN3 -
PIN4 P4.5
PIN5 P6.0
PIN6 P4.3
PIN7 -
PIN8 -
PIN9 -
PIN10 -
PIN11 P6.1
PIN12 P4.0
PIN13 P4.2
PIN14 P4.4
PIN15 -
PIN16 -
```


### WIFI
```
PIN1 P3.2
2 3V3
4 3V3
5 GND
PIN8 P3.3
```

#### SERVO
```
5V
GND
P2.4
```


### To set up statistics server
To retrieve the accountSID and authToken go to https://www.twilio.com/docs/authy/twilioauth-sdk/quickstart/obtain-authy-api-key.
Create a .env and add in the twillio accountSID and authToken. This is for starting the message portion.

To start node server.
Cd to fileDirectory and type:
```
node index.js
```
