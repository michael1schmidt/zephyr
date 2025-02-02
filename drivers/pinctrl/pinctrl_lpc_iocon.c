/*
 * Copyright 2022, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/pinctrl.h>
#include <fsl_clock.h>

#define OFFSET(mux) (((mux) & 0xFFF00000) >> 20)
#define TYPE(mux) (((mux) & 0xC0000) >> 18)

#define IOCON_TYPE_D 0x0
#define IOCON_TYPE_I 0x1
#define IOCON_TYPE_A 0x2

static volatile uint32_t *iocon =
	(volatile uint32_t *)DT_REG_ADDR(DT_NODELABEL(iocon));

int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins, uint8_t pin_cnt,
			   uintptr_t reg)
{
	for (uint8_t i = 0; i < pin_cnt; i++) {
		uint32_t pin_mux = pins[i];
		uint32_t offset = OFFSET(pin_mux);

		/* Check if this is an analog or i2c type pin */
		switch (TYPE(pin_mux)) {
		case IOCON_TYPE_D:
			pin_mux &= Z_PINCTRL_IOCON_D_PIN_MASK;
			break;
		case IOCON_TYPE_I:
			pin_mux &= Z_PINCTRL_IOCON_I_PIN_MASK;
			break;
		case IOCON_TYPE_A:
			pin_mux &= Z_PINCTRL_IOCON_A_PIN_MASK;
			break;
		default:
			/* Should not occur */
			assert(TYPE(pin_mux <= IOCON_TYPE_A));
		}
		/* Set pinmux */
		*(iocon + offset) = pin_mux;
	}
	return 0;
}

#ifdef CONFIG_SOC_FAMILY_LPC
/* LPC family needs iocon clock to be enabled */

static int pinctrl_clock_init(const struct device *dev)
{
	ARG_UNUSED(dev);
	/* Enable IOCon clock */
	CLOCK_EnableClock(kCLOCK_Iocon);
	return 0;
}

SYS_INIT(pinctrl_clock_init, PRE_KERNEL_1, 0);

#endif /* CONFIG_SOC_FAMILY_LPC */
