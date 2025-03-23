#include <stdio.h>
#include "rp2x_gpio_irq.h"

struct _gpio_callback_info {
	rp2x_gpio_callback_t callback;
	void *data;
};

struct _gpio_callback_info _gpio_callback_table[NUM_BANK0_GPIOS];

// General dispatch table function for all GPIO interrupts
void _gpio_irq_callback_dispatch(uint gpio, uint32_t event_mask) {
	struct _gpio_callback_info *info = &_gpio_callback_table[gpio];

	info->callback(gpio, event_mask, info->data);
}


void rp2x_gpio_irq_init(void) {
	static bool initialized = false;

	if (initialized == false) {
		for (int i = 0; i < NUM_BANK0_GPIOS; i++) {
			_gpio_callback_table[i].callback = NULL;
			_gpio_callback_table[i].data = NULL;
		}
	}

	gpio_set_irq_callback(_gpio_irq_callback_dispatch);

	initialized = true;
}

void rp2x_gpio_irq_enable(
		uint gpio, 
		uint32_t event_mask,
		rp2x_gpio_callback_t callback, 
		void *data
) 
{
	struct _gpio_callback_info *info = &_gpio_callback_table[gpio];

	// If this is already enabled we should clear old mask first
	if (info->callback != NULL) {
		// All interrupt events
		uint32_t clear_mask = 
				GPIO_IRQ_LEVEL_LOW | GPIO_IRQ_LEVEL_HIGH |
				GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE;

		gpio_set_irq_enabled(gpio, clear_mask, false);
	}

	// Set callback and data before enabling IRQ
	info->callback = callback;
	info->data = data;

	gpio_set_irq_enabled(gpio, event_mask, true);
	irq_set_enabled(IO_IRQ_BANK0, true);
}

void rp2x_gpio_irq_disable(uint gpio) {
	// All interrupt events
	uint32_t event_mask = 
			GPIO_IRQ_LEVEL_LOW | GPIO_IRQ_LEVEL_HIGH |
			GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE;

	gpio_set_irq_enabled(gpio, event_mask, false);

	struct _gpio_callback_info *info = &_gpio_callback_table[gpio];

	info->callback = NULL;
	info->data = NULL;
}

void rp2x_irq_disable_all(irq_state_t states) {
	// Clear states
	states[0] = 0;
	if (NUM_IRQS > 32)
		states[1] = 0;

	// Iterate through all IRQs
	for (uint32_t irq_num = 0; irq_num < NUM_IRQS; irq_num++) {
		// If enabled
		if(irq_is_enabled(irq_num)) {
			// Cache state
			states[irq_num/32] |= (1 << (irq_num % 32));	

			// disable
			irq_set_enabled(irq_num, false);
		}
	}
}

void rp2x_irq_restore(irq_state_t states) {
	// Enable any previously enabled IRQ
	for (uint32_t irq_num = 0; irq_num < NUM_IRQS; irq_num++) {
		if (states[irq_num/32] & (1 << (irq_num % 32)))
			irq_set_enabled(irq_num, true);
	}
}
