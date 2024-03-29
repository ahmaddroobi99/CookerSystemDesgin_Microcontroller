/*
 * File:   mainFile.c
 * Author: hp
 *
 * Created on April 24, 2021, 4:34 PM
 */

#define _XTAL_FREQ   4000000UL     // needed for the delays, set to 4 MH= your crystal frequency
// CONFIG1H
#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#include <xc.h>
#include <stdio.h>
#include "my_ser.h"
#include "my_adc.h"
#include "lcd_x8.h"
#include <xc.h>

#define STARTVALUE  3036
float HS = 1;
int mode =0;
float temp, sp ;
float tempf;
float spf;
char Buffer[32];
unsigned int RPS_count=0;
unsigned int RPS=0;
int ft=0;
int flageBuzz; 
void clockNormalMode(void);
signed short seconds = 20;
signed short minutes = 0;
signed short hours   = 1;
unsigned short clkMode = 1;
unsigned short stpMode = 0;
unsigned short initial = 0; // Initial State
//char* m3="Count Heat";
void reloadTimer0(void)
{  
    TMR3H = (unsigned char) ((STARTVALUE >>  8) & 0x00FF);
    TMR3L =  (unsigned char)(STARTVALUE & 0x00FF );   
}

void Timer0_isr(void)
{
  
    
    INTCONbits.TMR0IF=0;
   // PORTCbits.RC5=!(PORTCbits.RC5);
    ft=1;
    reloadTimer0();
  
}
void EXT_Int0_isr(void)
{
    INTCON3bits.INT1IF=0;
    if(mode==5)
    
    {
        mode=0;
    }
    else{
        mode++;
    }
    
}

void EXT_Int1_isr(void)
{
    INTCONbits.INT0IF=0;
     if (seconds!=0 && minutes!=0 &&hours!=0){
                      PORTCbits.RC5 = 0; 
                   }
//    if(HS==5)
//    {
//        HS=0.0;
//    }
//    else
//    {
//        HS=HS+0.5;
//    }
}




void __interrupt(high_priority)highIsr(void)
//void interrupt high_priority highIsr(void)
{
    if(INTCONbits.TMR0IF) {
        
         if (initial == 1 || (initial == 0 && clkMode == 0 && flageBuzz==0 ) ){
             clockNormalMode();
//             if (seconds!=0 && minutes!=0 &&hours!=0){
//                INTCONbits.TMR0IF = 0;
     
           
             
//             }
          
        
             
             reloadTimer0();
         }
//             
        else INTCONbits.TMR0IF = 0; // Ignore Timer0 In Setup Mode
}
    else if(INTCON3bits.INT1IF)EXT_Int0_isr();
    else if (INTCONbits.INT0IF)EXT_Int1_isr();
    
    
   
}
void delay_ms(unsigned int n)
{
    int i;
    for (i=0; i < n; i++){
         __delaywdt_ms(1) ; 
    }
}
void clockNormalMode(void) {
    INTCONbits.TMR0IF = 0;
              if(temp <= sp- HS) {
            PORTCbits.RC5 = 1;
        } 
        else if(temp >= (sp + HS)) {
            PORTCbits.RC5 = 0;
        }
    if(--seconds <= 0) { 
        seconds=59;
        if(--minutes == -1) {
            minutes = 59;
            if (--hours == -1){ hours = 0;
            if (seconds==59 && minutes==59 &&hours==0){
            hours=0;
            seconds=0; 
            minutes=0; 
            
            }}
        }
                   if (seconds!=0 && minutes!=0 &&hours!=0){
                       
                   }
                   

        
    }
    reloadTimer0();
}
void incrementClock(void) {  
    
    seconds+=1; 
    if (seconds>=59){seconds=seconds-59; 
    minutes+=1;
    if (minutes>=59){minutes=minutes-60; 
    hours+=1;
    if (hours>=9){hours=0; 

            
    }
    
    }}}

