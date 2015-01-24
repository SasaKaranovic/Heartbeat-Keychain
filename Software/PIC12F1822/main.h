/* 
 * File:   main.h
 * Author: Sale
 *
 * Created on January 23, 2015, 11:22 PM
 */

#ifndef MAIN_H
#define	MAIN_H

//Get rid of those ugly unable to resolve __delay_ms, __delay_us errors that aren't exactly erros but bugs in IDE
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000ULL
#endif

#if 1 // to fix stupid IDE error issues with __delay_ms
#ifndef _delay_ms(x)
#define _delay_ms(x) __delay_ms(x)
#endif
#ifndef _delay_us(x)
#define _delay_us(x) __delay_us(x)
#endif
#endif

#include <xc.h>
#include <stdint.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = OFF      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)


//Definicije
#define TRISDAT     TRISAbits.TRISA5
#define TRISCLK     TRISAbits.TRISA4
#define TRISREGCLK  TRISAbits.TRISA2

#define LEDRED      LATAbits.LATA0
#define LEDGREEN    LATAbits.LATA1
#define LEDBLUE     LATAbits.LATA2


void InitializeDevice();
void SetUpCapsense();
uint8_t FadeInOut(uint8_t current, uint8_t up);
#define Timer2_On     (T2CONbits.TMR2ON = 1;)
#define Timer2_Off    (T2CONbits.TMR2ON = 0;)


void InitializeDevice() {
    /*
            Configure device for 8MHz
     */
    //SCS<1:0>: System Clock Select bits
    OSCCONbits.SCS      = 0b00;     //00 = Clock determined by FOSC<2:0> in Configuration Word 1.

    //IRCF<3:0>: Internal Oscillator Frequency Select bits
    OSCCONbits.IRCF     = 0b1110;   //1110 = 8MHz HF

    //SPLLEN: Software PLL Enable bit   (If PLLEN in Configuration Word 1 = 0:)
    OSCCONbits.SPLLEN   = 0;        //0 = 4x PLL is disabled

    //Configure pins
    ANSELAbits.ANSA0   = 0;
    ANSELAbits.ANSA1   = 0;
    ANSELAbits.ANSA2   = 0;
    ANSELAbits.ANSA4   = 1;
    TRISAbits.TRISA4    = 1;
    OPTION_REGbits.nWPUEN   = 1;
    WPUA                = 0;

    TRISAbits.TRISA0    = 0;
    TRISAbits.TRISA1    = 0;
    TRISAbits.TRISA2    = 0;

    LATAbits.LATA0      = 0;
    LATAbits.LATA1      = 0;
    LATAbits.LATA2      = 0;

    /*
            Timer1 Interrupt and Enable
     */
//    PIE1bits.TMR1IE     = 1;
//    T1CONbits.TMR1CS    = 0b00; //00 =Timer1 clock source is instruction clock (FOSC/4)
//    T1CONbits.T1CKPS    = 0b11; //11 = 1:8 Prescale value
//    T1CONbits.TMR1ON    = 1;    //1 = Enables Timer1


    /*
            Timer2 Setup for heartbeat effect
     */
    //T2OUTPS<3:0>: Timer2 Output Postscaler Select bits
    T2CONbits.T2OUTPS   = 0b0000;   //0000 = 1:1 Postscaler

    //T2CKPS<1:0>: Timer2 Clock Prescale Select bits
    T2CONbits.T2CKPS    = 0b00;     //00 = Prescaler is 1

    //TMR2ON: Timer2 On bit
    T2CONbits.TMR2ON    = 1;        //1 = Timer2 is on


    /*
            Configure INTERRUPT CONTROL REGISTER
    */
    PIE1bits.TMR2IE = 1;    //1 = Enables the Timer2 to PR2 match interrupt
    INTCONbits.PEIE = 1;    //1 = Enables all active peripheral interrupts
    INTCONbits.GIE  = 1;    //1 = Enables all active interrupts
}


void SetUpCapsense() {
    CPSCON0bits.CPSON   = 1;    //1 = CPS module is enabled
    CPSCON0bits.CPSRM   = 0;    //0 = CPS module is in the low range. Internal oscillator voltage references are used.
    CPSCON0bits.CPSRNG  = 0b10; //10 = Oscillator is in Medium Range. Charge/Discharge Current is nominally 1.2 ?A
    CPSCON1bits.CPSCH   = 0b0011;   //0011 = channel 3, (CPS3) -> RA4

    OPTION_REGbits.TMR0CS   = 1 ;    //0 = Internal instruction cycle clock (FOSC/4)
    OPTION_REGbits.PSA      = 0;    //0 = Prescaler is assigned to the Timer0 module
    CPSCON0bits.T0XCS       = 1;    //

    INTCONbits.T0IE         = 1;    //
}


#endif	/* MAIN_H */

