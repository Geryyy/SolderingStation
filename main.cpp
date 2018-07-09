#include "mbed.h"
#include "arm_math.h"
#include "adc.h"
#include "tim.h"
#include "smps.h"

void smps_control_task();
void temperature_control_task();

Thread solderingthread;

DigitalOut led(PA_5);
AnalogIn u_smps(PA_0);
PwmOut pwm_smps(PA_8);



/*** main ***/
volatile float smps_u_ist;
volatile float smps_u_soll;
volatile float smps_u_pwm;

int main(){
    printf("Soldering Station V1\n(c) Gerald Ebmer 2018\n\n");

    // Init SMPS PWM
    pwm_smps.period_us(10); // 100kHz
    pwm_smps.write(0.5f); // duty cycle

    TIM_Config();
    MX_ADC1_Init();

    if (HAL_ADC_Start_IT(&hadc1) != HAL_OK)    {
        _Error_Handler(__FILE__,__LINE__);
    }

    if (HAL_TIM_Base_Start(&htim) != HAL_OK)    {
        _Error_Handler(__FILE__,__LINE__);
    }


    // solderingthread.start(temperature_control_task);

    while(true){
        wait(1);
        led = !led;
        printf("Uin: %f\nUsoll: %f\ndutycycle: %f\n\n",smps_u_ist, smps_u_soll, smps_u_pwm);
    }
}


DigitalOut TimingPin(PC_8);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
    if(hadc == &hadc1){
        TimingPin = 1;
        smps_control_task();
        TimingPin = 0;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
}

void smps_control_task(){
    static float u_pwm = 0.0;
    const float u_soll = 5.0;
    const float u_ist = k*Uref*( (float)HAL_ADC_GetValue(&hadc1) / 4095.0);

    u_pwm = smps_controller(u_soll, u_ist);
    u_pwm = limiter(u_pwm);
    pwm_smps.write(u_pwm);    
    
    smps_u_ist = u_ist;
    smps_u_soll = u_soll;
    smps_u_pwm = u_pwm;
}

void temperature_control_task(){
    while(true){
        wait_ms(1);
    }
}


void _Error_Handler(char* file, int line){
    fprintf(stderr, "Error in %s line %d\n",file, line);
    while(true);
}


