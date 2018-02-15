/*
 * Copyright 2017 NXP
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * \file nxp_sja1105p_switchdev.h
 *
 * \author Marco Hartmann
 *
 * \date 2017-08-02
 *
 */

#ifndef _NXP_SJA1105P_H
#define _NXP_SJA1105P_H

#include <linux/spi/spi.h>

#include "sja1105p_cfg_file.h"
#include "sja1105p_init.h"

int nxp_swdev_init(struct sja1105p_context_data **ctx_nodes);
void nxp_swdev_exit(void);


#endif /* _NXP_SJA1105P_H */
