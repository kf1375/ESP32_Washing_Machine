#include "BluetoothSerial.h"

#define     COLD_VALVE          13
#define     HOT_VALVE           14
#define     DRAIN_VALVE         27
#define     HEATER              26
#define     CCW                 25
#define     CW                  33
#define     SPEED               32

#define     PRESSURE_SENSOR     34
#define     BOTTOM_TERMOSTAT    39
#define     UP_TERMOSTAT        36

const unsigned long MINUTE_TO_MILIS = 60000UL;

BluetoothSerial SerialBT;
byte BTData;

bool TermoState;
long t0   = 0;

typedef enum {
    STOP    =   0,
    START   =   1,
    PAUSE   =   2,
} DeviceMode;

DeviceMode Mode;

/* Cold Valve */
void open_cold_valve(){
    // SerialBT.println("open_cold_valve");
    digitalWrite(COLD_VALVE, HIGH);
}

void close_cold_valve(){
    // SerialBT.println("close_cold_valve");
    digitalWrite(COLD_VALVE, LOW);
}

/* Hot Valve */
void open_hot_valve(){
    // SerialBT.println("open_hot_valve");
    digitalWrite(HOT_VALVE, HIGH);
}

void close_hot_valve(){
    // SerialBT.println("close_hot_valve");
    digitalWrite(HOT_VALVE, LOW);
}

/* Drain Valve */
void open_drain_valve(){
    // SerialBT.println("open_drain_valve");
    digitalWrite(DRAIN_VALVE, HIGH);
}

void close_drain_valve(){
    // SerialBT.println("close_drain_valve");
    digitalWrite(DRAIN_VALVE, LOW);
}

/* Heater */
void set_heater_on(){
    // SerialBT.println("set_heater_on");
    digitalWrite(HEATER, HIGH);
}

void set_heater_off(){
    // SerialBT.println("set_heater_off");
    digitalWrite(HEATER, LOW);
}

/* Direction */
void rotate_ccw() {
    // SerialBT.println("rotate_ccw");
    digitalWrite(CCW,   HIGH);
    digitalWrite(CW,    LOW);
}

void rotate_cw() {
    // SerialBT.println("rotate_cw");
    digitalWrite(CCW,   LOW);
    digitalWrite(CW,    HIGH);
}

void stop() {
    // SerialBT.println("stop");
    digitalWrite(CCW,   LOW);
    digitalWrite(CW,    LOW);
}

/* Speed */
void rotate_fast() {
    // SerialBT.println("rotate_fast");
    digitalWrite(SPEED, HIGH);
}

void rotate_slow(){
    // SerialBT.println("rotate_slow");
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
    // SerialBT.println("fill_water");
    open_cold_valve();
    open_hot_valve();
    //SerialBT.println(digitalRead(PRESSURE_SENSOR));
    while(digitalRead(PRESSURE_SENSOR) == LOW)
    {
      TermoState = digitalRead(UP_TERMOSTAT);
      if(TermoState) {
        close_hot_valve();
      }
      else {
        open_hot_valve();
      }
      delay(500);
    }
    delay(1000);
    // delay(135 * 1000);
    close_cold_valve();
    close_hot_valve();
}

void pump_water_out() {
    // SerialBT.println("pump_out_water");
    open_drain_valve();
    delay(52 * 1000);
    close_drain_valve();
}

void wash(int time) {
    // SerialBT.println("washing");
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
    // SerialBT.println("drying");
    open_drain_valve();
    rotate_slow();
    rotate_ccw();
    delay(2000);
    rotate_fast();
    rotate_cw();
    delay(time);
    close_drain_valve();
    rotate_slow();
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
        // Step 1
        SerialBT.println("Fill 1/5");
        fill_water();
        delay(5000);

        SerialBT.println("Wash 1/5");
        wash(5 * MINUTE_TO_MILIS);

        SerialBT.println("Pump 1/5");
        pump_water_out();
        delay(5000);

        // Step 2
        SerialBT.println("Fill 2/5");
        fill_water();
        delay(5000);

        SerialBT.println("Wash 2/5");
        wash(5 * MINUTE_TO_MILIS);

        SerialBT.println("Pump 2/5");
        pump_water_out();
        delay(5000);

        // Dry 1
        SerialBT.println("Dry 1/4");
        dry(2    * MINUTE_TO_MILIS);
        delay(5000);

        // Step 3
        SerialBT.println("Fill 3/5");
        fill_water();
        delay(5000);

        SerialBT.println("Wash 3/5");
        wash(5 * MINUTE_TO_MILIS);
        delay(5000);

        SerialBT.println("Pump 3/5");
        pump_water_out();
        delay(5000);

        // Dry 2
        SerialBT.println("Dry 2/4");
        dry(2    * MINUTE_TO_MILIS);
        delay(5000);
        
        // Step 4
        SerialBT.println("Fill 4/5");
        fill_water();
        delay(5000);

        SerialBT.println("Wash 4/5");
        wash(5 * MINUTE_TO_MILIS);
        delay(5000);

        SerialBT.println("Pump 4/5");
        pump_water_out();
        delay(5000);

        // Dry 3
        SerialBT.println("Dry 3/4");
        dry(2    * MINUTE_TO_MILIS);
        delay(5000);

        // Step 5
        SerialBT.println("Fill 5/5");
        fill_water();
        delay(5000);

        SerialBT.println("Wash 5/5");
        wash(5 * MINUTE_TO_MILIS);
        delay(5000);

        SerialBT.println("Pump 5/5");
        pump_water_out();
        delay(5000);

        // Dry 4
        SerialBT.println("Dry 4/4");
        dry(2    * MINUTE_TO_MILIS);
        delay(5000);
        
    }
}
