

#ifndef __smps_H
#define __smps_H

extern const float Uref = 3.3;
extern const float k = (10.0 + 1.0) / 1.0; // voltage divider

float smps_controller(float u_soll, float u_ist);
float limiter(float x);

#endif