#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wpiExtensions.h>
#include <gertboard.h>
#include <piFace.h>
#include <pthread.h>

#include "pwm.h"
#include "../version.h"

#ifdef CONFIG_ORANGEPI
#include "OrangePi.h"
#include <wiringSerial.h>
#endif

struct pwm
{
    /* data */
    int pin;
    int period;
    int highInterval;
    int index;
    int enable;
};

struct pwm pwms[8];
int pwmsCount = 0;
pthread_mutex_t lock;
pthread_t thread;
int threadRun = 1;
int timerInterval = 10;


void* pwmRun(void* id)
{
    int sleepTime = timerInterval;
    while(threadRun)
    {
        usleep(sleepTime);
        pthread_mutex_lock(&lock); 
        if(pwmsCount == 0)
        {
           pthread_mutex_unlock(&lock);
            continue;
        }

        //update stat
	    int sleepTimeNext = 30*1000*1000;
        for(int i=0 ;i< pwmsCount;i++)
        {
            if(pwms[i].enable == 0)
            {
                continue;
            }
            pwms[i].index = (pwms[i].index + sleepTime) % pwms[i].period;
            if(pwms[i].index == 0)
            {
                digitalWrite(pwms[i].pin, HIGH);
		       //printf("set high");
            }else if(pwms[i].index == pwms[i].highInterval)
            {
                digitalWrite(pwms[i].pin, LOW);
                //printf("set low");
            }

	        int nextWeakup = pwms[i].index >= pwms[i].highInterval ? pwms[i].period - pwms[i].index : pwms[i].highInterval - pwms[i].index;

	        sleepTimeNext = sleepTimeNext > nextWeakup ? nextWeakup : sleepTimeNext; 
        }

	    sleepTime = sleepTimeNext;

        pthread_mutex_unlock(&lock);
    }
   return NULL;
}

int initPwm(int initPi)
{
    if(initPi)
    {
        if (wiringPiSetup()== -1)
        {
            printf("setup failed");
            return -1;
        }
    }

    pthread_mutex_init(&lock, NULL);
    pthread_create(&thread,NULL,pwmRun,NULL);
    return 0;
}

int disablePwm(int pin)
{
    pthread_mutex_lock(&lock);
    for(int i = 0;i<pwmsCount;i++)
    {
        if(pwms[i].pin == pin)
        {
            pwms[i].enable = 0;
            digitalWrite(pin, LOW);
        }
    }

    pthread_mutex_unlock(&lock);
    return 0;
}

void destoryPwm()
{
    threadRun = 0;
}

int setPwm(int pin, int period, int highInterval)
{
    pthread_mutex_lock(&lock); 
    int i = 0;
    for(;i<pwmsCount;i++)
    {
        if(pwms[i].pin == pin)
        {
            int lastPeriod = pwms[i].period, lastHighInterval = pwms[i].highInterval;
            pwms[i].period = period - period%timerInterval;
            pwms[i].highInterval = highInterval - highInterval%timerInterval;
            if(lastPeriod != pwms[i].period || lastHighInterval != pwms[i].highInterval)
            {
                pwms[i].index=0;
            }
            pwms[i].enable=1;
            break;
        }
    }

    //add new
    if(i>= pwmsCount)
    {
        pinMode(pin, OUTPUT);
        pwms[pwmsCount].pin = pin;
        pwms[pwmsCount].period = period - period%timerInterval;
        pwms[pwmsCount].highInterval = highInterval - highInterval%timerInterval;
        pwms[pwmsCount].index = 0;
        pwms[pwmsCount].enable = 1;
        pwmsCount++;
    }
   
    pthread_mutex_unlock(&lock);
    return 0;
}


