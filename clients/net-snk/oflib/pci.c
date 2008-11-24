/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * All rights reserved.
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#include <rtas.h>
#include <of.h>
#include <pci.h>
#include <string.h>
#include <kernel.h>
#include <cpu.h>

int
pci_calc_bar_size(long long puid, int bus, int devfn, int bar)
{
	int size;
	int old;
	bar = bar * 4 + 0x10;
	old = rtas_pci_config_read(puid, 4, bus, devfn, bar);
	rtas_pci_config_write(puid, 4, bus, devfn, bar, 0xffffffff);
	size = (rtas_pci_config_read(puid, 4, bus, devfn, bar) & (-4)) * -1;
	rtas_pci_config_write(puid, 4, bus, devfn, bar, old);
	return size;
}

int
pci_get_bar_start(long long puid, int bus, int devfn, int bar)
{
	return rtas_pci_config_read(puid, 4, bus, devfn, bar * 4 + 0x10);
}

void
pci_set_bar_start(long long puid, int bus, int devfn, int bar, int value)
{
	rtas_pci_config_write(puid, 4, bus, devfn, bar * 4 + 0x10, value);
}

unsigned int
read_io(void *addr, size_t sz)
{
	unsigned int ret;

	switch (sz) {
	case 1:
		set_ci();
		ret = (unsigned int) *((unsigned char *) addr);
		clr_ci();
		break;
	case 2:
		set_ci();
		ret = (unsigned int) *((unsigned short *) addr);
		clr_ci();
		break;
	case 4:
		set_ci();
		ret = *((unsigned int *) addr);
		clr_ci();
		break;
	default:
		ret = 0;
	}

	return ret;
}

int
write_io(void *addr, unsigned int value, size_t sz)
{
	switch (sz) {
	case 1:
		set_ci();
		*((unsigned char *) addr) = (unsigned char) value;
		clr_ci();
		break;
	case 2:
		set_ci();
		*((unsigned short *) addr) = (unsigned short) value;
		clr_ci();
		break;
	case 4:
		set_ci();
		*((unsigned int *) addr) = value;
		clr_ci();
		break;
	default:
		return -1;
	}

	return 0;
}