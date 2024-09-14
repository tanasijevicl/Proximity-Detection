#include <stdbool.h>

#ifndef EXTERN_VARIABLES_H_
#define EXTERN_VARIABLES_H_

#define TRANSMIT_BUFFER_SIZE (30)
#define RECEIVE_BUFFER_SIZE (30)

extern volatile uint8_t transmit_buffer[TRANSMIT_BUFFER_SIZE];
extern volatile uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];

extern unsigned int transmit_cnt;
extern unsigned int receive_cnt;
extern unsigned int stop;

extern bool transmit_done;
extern bool receive_done;

#endif /* EXTERN_VARIABLES_H_ */