void decrementClock(void) {
   
     if(--seconds <= 0) { 
        seconds=59;
        if(--minutes == -1) {
            minutes = 59;
            if (--hours == -1) hours = 0;
        }
    }
}
void increment10SClock(void ){
    seconds+=10; 
    if (seconds>=59){seconds=seconds-60; 
    minutes+=1;
    if (minutes>=59){minutes=minutes-60; 
    hours+=1;
    if (hours>=9){hours=0; 

            
    }
    


    }}}
void decrement10SClock(void){
    seconds -=10 ;
    if (seconds<=0){seconds =seconds+60 ;
    minutes -=1;
   if (minutes<=0){minutes =60+minutes ;
   hours-=1;
      if (hours<=0){hours =0 ;}

   
   

    
    
    }
    

    }}

void incrementMinClock(void){
    minutes+=1; 
    if (minutes>=59){
        minutes=0;
        hours+=1; 
        if (hours>9){
            seconds=0;
 
            minutes=0; 
            
            hours =0 ;
            
        }
        }
    }

void decrementMinClock(void){
     minutes -=1 ;
    if (minutes<=0){minutes =minutes+60 ;
   
     hours -=1 ;
      if (hours<=0){hours =0 ;

   
   }
    

    
    
    }
    

    }
void increment10MinClock(void){
minutes+=10; 
    if (minutes>=59){
        minutes=minutes-60;
        hours+=1; 
        if (hours>9){
            seconds=0;
 
            minutes=0; 
            
            hours =0 ;
            
        }
        }

}
void decrement10MinClock(void){
  minutes -=10 ;
    if (minutes<0){minutes =minutes+60 ;hours -=1 ;
    
      
      if (hours <= 0){  hours =0 ;
        

   
   }
    

    
    
    }
    

}
void incrementHourClock(void){
    hours+=1;
    if (hours>9){
        hours=0 ;
    }
             
}
void decrementHourClock(void){
    hours-=1;
    if (hours<=0){ hours=0 ;
       
    }
             
}

void setupPorts(void)
{
    ADCON0 =1;
    ADCON1 = 0x0C; //3analog input
    TRISB = 0xFF; // all pushbuttons are inputs
    TRISC = 0x80; // RX input , others output
    PORTC =0;
    TRISA = 0xFF; // All inputs
    TRISD = 0x00; // All outputs
    TRISE= 0x00;  // All outputs
    
    TRISCbits.RC0 = 1; // Timer1 Clock
//    PORTCbits.RC2 = 0; // Turn Off Cooler
    PORTCbits.RC5 = 0; // Turn Off Heater
    
   
       
    
    RCONbits.IPEN = 0;        // Disable Interrupt priority , All are high
    INTCONbits.TMR0IE = 1;    // Enable Timer0 Interrupt
    T0CONbits.TMR0ON  = 1;    // Start timer 0
    INTCONbits.INT0IE   = 1;  // Enable external interrupt 1
    
    reloadTimer0();          // TR1Value = 3036, 1 second
    
}
void display(void){
char LCD[64];
    char LCDP[64];
    char Buffer1[32]; 
    char Buffer2[32]; 
    char Buffer[32]; 
    
    unsigned char  C, H;
    if(mode==4){
     H='B';
        
    }    
    
    else if(PORTCbits.RC5 == 1){   
//        H = 'ON';
        
        sprintf(Buffer1, " HT:%s", "ON   ");
        
        
    }
    else{
//        H = 'OFF';
                sprintf(Buffer1, " HT:%s", "OFF   ");

        
    }
    
    if(PORTCbits.RC5 == 1){
//        C = 'ON';
          Buffer2;
          sprintf(Buffer2, " CK:%s", "ON   ");

    }
    else{
          sprintf(Buffer2, " CK:%s", "OFF  ");
    }
    
    
    switch(mode){
        case 0:
            sprintf(LCDP, "  SEC           ");
            break;
        
        case 1:
            sprintf(LCDP, "  10 SEC          ");
            break;
        case 2:
            sprintf(LCDP, "  MIN          ");
            break;
        case 3:
            sprintf(LCDP, "  10 MIN      ");
            break;
        case 4:
            sprintf(LCDP, "  HOUR     ");
            break;
//        case 5:
//            sprintf(LCDP, "Auto          ");
//            break;
    }
    
    lcd_gotoxy(1, 1);
    sprintf(Buffer, "Time : %02d:%02d:%02d", hours, minutes, seconds);
    lcd_puts(Buffer);
    
//    lcd_gotoxy(10, 1);
//    sprintf(LCD, "H C");
//    lcd_puts(LCD);
     lcd_gotoxy(1, 2);
    sprintf(LCD, "TMP:%3.1fC   ", temp);
    lcd_puts(LCD);
    
    lcd_gotoxy(1, 3);
    sprintf(LCD, "SP :%3.1fC",sp);
    lcd_puts(LCD);
    
    lcd_gotoxy(10, 2);
    lcd_puts(Buffer1);
    
    lcd_gotoxy(10, 3);
    lcd_puts(Buffer2);
    
//    lcd_gotoxy(1, 3);
//    sprintf(LCD, "HS =%3.1fC", HS);
//    lcd_puts(LCD);
    
    
    lcd_gotoxy(1, 4);
    sprintf(LCD, "MD :     ");
    lcd_puts(LCD);
    
    lcd_gotoxy(3, 4);
    lcd_puts(LCDP);
    



    }
