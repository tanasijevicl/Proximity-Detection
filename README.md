# Proximity Detection

## Overview

This project involves reading data from a Proximity 11 Click sensor (for proximity detection) and transmitting the data to a computer via UART. The project is implemented on the MSP430F5529 development board. The main application is written in C, while some interrupt routines are written in assembly language. The development was done using Code Composer Studio 12.4.0.

## Features

- **Sensor Reading**: Interfaces with the Proximity 11 Click sensor to detect proximity and read sensor data.
- **UART Communication**: Transmits the read sensor data to a computer via UART for further processing.
- **Mixed Language Implementation**: The main program is implemented in C, and some interrupt routines are implemented in assembly language.

## Program Files

- **`main.c`**: Configuring the MSP430 microcontroller for handling UART, I2C communication, button debouncing, and managing sensor data transmission and reception through interrupts.
- **`sensor.c`** & **`sensor.h`**: Functions for resetting, initializing, reading data from a proximity sensor via I2C, converting the sensor's binary data to BCD, and transmitting the results over UART.
- **`extern_variables.h`**: Declaration of external variables used across multiple modules.
- **`p1isr.asm`**: The interrupt service routine for handling MSP430 Port 1 interrupts.

## Block diagrams

Refer to the **`block_diagrams.pdf`** for an easier understanding of the system. The file includes: 

1. **Hardware Block Diagram**

2. **Software Block Diagram**

## Documentation

Documentation includes:

1. Datasheets of MSP430F5529 and rpr-0521rs-e (Proximity 11 Click sensor)

2. Schematics of used hardware, Proximity 11 Click sensor and MSP430F5529 pinout

3. Erratasheet and User's Guide of MSP430F5529

## Setup

1. **Hardware Requirements**:
   - MSP430F5529 development board
   - Proximity 11 Click sensor
   - UART connection to a computer

2. **Software Requirements**:
   - Code Composer Studio 12.4.0

3. **Building the Project**:
   - Open Code Composer Studio.
   - Import the project into the workspace.
   - Build the project by selecting `Project` > `Build All`.

4. **Running the Program**:
   - Connect the MSP430F5529 to your computer using a UART interface.
   - Load the compiled program onto the development board.
   - Open a terminal on your computer to receive UART data.

## Usage

- Press the button to read proximity data from the sensor and transmit it via UART.
- The sensor data can be monitored using a terminal program connected to the UART interface.

## Notes

- Ensure that the Proximity 11 Click sensor is correctly connected to the MSP430F5529 board.
- Verify UART settings and connections before running the program.

## Dependencies

- **IDE**: Code Composer Studio 12.4.0 (Texas Instruments)
