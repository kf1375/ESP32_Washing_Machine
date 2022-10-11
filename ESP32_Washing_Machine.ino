#include "BluetoothSerial.h"

/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
    #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

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

String BTData;

int Total_Wash_Time = 0; // In minute
int Wash_Time = 0;
int Rinse_Time = 0;

typedef enum {
    STOP    =   0,
    START   =   1,
    PAUSE   =   2,
} DeviceMode;

DeviceMode Mode;

/* Cold Vavle */
void open_cold_valve(){
    SerialBT.println("open_cold_valve");
    digitalWrite(COLD_VALVE, HIGH);
}

void close_cold_valve(){
    SerialBT.println("close_cold_valve");
    digitalWrite(COLD_VALVE, LOW);
}

/* Hot Vavle */
void open_hot_valve(){
    SerialBT.println("turn_on_hot_valve");
    digitalWrite(HOT_VALVE, HIGH);
}

void close_hot_valve(){
    SerialBT.println("turn_off_hot_valve");
    digitalWrite(HOT_VALVE, LOW);
}

/* Drain Vavle */
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
    SerialBT.println("rotate_cw");
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

void loop()
{
    if (SerialBT.available())
    {
        // BTData = SerialBT.readString();
        // Total_Wash_Time = BTData.toInt();
        fill_water();
        wash(30 * MINUTE_TO_MILIS);
        pump_water_out();
        fill_water();
        wash(20 * MINUTE_TO_MILIS);
        pump_water_out();
        dry(10 * MINUTE_TO_MILIS);
    }

    /* 48 for 0 and 49 for 1 */
    // if(Total_Wash_Time == 1)
    // {
    //     Mode = START;
    // }
    // /* If received Character is 0, then turn OFF the LED */
    // if(BTData == 0)
    // {
    //     Mode = STOP;
    // }

    // if(Mode == START)
    // {
    //     fill_water();
    // }
    // else if (Mode == STOP)
    // {
    //     pump_out_water();
    // }
    
    // delay(1000);
}

void fill_water() {
    SerialBT.println("fill_water");
    close_drain_valve();
    open_cold_valve();
    open_hot_valve();
    while(digitalRead(PRESSURE_SENSOR) == LOW);
    close_cold_valve();
    close_hot_valve();
}

void pump_water_out() {
    SerialBT.println("pump_out_water");
    open_drain_valve();
    delay(45 * 1000);
    close_drain_valve();
}

void wash(int time) {
    SerialBT.println("washing");
    for(int i = 0; i < int(time / MINUTE_TO_MILIS); i++) {
        if(i % 2 == 0) {
            rotate_ccw();
        }
        else {
            rotate_cw();
        }
        rotate_slow();
        delay(MINUTE_TO_MILIS);
        stop();
        delay(2000);
    }
}

void dry(int time) {
    SerialBT.println("drying");
    for(int i = 0; i < int(time / MINUTE_TO_MILIS); i++) {
        if(i % 2 == 0) {
            rotate_ccw();
        }
        else {
            rotate_cw();
        }
        rotate_fast();
        delay(MINUTE_TO_MILIS);
        stop();
        delay(5000);
    }
    rotate_slow();
}