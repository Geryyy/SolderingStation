#include "mbed.h"
#include "arm_math.h"

void smps_control_task();
void temperature_control_task();

Thread smpsthread;
Thread solderingthread;

DigitalOut led(PA_5);
AnalogIn u_smps(PA_0);

DigitalOut timing(PC_7);

PwmOut smps(PA_8);
int main(){
    printf("Soldering Station V1\n(c) Gerald Ebmer 2018\n\n");

    // Init SMPS PWM
    smps.period(1e-5f); // 100kHz
    smps.write(0.5f); // duty cycle

    smpsthread.start(smps_control_task);
    solderingthread.start(temperature_control_task);
    // arm_iir_lattice_init_f32();


    while(true){
        wait(1);
        led = !led;
    }
}

arm_iir_lattice_instance_f32 F1d;

volatile float u_soll = 5.0;

void smps_control_task(){
    const float Ta = 50e-6; // 50 us

    const float b0 = 0.0f;
    const float b1 = 0.2724050f;
    const float b2 = 0.1613566f;

    const float a1 = -1.3304278f;
    const float a2 = 0.7641896f;

    static float x[3] = {0};
    static float y[3] = {0};
    
    float e;
    static float Uref = 3.3;
    static float k = (6.8 + 1.0) / 1.0; // voltage divider

    while(true){
        timing = 1;
        
        e = u_soll - k*Uref*smps.read();
        x[2] = x[1];
        x[1] = x[0];
        x[0] = e;

        y[2] = y[1];
        y[1] = y[0];
        // control algorithm
        y[0] = b0*x[0] + b1*x[1] + b2*x[2] - a1*y[1] - a2*y[2];

        if(y[0]>1.0) y[0] = 1.0;
        if(y[0]<0.0) y[0] = 0.0;
        smps.write(y[0]);

        timing = 0;

        wait(Ta);
    }
}

void temperature_control_task(){

    while(true){

        wait_ms(1);
    }
}