#include <stdio.h>
#include <wiringPi.h>
#include <wpiExtensions.h>
#include <gertboard.h>
#include <piFace.h>
#include <pthread.h>

#include "controller.h"
#include "pwd.h"

#define DIRECTION_PIN 5
#define DIRECTION_PWM_PERIOD 20 * 1000
#define DIRECTION_PWM_HIGH_MAX 2500
#define DIRECTION_PWM_HIGH_MIN 2500

#define SPEED_PIN_PWM 3
#define SPEED_PIN_FRONT 4
#define SPEED_PIN_BACK 6
#define SPEED_PWM_PERIOD 100 * 1000

int initController()
{
    if (wiringPiSetup() == -1)
    {
        printf("setup failed");
        return -1;
    }

    pinMode(SPEED_PIN_FRONT, OUTPUT);
    pinMode(SPEED_PIN_BACK, OUTPUT);
    initPwm(0);
}

int initResponse(char *output, int len, int code, int status)
{
    *(output) = 0x7e;
    *(output + 1) = len;
    *(output + 2) = code;
    *(output + 3) = status;
}

// dir 0: left, 1:right
int changeDirection(int dir, int angle, char *output, int cap)
{
    int angleDir = dir == 0 ? -angle : angle;
    int time = 500 + ((angleDir + 90) * 2000) / 180;
    setPwm(DIRECTION_PIN, DIRECTION_PWM_PERIOD, time);
    if (cap < 4)
    {
        return 0;
    }
    initResponse(output, 4, 0x00, 0x00);
    return 4;
}
// dir 0:front, 1:back
int changeSpeed(int dir, int speed, char *output, int cap)
{
    //stop
    if(speed == 0)
    {
        digitalWrite(SPEED_PIN_BACK, LOW);
        digitalWrite(SPEED_PIN_FRONT, LOW);
        disablePwm(SPEED_PIN_PWM);

        initResponse(output, 4, 0x01, 0x00);
        return 4;
    }

    int time = (SPEED_PWM_PERIOD * speed) / 10;
    setPwm(SPEED_PIN_PWM, SPEED_PWM_PERIOD, time);
    if (dir == 0)
    {
        digitalWrite(SPEED_PIN_BACK, LOW); 
        digitalWrite(SPEED_PIN_FRONT, HIGH);
    }
    else
    {
        digitalWrite(SPEED_PIN_FRONT, LOW);
        digitalWrite(SPEED_PIN_BACK, HIGH);
    }

    if (cap < 4)
    {
        return 0;
    }

    initResponse(output, 4, 0x01, 0x00);
    return 4;
}

// 0x7e len code data
// code:0x00,方向 d0:r or left, d1:angle；0x01, 速率, d0: front or back, d1:speed
//忽略粘包
int procCmd(char *input, int len, char *output, int cap)
{
    int outlen = 0;
    int i = 0;
    for (; i < len; i++)
    {
        if (*(input + i) == 0x7e)
        {
            if (i + *(input + i + 1) <= len)
            {
                break;
            }
        }
    }
    if (i >= len)
    {
        return 0;
    }

    char *frame = input + i;
    switch (*(frame + 2))
    {
    case 0x00:
        outlen = changeDirection(*(frame + 3), *(frame + 4), output, cap);
        break;
    case 0x01:
        outlen = changeSpeed(*(frame + 3), *(frame + 4), output, cap);
        break;
    default:
        return 0;
    }

    return outlen;
}
