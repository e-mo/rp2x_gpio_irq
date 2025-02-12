#include <stdio.h>
#include "rp2x_gpio_irq.h"

// There are 26 IRQ capable GPIO pins
// 0-22 and 26-28
// After some though I figured that it was better to just
// leave a few indexes unused then have to check for and do an
// index translation for pins 26-28 on every function and callback.
#define GPIO_IRQ_PIN_MAX (29)

struct _gpio_callback_info {
	rp2x_gpio_callback_t callback;
	void *data;
};

struct _gpio_callback_info _gpio_callback_table[GPIO_IRQ_PIN_MAX];

// General dispatch table function for all GPIO interrupts
void _gpio_irq_callback_dispatch(uint gpio, uint32_t event_mask) {
	struct _gpio_callback_info *info = &_gpio_callback_table[gpio];

	info->callback(gpio, event_mask, info->data);
}

void rp2x_gpio_irq_init(void) {
	static bool initialized = false;
	if (initialized) return;

	for (int i = 0; i < GPIO_IRQ_PIN_MAX; i++) {
		_gpio_callback_table[i].callback = NULL;
		_gpio_callback_table[i].data = NULL;
	}

	// Set generic callback to our dispatch function
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
