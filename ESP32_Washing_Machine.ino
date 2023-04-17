#include "BluetoothSerial.h"
#include <StopWatch.h>

#define ON 1
#define OFF 0

#define INPUT_SSR 23

#define COLD_VALVE 13
#define HOT_VALVE 14
#define DRAIN_VALVE 27
#define HEATER 26
#define CCW 25
#define CW 33
#define SPEED 32

#define PRESSURE_SENSOR 34
#define BOTTOM_TERMOSTAT 39
#define UP_TERMOSTAT 36

BluetoothSerial SerialBT;
byte BTData;

bool TermoState;
bool PressureState;

StopWatch SW(StopWatch::MINUTES);

typedef enum
{
    STOP = 0,
    START = 1,
    PAUSE = 2,
} DeviceMode;

DeviceMode Mode;

/* Cold Valve */
void open_cold_valve()
{
    // SerialBT.println("open_cold_valve");
    digitalWrite(COLD_VALVE, HIGH);
}

void close_cold_valve()
{
    // SerialBT.println("close_cold_valve");
    digitalWrite(COLD_VALVE, LOW);
}

/* Hot Valve */
void open_hot_valve()
{
    // SerialBT.println("open_hot_valve");
    digitalWrite(HOT_VALVE, HIGH);
}

void close_hot_valve()
{
    // SerialBT.println("close_hot_valve");
    digitalWrite(HOT_VALVE, LOW);
}

/* Drain Valve */
void open_drain_valve()
{
    // SerialBT.println("open_drain_valve");
    digitalWrite(DRAIN_VALVE, HIGH);
}

void close_drain_valve()
{
    // SerialBT.println("close_drain_valve");
    set_input_ssr(OFF);
    digitalWrite(DRAIN_VALVE, LOW);
}

/* Heater */
void set_heater_on()
{
    // SerialBT.println("set_heater_on");
    digitalWrite(HEATER, HIGH);
}

void set_heater_off()
{
    // SerialBT.println("set_heater_off");
    digitalWrite(HEATER, LOW);
}

/* Direction */
void rotate_ccw()
{
    // SerialBT.println("rotate_ccw");
    digitalWrite(CCW, HIGH);
    digitalWrite(CW, LOW);
}

void rotate_cw()
{
    // SerialBT.println("rotate_cw");
    digitalWrite(CCW, LOW);
    digitalWrite(CW, HIGH);
}

void stop()
{
    // SerialBT.println("stop");
    digitalWrite(CCW, LOW);
    digitalWrite(CW, LOW);
}

/* Speed */
void rotate_fast()
{
    // SerialBT.println("rotate_fast");
    digitalWrite(SPEED, HIGH);
}

void rotate_slow()
{
    // SerialBT.println("rotate_slow");
    digitalWrite(SPEED, LOW);
}

void setup()
{
    // put your setup code here, to run once:
    pinMode(INPUT_SSR, OUTPUT);
    pinMode(COLD_VALVE, OUTPUT);
    pinMode(HOT_VALVE, OUTPUT);
    pinMode(DRAIN_VALVE, OUTPUT);
    pinMode(HEATER, OUTPUT);
    pinMode(CCW, OUTPUT);
    pinMode(CW, OUTPUT);
    pinMode(SPEED, OUTPUT);

    pinMode(PRESSURE_SENSOR, INPUT);
    pinMode(BOTTOM_TERMOSTAT, INPUT);
    pinMode(UP_TERMOSTAT, INPUT);

    SerialBT.begin();
}

void fill_water()
{
    // SerialBT.println("fill_water");
    // SerialBT.println(digitalRead(PRESSURE_SENSOR));
    set_input_ssr(ON);
    
    set_heater_on();
    bool fill_state = true;
    while (fill_state)
    {
        PressureState = digitalRead(PRESSURE_SENSOR);
        TermoState = digitalRead(UP_TERMOSTAT);
        if (PressureState && TermoState) fill_state = false;
        
        if (PressureState == LOW)
        {
            open_cold_valve();
            open_hot_valve();
        }
        else
        {
            close_cold_valve();
            close_hot_valve();
        }

        if (TermoState == LOW)
        {
            set_heater_on();
        }
        else
        {
            set_heater_off();
        }
        milis_delay(500);
    }
    set_input_ssr(OFF);
    close_cold_valve();
    close_hot_valve();
    set_heater_off();
}

