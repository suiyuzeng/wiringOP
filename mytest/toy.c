#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>
#include <wpiExtensions.h>

#include <gertboard.h>
#include <piFace.h>

#include "../version.h"

#ifdef CONFIG_ORANGEPI
#include "OrangePi.h"
#include <wiringSerial.h>
#endif

static void myTest2()
{
    initPwm(1);
    pinMode(4, OUTPUT);
    pinMode(6, OUTPUT);
    digitalWrite(6, LOW);
    setPwm(3, 50000, 10000);

    for(int i=0;i<10;i++)
    {
    digitalWrite(4, HIGH);
    sleep(3);
    digitalWrite(4, LOW);
    sleep(2);
    
    digitalWrite(6, HIGH);
    sleep(3);
    digitalWrite(6, LOW);
    sleep(2);
    }

    //int time=500;
    //setPwm(4, 20000, 500);

    //sleep(5);

    //disablePwm(3);
    //sleep(5);
    //setPwm(3, 2000000, 1000000);

    while(1)
    {
        sleep(1000);
        /*
        setPwm(4, 20000, time);
        time = time + 200;
        if(time > 2300)
            time = 500;
        sleep(2);
        */
    }
}

int main (int argc, char *argv [])
{
    //myTest2();
	initController();
    if(initNet(12522)!=0)
    {
        printf("init net failed\n");
        return -1;
    }

    while(1)
    {
        sleep(1);
    }
    printf("hello world");
}
