#ifndef PID_GUARD
#define PID_GUARD

#include "stdint.h"

/** Generic PID controller
 * 
 * This code is free for usage and modifications.
 * Use it wisely at our own risk.
 * 
 * 
 * all times are in milliseconds
 * TODO: usage
 */

struct pid_controller_t
{
    uint32_t last_time;
    double output;
    double setpoint;
    double i_term;
    double last_input;
    double kp, ki, kd;
    int32_t sample_time;
    double out_min;
    double out_max;
};

void pid_compute(struct pid_controller_t *p, uint32_t current_millis, double input);
void pid_init_controller(struct pid_controller_t *p);
void pid_set_current_time(struct pid_controller_t *p, int32_t millis);
void pid_set_sample_time(struct pid_controller_t *p, int32_t new_sample_time);
void pid_set_tunings(struct pid_controller_t *p, double kp, double ki, double kd);
void pid_set_output_limits(struct pid_controller_t *p, double min, double max);
void pid_set_setpoint(struct pid_controller_t *p, double value);
double pid_get_output(struct pid_controller_t *p);

#endif // PID_GUARD
