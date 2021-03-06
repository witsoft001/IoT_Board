/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-23     balanceTWK   first implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

enum
{
    MOTOR_STOP,
    MOTOR_LEFT,
    MOTOR_RIGHT
};

void motor_ctrl(rt_uint8_t turn)
{
    if (turn == MOTOR_STOP)
    {
        rt_pin_write(PIN_MOTOR_A, PIN_LOW);
        rt_pin_write(PIN_MOTOR_B, PIN_LOW);
    }
    else if (turn == MOTOR_LEFT)
    {
        rt_pin_write(PIN_MOTOR_A, PIN_LOW);
        rt_pin_write(PIN_MOTOR_B, PIN_HIGH);
    }
    else if (turn == MOTOR_RIGHT)
    {
        rt_pin_write(PIN_MOTOR_A, PIN_HIGH);
        rt_pin_write(PIN_MOTOR_B, PIN_LOW);
    }
    else
    {
        rt_kprintf("err parameter ! Please enter 0-2. \n");
    }
}

void beep_ctrl(rt_uint8_t on)
{
    if (on)
    {
        rt_pin_write(PIN_BEEP, PIN_HIGH);
    }
    else
    {
        rt_pin_write(PIN_BEEP, PIN_LOW);
    }
}

/*interrupt callback function*/
void irq_callback(void *args)
{
    rt_uint32_t sign = (rt_uint32_t)args;
    switch (sign)
    {
    case PIN_KEY0:
        motor_ctrl(MOTOR_LEFT);
        rt_kprintf("KEY0 interrupt. motor turn left.\n");
        break;
    case PIN_KEY1:
        motor_ctrl(MOTOR_RIGHT);
        rt_kprintf("KEY1 interrupt. motor turn right.\n");
        break;
    case PIN_KEY2:
        motor_ctrl(MOTOR_STOP);
        rt_kprintf("KEY2 interrupt. motor stop.\n");
        break;
    default:
        rt_kprintf("error sign= %d !\n", sign);
        break;
    }
}

int main(void)
{
    unsigned int count = 1;

    /* set key pin mode to input */
    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT_PULLDOWN);

    /* set motor pin mode to output */
    rt_pin_mode(PIN_MOTOR_A, PIN_MODE_OUTPUT);
    rt_pin_mode(PIN_MOTOR_B, PIN_MODE_OUTPUT);

    /* set beep pin mode to output */
    rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);

    /* set interrupt mode and attach interrupt callback function */
    rt_pin_attach_irq(PIN_KEY0, PIN_IRQ_MODE_FALLING, irq_callback, (void *)PIN_KEY0);
    rt_pin_attach_irq(PIN_KEY1, PIN_IRQ_MODE_FALLING, irq_callback, (void *)PIN_KEY1);
    rt_pin_attach_irq(PIN_KEY2, PIN_IRQ_MODE_FALLING, irq_callback, (void *)PIN_KEY2);

    /* enable interrupt */
    rt_pin_irq_enable(PIN_KEY0, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(PIN_KEY1, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(PIN_KEY2, PIN_IRQ_ENABLE);

    while (count > 0)
    {
        if (rt_pin_read(PIN_WK_UP) == PIN_HIGH)
        {
            rt_thread_mdelay(50);
            if (rt_pin_read(PIN_WK_UP) == PIN_HIGH)
            {
                rt_kprintf("WK_UP pressed. beep on.\n");
                beep_ctrl(1);
            }
        }
        else
        {
            beep_ctrl(0);
        }
        rt_thread_mdelay(10);
        count++;
    }
    return 0;
}
