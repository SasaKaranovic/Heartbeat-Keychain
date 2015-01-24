/* 
 * File:   main.c
 * Author: Sale
 *
 * Created on January 23, 2015, 11:21 PM
 */

#include "main.h"

volatile uint8_t currentDC       = 0;

volatile uint8_t targetRed       = 0;
volatile uint8_t targetGreen     = 0;
volatile uint8_t targetBlue      = 0;

volatile uint8_t fadeSide       = 1;
volatile uint8_t heartBeatCount = 0;
volatile uint8_t delay          = 0;


volatile uint16_t TimeBase       = 0;
volatile uint8_t CapNominalDone = 0;
volatile uint8_t CapNominalFreq = 0;
volatile uint8_t CapCurrentFreq = 0;
volatile uint8_t CapTresholdFreq = 30;



/*
 * 
 */
void main() {
    InitializeDevice();
    SetUpCapsense();
    PR2 = 50;

    while(1) {
        if( (delay > 5) &&  (heartBeatCount > 0) ) {
            
            if(fadeSide == 1) {
                targetRed   = (targetRed < 254)     ? FadeInOut(targetRed,1)     : 0;
                targetGreen = (targetGreen < 254)   ? FadeInOut(targetGreen,1)   : 0;
                targetBlue  = (targetBlue < 254)    ? FadeInOut(targetBlue,1)    : 0;
            }
            else {
                targetRed   = (targetRed > 0)     ? FadeInOut(targetRed,0)     : 0;
                targetGreen = (targetGreen > 0)   ? FadeInOut(targetGreen,0)   : 0;
                targetBlue  = (targetBlue > 0)    ? FadeInOut(targetBlue,0)    : 0;
            }


            if(targetRed >= 254 || targetGreen >= 254 || targetBlue >= 254) {
                fadeSide = 0;
            }

            if(targetRed == 0 && targetGreen == 0 && targetBlue == 0) {
                fadeSide = 1;
                heartBeatCount--;
            }

            delay = 0;
        }
    }
}


void interrupt ISRRoutine(void)
{
    //Timer0 interrupt
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;

        if(CapNominalDone < 10) {
            if(CapNominalDone == 0) { CapNominalFreq = TimeBase; }
            CapNominalFreq = (CapNominalFreq + TimeBase) /2;

            ++CapNominalDone;
            TimeBase = 0;
        }
        else {
            CapCurrentFreq = TimeBase;

            if (CapCurrentFreq < (CapNominalFreq - CapTresholdFreq) ) {
                if(heartBeatCount == 0) {
                    heartBeatCount = 2;
                }
            }

        }

        TMR0 = 0;
        TimeBase = 0;
        return;
    }

    //Timer2 interrupt
    if(PIE1bits.TMR2IE && PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        //INTCONbits.PEIE = 0;
        currentDC += 1;
        TimeBase += 1;
        delay += 1;
        
        if(currentDC >= 254) { currentDC = 0; }

        if(currentDC < targetRed) { LEDRED = 1; }
        else { LEDRED = 0; }

        if(currentDC < targetGreen) { LEDGREEN = 1; }
        else { LEDGREEN = 0; }

        if(currentDC < targetBlue) { LEDBLUE = 1; }
        else { LEDBLUE = 0; }

        //TMR2 = 0;
        //INTCONbits.PEIE = 1;
        return;
    }
}


uint8_t FadeInOut(uint8_t current, uint8_t up) {
    uint8_t add = 0;

    if(up == 1) {
        if( current >= 250 ) { add = 1; }
        else if( current >= 200 ) { add = 10; }
        else if (current >= 100) { add = 5; }
        else if (current >= 50)  { add = 2; }
        else { add = 1; }
        return (current+add);
    }
    else {
        if( current >= 200 ) { add = 10; }
        else if (current >= 100) { add = 5; }
        else if (current >= 50)  { add = 2; }
        else { add = 1; }

        return (current-add);
    }
}