void pump_water_out()
{
    // SerialBT.println("pump_out_water");
    open_drain_valve();
    set_input_ssr(ON);
    milis_delay(52 * 1000);
    close_drain_valve();
}

void wash(uint32_t time)
{
    // SerialBT.println("washing");
    uint32_t elapsed_time = 0;
    SW.start();
    for (;;)
    {
        elapsed_time = SW.elapsed();
        if (elapsed_time == time)
            break;
        rotate_slow();
        rotate_cw();
        set_input_ssr(ON);
        milis_delay(8 * 1000);
        set_input_ssr(OFF);
        stop();
        milis_delay(2000);
        rotate_slow();
        rotate_ccw();
        set_input_ssr(ON);
        milis_delay(8 * 1000);
        set_input_ssr(OFF);
        stop();
        milis_delay(2000);
    }
    set_input_ssr(OFF);
    stop();
    SW.stop();
    SW.reset();
}

void dry(uint32_t time)
{
    uint32_t elapsed_time = 0;
    SW.start();
    open_drain_valve();
    rotate_slow();
    rotate_ccw();
    set_input_ssr(ON);
    milis_delay(2 * 1000);
    set_input_ssr(OFF);
    stop();
    rotate_fast();
    for (;;)
    {
        elapsed_time = SW.elapsed();
        if (elapsed_time == time)
            break;
        rotate_cw();
        set_input_ssr(ON);
    }
    SW.stop();
    SW.reset();
    set_input_ssr(OFF);
    rotate_slow();
    stop();
    close_drain_valve();
}

void loop()
{
    if (SerialBT.available())
    {
        BTData = SerialBT.read();
    }

    if (BTData == '1')
    {
        do_work();
    }
}

void milis_delay(unsigned long duration)
{
    unsigned long time = millis();
    while (millis() < time + duration)
        ;
}

void do_work()
{
    // Step 1
    SerialBT.println("Fill 1/6");
    fill_water();
    SerialBT.println("Wash 1/6");
    wash(5);
    SerialBT.println("Pump 1/6");
    pump_water_out();

    // Step 2
    SerialBT.println("Fill 2/6");
    fill_water();
    SerialBT.println("Wash 2/6");
    wash(5);
    SerialBT.println("Pump 2/6");
    pump_water_out();

    // Dry 1
    SerialBT.println("Dry 1/5");
    dry(2);

    // Step 3
    SerialBT.println("Fill 3/6");
    fill_water();
    SerialBT.println("Wash 3/6");
    wash(5);
    SerialBT.println("Pump 3/6");
    pump_water_out();

    // Dry 2
    SerialBT.println("Dry 2/5");
    dry(2);

    // Step 4
    SerialBT.println("Fill 4/6");
    fill_water();
    SerialBT.println("Wash 4/6");
    wash(5);
    SerialBT.println("Pump 4/6");
    pump_water_out();

    // Dry 3
    SerialBT.println("Dry 3/5");
    dry(2);

    // Step 5
    SerialBT.println("Fill 5/6");
    fill_water();
    SerialBT.println("Wash 5/6");
    wash(5);
    SerialBT.println("Pump 5/6");
    pump_water_out();

    // Dry 4
    SerialBT.println("Dry 4/5");
    dry(3);

    // Step 6
    SerialBT.println("Fill 6/6");
    fill_water();
    SerialBT.println("Wash 6/6");
    wash(5);
    SerialBT.println("Pump 6/6");
    pump_water_out();

    // Dry 5
    SerialBT.println("Dry 5/5");
    dry(3);
}

void set_input_ssr(bool state)
{
    milis_delay(400);
    digitalWrite(INPUT_SSR, state);
    milis_delay(400);
}