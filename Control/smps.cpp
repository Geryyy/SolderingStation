
const float Uref = 3.3;
const float k = (10.0 + 1.0) / 1.0; // voltage divider

float smps_controller(float u_soll, float u_ist){
    // const float Ta = 50e-6; // 50 us
    const float b0 = 0.0f;
    const float b1 = 0.2724050f;
    const float b2 = 0.1613566f;
    const float a1 = -1.3304278f;
    const float a2 = 0.7641896f;
    static float x[3] = {0};
    static float y[3] = {0};
    const float e = u_soll - u_ist;

    // time shift
    x[2] = x[1];
    x[1] = x[0];
    x[0] = e;

    y[2] = y[1];
    y[1] = y[0];

    // control algorithm
    y[0] = b0*x[0] + b1*x[1] + b2*x[2] - a1*y[1] - a2*y[2];
    
    return y[0];
}

float limiter(float x){
    float y;
    if(x>1.0) y = 1.0;
    else if(x<0.0) y = 0.0;
    else y = x;
    return y;
}

