#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

// DAC I2C Address
#define DAC_ADDRESS 0x60  // MCP4725 address

void I2C0_Init(void) {
    SYSCTL_RCGCI2C_R |= (1U << 0); // Enable I2C0
    SYSCTL_RCGCGPIO_R |= (1U << 1); // Enable Port B
    GPIO_PORTB_AFSEL_R |= (1U << 2) | (1U << 3); // PB2 and PB3 as I2C
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFFFF00) | 0x00000033; // Set PB2 and PB3 to I2C
    GPIO_PORTB_DEN_R |= (1U << 2) | (1U << 3); // Enable digital function
    I2C0_MCR_R = I2C_MCR_MFE; // Enable I2C0 master
}

void I2C0_Write(uint16_t value) {
    uint8_t data[2];
    data[0] = (value >> 4) & 0x0F; // Upper 4 bits and command bits
    data[0] |= 0x40; // Set command bits for the MCP4725 (write to DAC)
    data[1] = (value & 0x0F) << 4;  // Lower 8 bits

    // Send data to DAC
    I2C0_MSA_R = DAC_ADDRESS; // Set slave address
    I2C0_MDR_R = data[0];     // Send upper byte
    I2C0_MCS_R = I2C_MCS_START | I2C_MCS_RUN; // Start condition
    while (I2C0_MCS_R & I2C_MCS_BUSY); // Wait for completion

    I2C0_MDR_R = data[1];     // Send lower byte
    I2C0_MCS_R = I2C_MCS_RUN | I2C_MCS_STOP; // Run and stop condition
    while (I2C0_MCS_R & I2C_MCS_BUSY); // Wait for completion
}

int main(void) {
    I2C0_Init(); // Initialize I2C0
    I2C0_Write(2048); // Set the DAC to mid-scale (2048)
    while(1); // Loop forever
}