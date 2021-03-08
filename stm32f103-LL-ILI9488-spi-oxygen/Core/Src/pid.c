#include "pid.h"

void pid_compute(struct pid_controller_t *p, uint32_t current_millis, double input)
{
  int32_t time_change = (current_millis - p->last_time);
  if (time_change >= p->sample_time)
  {
    /*Compute all the working error variables*/
    double error = p->setpoint - input;
    p->i_term += (p->ki * error);
    if (p->i_term > p->out_max)
    {
      p->i_term = p->out_max;
    }
    else if (p->i_term < p->out_min)
    {
      p->i_term = p->out_min;
    }
    double dInput = (input - p->last_input);
    /*Compute PID Output*/
    p->output = p->kp * error + p->i_term - p->kd * dInput;
    if (p->output > p->out_max)
    {
      p->output = p->out_max;
    }
    else if (p->output < p->out_min)
    {
      p->output = p->out_min;
    }
    /*Remember some variables for next time*/
    p->last_input = input;
    p->last_time = current_millis;
  }
}

void pid_init_controller(struct pid_controller_t *p)
{
  p->i_term = 0.0f;
  p->output = 0.0f;
  p->setpoint = 0.0f;
  p->last_time = 0.0f;
  p->sample_time = 10;
  p->out_min = 0.0f;
  p->out_max = 100.0f;
  p->last_input = 0.0f;
}

void pid_set_current_time(struct pid_controller_t *p, int32_t millis)
{
  p->last_time = millis;
}

void pid_set_sample_time(struct pid_controller_t *p, int32_t new_sample_time)
{
  if (new_sample_time > 0)
  {
    double ratio = (double)new_sample_time / (double)p->sample_time;
    p->ki *= ratio;
    p->kd /= ratio;
    p->sample_time = (uint32_t)new_sample_time;
  }
}

void pid_set_tunings(struct pid_controller_t *p, double kp, double ki, double kd)
{
  // double sample_time_in_sec = ((double)p->sample_time) / 1000.0f;
  p->kp = kp;
  p->ki = ki; // sample_time_in_sec;
  p->kd = kd; // sample_time_in_sec;
}

void pid_set_output_limits(struct pid_controller_t *p, double min, double max)
{
  if (min > max)
  {
    return;
  }
  p->out_min = min;
  p->out_max = max;
  if (p->output > p->out_max)
  {
    p->output = p->out_max;
  }
  else if (p->output < p->out_min)
  {
    p->output = p->out_min;
  }
  if (p->i_term > p->out_max)
  {
    p->i_term = p->out_max;
  }
  else if (p->i_term < p->out_min)
  {
    p->i_term = p->out_min;
  }
}

void pid_set_setpoint(struct pid_controller_t *p, double value)
{
  p->setpoint = value;
}

double pid_get_output(struct pid_controller_t *p)
{
  return p->output;
}
