#include "mbed.h"
#include "arm_math.h"
#include "adc.h"
#include "tim.h"

void smps_control_task();
void temperature_control_task();

Thread smpsthread;
Thread solderingthread;

DigitalOut led(PA_5);
AnalogIn u_smps(PA_0);

DigitalOut timing(PC_7);

Ticker fastcontrol;

PwmOut smps(PA_8);


/*** SMPS control loop ***/

volatile float u_soll = 5.0;
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
const float k = (10.0 + 1.0) / 1.0; // voltage divider
static float u_ist;
static float dutycycle;

void smps_control_task(){

    // while(true){
        timing = 1;

        // u_ist = k*Uref*u_smps.read();
        e = u_soll - u_ist;
        x[2] = x[1];
        x[1] = x[0];
        x[0] = e;

        y[2] = y[1];
        y[1] = y[0];
        // control algorithm
        y[0] = b0*x[0] + b1*x[1] + b2*x[2] - a1*y[1] - a2*y[2];

        if(y[0]>1.0) dutycycle = 1.0;
        else if(y[0]<0.0) dutycycle = 0.0;
        else dutycycle = y[0];

        smps.write(dutycycle);

        timing = 0;

        // wait_us(50);
    // }
}


/*** main ***/
volatile uint32_t isrcnt;
volatile uint16_t u_in;

int main(){
    printf("Soldering Station V1\n(c) Gerald Ebmer 2018\n\n");
    // printf("Sysclk frequency: %ld\n\n",SYSCLK_Frequency);

    // Init SMPS PWM
    smps.period_us(10); // 100kHz
    smps.write(0.5f); // duty cycle

      /*##-1- TIM Peripheral Configuration ######################################*/
    TIM_Config();

    /*##-2- Configure the ADC peripheral ######################################*/
    MX_ADC1_Init();

    /*##-4- Start the conversion process and enable interrupt ##################*/
    if (HAL_ADC_Start_IT(&hadc1) != HAL_OK)
    {
        /* Start Conversation Error */
        _Error_Handler(__FILE__,__LINE__);
    }

    /*##-3- TIM counter enable ################################################*/
    if (HAL_TIM_Base_Start(&htim) != HAL_OK)
    {
        /* Counter Enable Error */
        _Error_Handler(__FILE__,__LINE__);
    }


    // HAL_TIM_Base_Start_IT(&htim4);

    // HAL_ADC_Start_IT(&hadc1);
    
    // HAL_TIM_OC_Start_IT(&htim, TIM_CHANNEL_4); 


    // fastcontrol.attach(&smps_control_task,0.00005);

    //  smpsthread.start(smps_control_task);
    // solderingthread.start(temperature_control_task);
    // arm_iir_lattice_init_f32();


    while(true){
        wait(1);
        led = !led;
        printf("Uin: %f\nUsoll: %f\ndutycycle: %f\n\n",u_ist, u_soll, dutycycle);
        printf("isrcnt: %ld\n",isrcnt);
        printf("u_in: %d\n\n",u_in);
    }
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
    if(hadc == &hadc1){
        isrcnt++;
        u_in = HAL_ADC_GetValue(&hadc1);
    }
    // HAL_TIM_Base_Stop_IT(&htim4);
    // HAL_ADC_Stop_IT(&hadc1);
    // HAL_ADC_Stop_IT(&hadc1);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
    // printf("Hello");
    // HAL_ADC_Start_IT(&hadc1);
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


