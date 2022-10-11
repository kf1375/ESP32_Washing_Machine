#include "BluetoothSerial.h"

#define     COLD_VALVE          13
#define     HOT_VALVE           14
#define     DRAIN_VALVE         27
#define     HEATER              26
#define     CCW                 25
#define     CW                  33
#define     SPEED               32

#define     PRESSURE_SENSOR     34
#define     BOTTOM_TERMOSTAT    35
#define     UP_TERMOSTAT        39

#define     MINUTE_TO_MILIS     60 * 1000
#define     SECOND_TO_MILIS     1000

BluetoothSerial SerialBT;
byte BTData;

typedef enum {
    STOP    =   0,
    START   =   1,
    PAUSE   =   2,
} DeviceMode;

DeviceMode Mode;

/* Cold Valve */
void open_cold_valve(){
    SerialBT.println("open_cold_valve");
    digitalWrite(COLD_VALVE, HIGH);
}

void close_cold_valve(){
    SerialBT.println("close_cold_valve");
    digitalWrite(COLD_VALVE, LOW);
}

/* Hot Valve */
void open_hot_valve(){
    SerialBT.println("open_hot_valve");
    digitalWrite(HOT_VALVE, HIGH);
}

void close_hot_valve(){
    SerialBT.println("close_hot_valve");
    digitalWrite(HOT_VALVE, LOW);
}

/* Drain Valve */
void open_drain_valve(){
    SerialBT.println("open_drain_valve");
    digitalWrite(DRAIN_VALVE, HIGH);
}

void close_drain_valve(){
    SerialBT.println("close_drain_valve");
    digitalWrite(DRAIN_VALVE, LOW);
}

/* Heater */
void set_heater_on(){
    SerialBT.println("set_heater_on");
    digitalWrite(HEATER, HIGH);
}

void set_heater_off(){
    SerialBT.println("set_heater_off");
    digitalWrite(HEATER, LOW);
}

/* Direction */
void rotate_ccw() {
    SerialBT.println("rotate_ccw");
    digitalWrite(CCW,   HIGH);
    digitalWrite(CW,    LOW);
}

void rotate_cw() {
    SerialBT.println("rotate_cw");
    digitalWrite(CCW,   LOW);
    digitalWrite(CW,    HIGH);
}

void stop() {
    SerialBT.println("stop");
    digitalWrite(CCW,   LOW);
    digitalWrite(CW,    LOW);
}

/* Speed */
void rotate_fast() {
    SerialBT.println("rotate_fast");
    digitalWrite(SPEED, HIGH);
}

void rotate_slow(){
    SerialBT.println("rotate_slow");
    digitalWrite(SPEED, LOW);
}

void setup()
{
    // put your setup code here, to run once:
    pinMode(COLD_VALVE,     OUTPUT);
    pinMode(HOT_VALVE,      OUTPUT);
    pinMode(DRAIN_VALVE,    OUTPUT);
    pinMode(HEATER,         OUTPUT);
    pinMode(CCW,            OUTPUT);
    pinMode(CW,             OUTPUT);
    pinMode(SPEED,          OUTPUT);

    pinMode(PRESSURE_SENSOR,    INPUT);
    pinMode(BOTTOM_TERMOSTAT,   INPUT);
    pinMode(UP_TERMOSTAT,       INPUT);
    
    SerialBT.begin();
}

void fill_water() {
    SerialBT.println("fill_water");
    open_cold_valve();
    open_hot_valve();
    // while(digitalRead(PRESSURE_SENSOR) == LOW);
    delay(135 * 1000);
    close_cold_valve();
    close_hot_valve();
}

void pump_water_out() {
    SerialBT.println("pump_out_water");
    open_drain_valve();
    delay(40 * 1000);
    close_drain_valve();
}

void wash(int time) {
    SerialBT.println("washing");
    for(int i = 0; i < int(time / 8000); i++) {
        if(i % 2 == 0) {
          rotate_cw();
            
        }
        else {
            rotate_ccw();
        }
        rotate_slow();
        delay(8000);
        stop();
        delay(4000);
    }
}

void dry(int time) {
    SerialBT.println("drying");
    open_drain_valve();
    rotate_cw();
    rotate_slow();
    delay(5000);
    rotate_fast();
    delay(time);
    close_drain_valve();
    stop();
}

void loop()
{
    if (SerialBT.available())
    {
        BTData = SerialBT.read();
    }

    if(BTData == '1')
    {
        BTData = 0;
        fill_water();
        wash(10 * MINUTE_TO_MILIS);
        pump_water_out();
        fill_water();
        wash(10 * MINUTE_TO_MILIS);
        pump_water_out();
        dry(1    * MINUTE_TO_MILIS);
        fill_water();
        wash(10 * MINUTE_TO_MILIS);
        pump_water_out();
        dry(2    * MINUTE_TO_MILIS);
    }
}
