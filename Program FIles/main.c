#include <msp430.h>
#include <stdint.h>
#include <sensor.h>
#include <extern_variables.h>

#define DEBOUNCE_PERIOD (1048)      // ~32ms (31.25ms)

unsigned int transmit_cnt = 0;
unsigned int receive_cnt = 0;
unsigned int stop = 0;

volatile uint8_t transmit_buffer[TRANSMIT_BUFFER_SIZE] = {0};
volatile uint8_t receive_buffer[RECEIVE_BUFFER_SIZE] = {0};

typedef enum {IDLE, RESET, SEND} state;
state curr_state = RESET;

void configuration()
{
    // S3 button config
    P1REN |= BIT4;                      // enable pull up/down
    P1OUT |= BIT4;                      // set pull up
    P1DIR &= ~BIT4;                     // P1.4 as in
    P1IES |= BIT4;                      // interrupt on falling edge
    P1IFG &= ~BIT4;                     // clear flag
    P1IE  |= BIT4;                      // enable interrupt

    // Timer A0 config
    TA0CCR0 = DEBOUNCE_PERIOD;          // debounce period
    TA0CCTL0 |= CCIE;                   // enable CCR0 interrupt
    TA0CTL = TASSEL__SMCLK;             // SMCLK as a clk

    // I2C config
    P4SEL |= BIT1 | BIT2;               // P4.1 and P4.2 peripheral module function select (I2C)
    UCB1CTL1 |= UCSWRST;                // USCI_B1 in reset state
    UCB1CTL1 |= UCSSEL__SMCLK;          // SMCLK as clock source
    UCB1BRW = 0x0008;                   // bit clock prescaler
    UCB1CTL0 |= UCMODE_3 + UCMST;       // I2C mode and master mode
    UCB1CTL1 &= ~UCSWRST;               // USCI_B1 in operational state
    UCB1IE |= UCNACKIE;                 // enable NACK interrupt

    // UART config
    P4SEL |= BIT4 | BIT5;               // P4.4 and P4.5 peripheral module function select (UART)
    UCA1CTL1 |= UCSWRST;                // USCI_A1 in reset state
    UCA1CTL1 |= UCSSEL__SMCLK;          // SMCLK as clk source
    UCA1BR0 = 109;                      // BRCLK Freqency = 1.048.576Hz, Baud Rate = 9600
    UCA1BR1 = 0;
    UCA1MCTL |= UCBRS_2 | UCBRF_0;
    UCA1CTL1 &= ~UCSWRST;               // USCI_A1 in operational state

    __enable_interrupt();               // global interrupts enable
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer

    configuration();                    // initial configuration

    while(1){                           // main loop
        switch (curr_state){
        case IDLE:
            break;
        case RESET:
            psReset();
            psInit();
            curr_state = IDLE;
            break;
        case SEND:
            psSendData();
            curr_state = IDLE;
            break;
        }
    }
}


void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) CCR0ISRA0 (void)
{
    if ((P1IN & BIT4) == 0)                             // check if button is still pressed
    {
        curr_state = SEND;                              // send data (change state)
    }
    TA0CTL &= ~(MC0 | MC1);                             // stop timer
    TA0CTL |= TACLR;                                    // clear timer
    P1IFG &= ~BIT4;                                     // clear P1.4 flag
    P1IE |= BIT4;                                       // enable P1.4 interrupt
}


void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCIA1 (void)
{
    switch (UCA1IV)
    {
    case 0:
        break;
    case USCI_UCTXIFG:
        if (transmit_cnt >= (stop-1)){
            UCA1IE &= ~UCTXIE;                          // disable TX interrupt
            transmit_done = true;                       // transmit is done
        }
        UCA1TXBUF = transmit_buffer[transmit_cnt];      // write data into buffer
        transmit_cnt++;                                 // incerement counter
        break;
    default:
        break;
    }
}


void __attribute__ ((interrupt(USCI_B1_VECTOR))) USCIB1 (void)
{
    switch (UCB1IV)
    {
    case 0:
        break;
    case USCI_I2C_UCNACKIFG:                            // in case of not ack flag
        UCB1CTL1 |= UCTXSTP;                            // stop communication
        UCB1IE &= ~UCRXIE;                              // disable transmit interrupt
        curr_state = RESET;                             // reset sensor (change state)
        break;
    case USCI_I2C_UCRXIFG:
        if (receive_cnt == (stop - 2)){
            UCB1CTL1 |= UCTXSTP;                        // set stop transmit bit (slave will send one more data)
        }
        else if (receive_cnt >= (stop-1)){
            UCB1IE &= ~UCRXIE;                          // disable RX interrupt
            receive_done = true;                        // receive is done
        }
        receive_buffer[receive_cnt] = UCB1RXBUF;        // read data from buffer
        receive_cnt++;                                  // incerement counter
        break;
    case USCI_I2C_UCTXIFG:
        if (transmit_cnt >= (stop-1)){
            UCB1IE &= ~UCTXIE;                          // disable TX interrupt
            transmit_done = true;                       // transmit is done
        }
        UCB1TXBUF = transmit_buffer[transmit_cnt];      // write data into buffer
        transmit_cnt++;                                 // incerement counter
        break;
    default:
        break;
    }
}
