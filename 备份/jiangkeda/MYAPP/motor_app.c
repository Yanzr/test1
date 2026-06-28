#include "motor_app.h"
#include "tim.h"


void Motor_SetPWM(uint8_t n, int16_t Duty)
{
    if (n == 1)
    {
        if (Duty >= 0)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);

			pwm_set_duty(Duty,1);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

			pwm_set_duty(-Duty,1);
        }
    }
    else if (n == 2)
    {
        if (Duty >= 0)
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);

			pwm_set_duty(Duty,2);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
  
			pwm_set_duty(-Duty,2);
        }
    }
}
