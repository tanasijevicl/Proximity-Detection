#include <msp430.h>
#include <sensor.h>
#include <stdint.h>
#include <extern_variables.h>

#define PS_ADDR (0x38)                          // sensor slave address
// sensor registers
#define SYSTEM_CONTROL_REG (0x40)               // system control register address
#define MODE_CONTROL_REG (0x41)                 // mode control register address
#define PS_DATA_REG (0x44)                      // proximity data register address (LSB)
// configuration data
#define MODE_CONTROL_DATA (0x43)                // PS enable, measurement time 100ms
#define SW_RESET (0x80)                         // start initial reset

#define PRINT_DATA (21)
/** macro to convert ASCII code to digit */
#define ASCII2DIGIT(x)      (x - '0')
/** macro to convert digit to ASCII code */
#define DIGIT2ASCII(x)      (x + '0')

bool transmit_done = false;
bool receive_done = false;

uint8_t proximity_data[2] = {0};
uint8_t digits[4] = {0};
uint8_t print_data[] = "Proximity data: 0000 ";


// Function for sensor reset
void psReset()
{
    transmit_done = false;              // setting initial values
    transmit_cnt = 0;
    transmit_buffer[0] = SYSTEM_CONTROL_REG;
    transmit_buffer[1] = SW_RESET;
    stop = 2;

    UCB1I2CSA = PS_ADDR;                // writing the desired slave address
    UCB1CTL1 |= UCTR;                   // setting UCTR for tansmitter mode
    UCB1CTL1 |= UCTXSTT;                // generating start condition
    UCB1IE |= UCTXIE;                   // enable TX interrupt

    while(!transmit_done);              // wait until data is transmitted

    UCB1CTL1 |= UCTXSTP;                // stopping comunication
}

// Function for proximity sensor initialization
void psInit()
{
    transmit_done = false;                          // setting initial values
    transmit_cnt = 0;
    transmit_buffer[0] = MODE_CONTROL_REG;
    transmit_buffer[1] = MODE_CONTROL_DATA;
    stop = 2;

    UCB1I2CSA = PS_ADDR;                            // writing the desired slave address
    UCB1CTL1 |= UCTR;                               // setting UCTR for tansmitter mode
    UCB1CTL1 |= UCTXSTT;                            // generating start condition
    UCB1IE |= UCTXIE;                               // enable TX interrupt

    while(!transmit_done);                          // wait until data is transmitted

    UCB1CTL1 |= UCTXSTP;                            // stopping comunication
}

// Function for reading proximity sensor data
void psReadData()
{
    transmit_done = false;                          // setting initial values
    transmit_cnt = 0;
    transmit_buffer[0] = PS_DATA_REG;
    stop = 1;

    UCB1I2CSA = PS_ADDR;                            // writing the desired slave address
    UCB1CTL1 |= UCTR;                               // setting UCTR for tansmitter mode
    UCB1CTL1 |= UCTXSTT;                            // generating start condition
    UCB1IE |= UCTXIE;                               // enable TX interrupt

    while(!transmit_done);                          // wait until data is transmitted

    receive_done = false;                           // setting initial values
    receive_cnt = 0;
    stop = 2;
    UCB1CTL1 &= ~UCTR;                              // clearing UCTR for reciever mode
    UCB1CTL1 |= UCTXSTT;                            // generating repeated start condition
    UCB1IE |= UCRXIE;                               // enable RX interrupt

    while(!receive_done);                           // wait until data is received

    proximity_data[0] = receive_buffer[0];          // save received data
    proximity_data[1] = receive_buffer[1];
}

// Function for convertig 16-bit number into bcd digits
void bin2bcd(const uint16_t number)
{
    uint16_t nr = number;
    uint16_t converted = 0;
    uint8_t tmp;
    for (tmp = 16; tmp > 0; tmp--) {
        converted = __bcd_add_short(converted, converted);
        if ((nr & 0x8000) != 0) {
            converted = __bcd_add_short(converted, 1);
        }
        nr <<= 1;
    }
    for (tmp = 0; tmp < 4; tmp++)
    {
        digits[tmp] = (converted & (0xf << tmp*4)) >> tmp*4;
    }
}

// Function for sendig data via UART
void psSendData()
{
    uint16_t proximity = 0;
    uint8_t i = 0;

    psReadData();                                                               // read ps data
    proximity = (proximity_data[1] << 8) | (0xff & proximity_data[0]);          // accommodate the data (MSB & LSB)


    bin2bcd(proximity);                                     // get bcd digits
    print_data[16] = DIGIT2ASCII(digits[3]);                // prepare data for sending
    print_data[17] = DIGIT2ASCII(digits[2]);
    print_data[18] = DIGIT2ASCII(digits[1]);
    print_data[19] = DIGIT2ASCII(digits[0]);
    print_data[20] = 0x0A;

    for (i = 0; i < PRINT_DATA; i++){                       // write data in (local) transmit buffer
        transmit_buffer[i] = print_data[i];
    }

    transmit_done = false;                                  // setting initial values
    transmit_cnt = 0;
    stop = PRINT_DATA;
    UCA1IE |= UCTXIE;

    while(!transmit_done);                                  // wait until data is transmitted

}



