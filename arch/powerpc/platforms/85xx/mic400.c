/*
 * MIC-400 Board Setup
 *
 * Copyright 2013 OMICRON electronics GmbH.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>

#include <asm/time.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <mm/mmu_decl.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <asm/mpic.h>
#include <linux/fsl/guts.h>
#include <asm/io.h>

#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>

#include "mpc85xx.h"

void __init mic400_pic_init(void)
{
	struct mpic *mpic = mpic_alloc(NULL, 0, MPIC_BIG_ENDIAN |
				       MPIC_SINGLE_DEST_CPU,
				       0, 256, " OpenPIC  ");

	BUG_ON(mpic == NULL);

	mpic_init(mpic);
}


/*
 * Setup the architecture
 */
static void __init mic400_setup_arch(void)
{
	if (ppc_md.progress)
		ppc_md.progress("mic400_setup_arch()", 0);

	fsl_pci_assign_primary();

	printk(KERN_INFO "MIC-400 board from OMICRON electronics GmbH.\n");
}

/* Returns a pointer to the GUTS */
static struct ccsr_guts __iomem *get_p1010_guts(void)
{
	struct device_node *guts_np = NULL;
	struct ccsr_guts __iomem *guts;

	/* Map the global utilities registers. */
	guts_np = of_find_compatible_node(NULL, NULL, "fsl,p1010-guts");
	if (!guts_np) {
		pr_err("mic400: missing global utilties device node\n");
		return NULL;
	}

	guts = of_iomap(guts_np, 0);
	of_node_put(guts_np);
	if (!guts) {
		pr_err("mic400: could not map global utilties device\n");
		return NULL;
	}

	return guts;
}

/* Bits from guts->powmgtcsr */
#define PMCSR_MSK       (0xf << 28)
#define PMCSR_SLP	(1 << 17)
#define PMCSR_SLPING	(1 << 1)

/* Switch off all peripherals and put the CPU's into sleep mode */
static __attribute__((noreturn)) void do_halt(void)
{
	struct ccsr_guts __iomem *guts = get_p1010_guts();

	if (guts) {
		/* Switch off everything (not the E500 Core or the Time Base) */
		out_be32(&guts->devdisr, 0xffff3fff);
		out_be32(&guts->devdisr2, 0xffffffff);

		/* Disable clocks in sleep mode */
		out_be32(&guts->pmcdr, 0xffffffff);
	}

	local_irq_disable();
	while (1) {
		if (guts)
			/* Enable sleep mode, don't allow interrupts
			 * or Critical interrupts to wake either
			 * core. */
			out_be32(&guts->powmgtcsr, PMCSR_MSK | PMCSR_SLP);
	}
}

/* Called from machine_power_off in arch/powerpc/kernel/setup-common.c */
/* Called from machine_halt in arch/powerpc/kernel/setup-common.c */
static __attribute__((noreturn)) void mic400_halt(void)
{
#ifdef CONFIG_SMP
	smp_send_stop();
#endif
	printk(KERN_EMERG "%s: System Halted, OK to turn off power\n",
			__func__);
	do_halt();
}

machine_arch_initcall(mic400, mpc85xx_common_publish_devices);

/*
 * Called very early, device-tree isn't unflattened
 */
static int __init mic400_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	if (of_flat_dt_is_compatible(root, "omicron,mic400"))
		return 1;
	return 0;
}

define_machine(mic400) {
	.name			= "MIC-400",
	.probe			= mic400_probe,
	.setup_arch		= mic400_setup_arch,
	.init_IRQ		= mic400_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
	.pcibios_fixup_phb      = fsl_pcibios_fixup_phb,
#endif
	.get_irq		= mpic_get_irq,
    //	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,

    //	.power_off              = mic400_halt,
	.halt                   = mic400_halt
};
