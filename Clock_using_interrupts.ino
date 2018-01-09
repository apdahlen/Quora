/*
 *
 * Copyright 2014 Aaron P. Dahlen 
 *
 * This program is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <Licenses - GNU Project - Free Software Foundation (http://www.gnu.org/licenses/)>.
 */

/* @Warning - this code breaks if you use any timer-2 functions such as tone.
 *
 * @Warning - there is a bug between the timer ISR and the Arduino analogWrite function.  If you 
 * need to use the PWM from within the ISR then use the "fast PWM"
 * 
 *
 * Arduino - SecretsOfArduinoPWM (http://arduino.cc/en/Tutorial/SecretsOfArduinoPWM)
 * 
 *    void setup(){
 *        .
 *        .
 *        .
 *        pinMode(3, OUTPUT);
 *        pinMode(11, OUTPUT);
 *        TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
 *        TCCR2B = _BV(CS22);                                               // This register controls PWM frequency

 *        OCR2A = 0;                                                        // Arduino I/O pin # 11 duty cycle
 *        OCR2B = 0;                                                        // Arduino I/O pin # 3 duty cycle
 *        .
 *        .
 *        .
 *    }
 *
 *    void loop(){
 *        .
 *        .
 *        .
 *        OCR2A = pin_11_PWM_DC;
 *        OCR2B = pin_3_PWM_DC;
 *        .
 *        .
 *        .
 *    }
 */



// AVR GCC libraries for more information see:
//     avr-libc: Modules (http://www.nongnu.org/avr-libc/user-manual/modules.html)
//     The GNU C Library (https://www.gnu.org/software/libc/manual/)

    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <stdint.h>
    #include <string.h>
    #include <ctype.h>


// Project specific includes


// Global variables




void setup(){

    digitalWrite(K1_PIN, LOW);

    USART_set_terminator(LINE_TERMINATOR);

    init_timer_1_CTC(100);                          // Enable the timer ISR


}



/*********************************************************************************
 *  ______  ____   _____   ______  _____  _____    ____   _    _  _   _  _____
 * |  ____|/ __ \ |  __ \ |  ____|/ ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |__  | |  | || |__) || |__  | |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  __| | |  | ||  _  / |  __| | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |    | |__| || | \ \ | |____| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |_|     \____/ |_|  \_\|______|\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 ********************************************************************************/



ISR(TIMER1_COMPA_vect){

/** @brief This Interrupt Service Routine (ISR) serves as the 100 Hz heartbeat 
 * for the Arduino. See the companion function init_timer_1_CTC for additional 
 * information.
 *
 * @ Note:
 *    1) Compiler generated code pushes status register and any used registers to stack.
 *    2) Calling a subroutine from the ISR causes compiler to save all 32 registers; a 
 *       slow operation (fact check). 
 *    3) Status and used registers are popped by compiler generated code.
 */

    static uint8_t LED_flag;
    static uint16_t time_in_state = 0;








    //  INSERT one second LED blink code here...






}


/*********************************************************************************
 *  ____            _____  _  __ _____  _____    ____   _    _  _   _  _____
 * |  _ \    /\    / ____|| |/ // ____||  __ \  / __ \ | |  | || \ | ||  __ \
 * | |_) |  /  \  | |     | ' /| |  __ | |__) || |  | || |  | ||  \| || |  | |
 * |  _ <  / /\ \ | |     |  < | | |_ ||  _  / | |  | || |  | || . ` || |  | |
 * | |_) |/ ____ \| |____ | . \| |__| || | \ \ | |__| || |__| || |\  || |__| |
 * |____//_/    \_\\_____||_|\_\\_____||_|  \_\ \____/  \____/ |_| \_||_____/
 *
 ********************************************************************************/

void loop(){

}



void init_timer_1_CTC(long desired_ISR_freq){
/**
 * @brief Configure timer #1 to operate in Clear Timer on Capture Match (CTC Mode)
 *
 *      desired_ISR_freq = (F_CLK / prescale value) /  Output Compare Registers
 *
 *   For example:
 *        Given an Arduino Uno: F_clk = 16 MHz
 *        let prescale                = 64
 *        let desired ISR heartbeat   = 100 Hz
 *
 *        if follows that OCR1A = 2500
 *
 * @param desired_ISR_freq is the desired operating frequency of the ISR
 * @param F_CLK must be defined globally e.g., #define F_CLK 16000000L
 *
 * @return void
 *
 * @note The prescale value is set manually in this function.  Refer to ATMEL ATmega48A/PA/88A/PA/168A/PA/328/P datasheet for specific settings.
 *
 * @warning There are no checks on the desired_ISR_freq parameter.  Use this function with caution.
 *
 * @warning Use of this code will break the Arduino Servo() library.
 */
    cli();                                          // Disable global
    TCCR1A = 0;                                     // Clear timer counter control registers.  The initial value is zero but it appears Arduino code modifies them on startup...
    TCCR1B = 0;
    TCCR1B |= (1 << WGM12);                         // Timer #1 using CTC mode
    TIMSK1 |= (1 << OCIE1A);                        // Enable CTC interrupt
    TCCR1B |= (1 << CS10)|(1 << CS11);              // Prescale: divide by F_CLK by 64.  Note SC12 already cleared
    OCR1A = (F_CLK / 64L) / desired_ISR_freq;       // Interrupt when TCNT1 equals the top value of the counter specified by OCR
    sei();                                          // Enable global
}
