#include "main.hpp"

void pid_2ms_loop_INST_IRQHandler()
{
    motor_right.update();
    motor_left.update();
}