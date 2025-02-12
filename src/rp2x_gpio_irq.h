#ifndef RP2X_GPIO_IRQ_H
#define RP2X_GPIO_IRQ_H
// A helper library for better handling of GPIO IRQ callbacks using the
// RP2X series of microcontrollers.
//
// Due to the design of the hardware, all GPIO interrupts call the same interrupt
// handler which then pass on event/pin information to the one user-defined
// GPIO interrupt callback.
//
// Even using raw interrupt handlers (available through API) doesn't really avoid
// having to look up the IO_BANK0 values and determine which GPIO fired the IRQ.
// This makes setting up seperate callback functions for different GPIO pins a
// bit tedious and requires the user to write a large dispatch function if they
// are using a large number of GPIO pins as interrupts, or if the pins they are
// using as interrupts change dynamically.
//
// I use the handler to create a framework that abstracts this tedium away
// as well as make user defined data available to callback functions.
#include <stdint.h>

#include "hardware/gpio.h"

typedef unsigned uint;
typedef void(* rp2x_gpio_callback_t)(uint gpio, uint32_t event_mask, void *data);

// Initializes the default state of the callback dispatch table
// and sets the generic irq callback to the proper function.
// The library will not function if you do not call this first. 
void rp2x_gpio_irq_init(void);

// Enables IRQ on a GPIO pin, sets the event mask, callback function, and
// (optional) user data to be passed into callback function.
// Can be called multiple times to change mask, callback, or data
// without having to disable first
void rp2x_gpio_irq_enable(
		uint gpio, 
		uint32_t event_mask,
		rp2x_gpio_callback_t callback, 
		void *data
); 

// Completely disables irq events on pin
void rp2x_gpio_irq_disable(uint gpio);
	
#endif // RP2X_GPIO_IRQ_H
