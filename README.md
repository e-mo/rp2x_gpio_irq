# rp2x_gpio_irq

A three function helper library I wrote while building some interrupt heavy communication routines that required frequent  
reconfiguring of callbacks.  

These functions are used *instead* of the standard gpio_irq\* functions from the pico stdlib.  
Trying to use both together will likely lead to some confusing results.   
  
## Functions
```c
//| Callback function typedef.
typedef void(* rp2x_gpio_callback_t)(uint gpio, uint32_t event_mask, void *data);
```
  
```c
//| rp2x_gpio_irq_init
//| 
//| Initialize the default state of the callback dispatch table.
//| Must be called first. Repeated calls do nothing.
void rp2x_gpio_irq_init(void);
```
  
```c
//| rp2x_gpio_irq_enable
//|
//| Configure a GPIO pin for interrupt, set a callback function,
//| and provide (optional) pointer to be passed to callback function.
//| 
//| gpio: GPIO pin
//| event_mask: Mask for events you wish to trigger an interrupt.
//|             ex. mask = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE;
//| callback: Interrupt callback function.
//| data: Option void pointer to user data. Can be NULL.
void rp2x_gpio_irq_enable(
		uint gpio, 
		uint32_t event_mask,
		rp2x_gpio_callback_t callback, 
		void *data
); 
```

```c
//| rp2x_gpio_irq_disable
//|
//| Disable interrupt on GPIO pin.
//|
//| gpio: GPIO pin
void rp2x_gpio_irq_disable(uint gpio);
); 
```
  
## Usage

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "rp2x_gpio_irq.h"

#define IRQ_PIN (25)

void callback_example(uint gpio, uint32_t event_mask, void *data) {
	// Use event_mask to determine which type of event triggered
	// the interrupt.
	if (event_mask & GPIO_IRQ_EDGE_RISE)
		printf("Rising edge on GPIO %u\n", gpio);	
	else if (event_mask & GPIO_IRQ_EDGE_FALL)
		printf("Falling edge on GPIO %u\n", gpio);	
	
	// Will print "data: Test"
	printf("data: %s\n", (char *)data);
}

int main() {
	stdio_init_all();

	rp2x_gpio_irq_init();

	// Enable interrupt for GPIO 25 on both falling and rising edges.
	// Also pass the string "Test" to be used in the callback function. 
	uint32_t mask = GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
	rp2x_gpio_irq_enable(IRQ_PIN, mask, callback_example, "Test");

	// Sleep for 10 seconds waiting for interrupts.
	sleep_ms(10000); 

	// Disable interrupt
	rp2x_gpio_irq_disable(IRQ_PIN);
}
```