int count=4;
// int flageBuzz =0 ;
void main(void) {
     seconds = 1;
    minutes = 0;
    hours   = 0;
    setupPorts();
    int count=0; 
    flageBuzz =0 ;
    init_adc_no_lib();
     initial == 1 ; clkMode == 1;
    INTCON = 0; // disable interrupts first, then enable the ones u want
    
    INTCON = 0;
    RCONbits.IPEN =0;
    
    INTCONbits.INT0IE = 1;
    INTCONbits.TMR0IE=1;
    
    
    INTCON2 = 0;
    
    INTCON3 = 0;
    INTCON3bits.INT1IE = 1;
    
    INTCON2bits.INTEDG1 = 1;
    INTCON2bits.INTEDG0= 1;
    T0CON=0X80;
    
    PIE1 = 0;
    PIR1 = 0;
    IPR1 = 0;
    PIE2 = 0;
    PIE2 = 0;
    PIR2 = 0;
    IPR2 = 0;
    
    INTCONbits.GIEH = 1;  // enable global interrupt bits
    INTCONbits.GIEL = 1;
   PORTCbits.RC1=0;
   lcd_init();
   lcd_send_byte(0, 1);
 
        
        
        
   
    while(1)
    {
         
        CLRWDT();  // no need for this inside the delay below
        //PORTDbits.RD5 = !PORTDbits.RD5; 
      //  delay_ms(5000);//read ADC AN0,AN1, AN2 every 5 seconds
//        if(PORTBbits.RB3 == 0) incrementClock();
//        else if(PORTBbits.RB4 == 0) decrementClock();
         
        if (PORTBbits.RB1 == 0){ initial = 0 ; clkMode = 0;  PORTCbits.RC5 = 1;
        if (flageBuzz==1)flageBuzz= 0;
                  
//        if(temp <= sp- HS) {
//            PORTCbits.RC5 = 1;
//        } 
//        else if(temp >= (sp + HS)) {
//            PORTCbits.RC5 = 0;
//        }
           
        }
        if (PORTBbits.RB0 == 0){ 
            mode+=1; 
            if (mode==5)mode=0;
           
        }
         if (PORTBbits.RB5 == 0){ seconds=0 ; minutes =0 ;hours =0;count=0 ;mode=0 ;PORTCbits.RC5 = 0;}
         if (PORTBbits.RB2 == 0){ PORTCbits.RC5 = 0; flageBuzz =1 ;}

        spf = read_adc_voltage(0);
        sp=spf*40.0;
        
//       if (seconds==0.0&& minutes==0.0&&hours==0.0 ){
        tempf =read_adc_voltage(2);
        temp=tempf*200;
//       }        
        
//        PORTCbits.RC2 = 0;
               
//        if (hours!=0 && minutes !=0 && seconds!=0 )      {  
//        if(temp <= sp- HS) {
//            PORTCbits.RC5 = 1;
//        } 
//        else if(temp >= (sp + HS)) {
//            PORTCbits.RC5 = 0;
//        }}
          if (mode ==0)
        {
//            PORTCbits.RC5=0;
//            PORTCbits.RC2=0;
            
//            incrementClock();

         if(PORTBbits.RB3 == 0) {incrementClock();count=4;}
        else if(PORTBbits.RB4 == 0) {decrementClock();count=4;}
         
        }   
                    
        else if(mode ==1)
        {
//            PORTCbits.RC5=1;
//            PORTCbits.RC2=0;
            if(PORTBbits.RB3 == 0) {increment10SClock();count=4;}
        else if(PORTBbits.RB4 == 0) {decrement10SClock();count=4;}
        }
        else if(mode ==2)
        {
//            PORTCbits.RC5=0;
//            PORTCbits.RC2=1;
            if(PORTBbits.RB3 == 0){ incrementMinClock();count=4;}
        else if(PORTBbits.RB4 == 0) {decrementMinClock();count=4;}
        }
        else if(mode ==3)
        { 
//           PORTCbits.RC2 = 0;
//               
//                
//                if(temp <= sp- HS) {
//                    PORTCbits.RC5 = 1;
//                } 
//                else if(temp >= (sp + HS)) {
//                    PORTCbits.RC5 = 0;
//            
//        
//            
//            }
            
            if(PORTBbits.RB3 == 0) {increment10MinClock();count=4;}
        else if(PORTBbits.RB4 == 0){ decrement10MinClock();count=4;}
 
        }
        
        else if(mode ==4)
        { 
//        if(ft==1){
//                PORTCbits.RC5 =!PORTCbits.RC5 ;
//                
//                ft=0;
//                
//                }
//                if(temp >= sp) {
//                    PORTCbits.RC2 = 1;
//                 
//                } 
//                else if(temp < (sp - HS)) {
//                    PORTCbits.RC2 = 0;
//                
//                }
// 
            if(PORTBbits.RB3 == 0){ incrementHourClock();count=4;}
            else if(PORTBbits.RB4 == 0){ decrementHourClock();count=4;}
       
        }
//             else if(mode ==5)
//        { 
//        
//                if(temp >= sp+HS) {
//                    PORTCbits.RC2 = 1;
//                 
//                    
//                    PORTCbits.RC5 = 0;
//                } 
//                else if(temp <= sp-HS) {
//                    PORTCbits.RC2 = 0;
//               
//                    
//                    PORTCbits.RC5 = 1;
//                }
//                
// 
//        }
        
        
        
      
        
        
        
        
//        display();
        if (seconds==0.0&& minutes==0.0&&hours==0.0 ){
//                INTCONbits.TMR0IE=0;
//            flageBuzz+=1; 
            PORTCbits.RC5=0 ;
            
            for (;count>0;count--){
             PORTCbits.RC1=1 ;
             delay_ms(20);
             PORTCbits.RC1=0;
             delay_ms(1000);
             
            }
            setupPorts();
      PORTCbits.RC5=0 ;

//            seconds =0; 
//            minutes=0;
//            hours=0 ;
            
            
            
//             PORTCbits.RC1=0 ;
//             break ;
            
//            count+=1;
//            delay_ms(2000);
//            if (PORTCbits.RC1==1){
//                
//            PORTCbits.RC1=0 ;
//            break; 
//            
//            
//            }
           
            
            
        }
        display(); 
//        if (count==4)break ;
    }
   
    return;
}