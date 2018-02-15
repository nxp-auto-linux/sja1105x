/*
* AVB switch driver module for SJA1105
* Copyright (C) 2017 NXP Semiconductors
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
/**
*
* \file sja1105p_debugfs.h
*
* \author Philippe guasch, Laurent Brando
*
* \date 2017-08-02
*
* \brief Functions definitions for SJA1105P debugfs
*
*****************************************************************************/
#ifndef _SJA1105P_DEBUGFS_H__
#define _SJA1105P_DEBUGFS_H__

#include "sja1105p_init.h"

void sja1105p_debugfs_init(struct sja1105p_context_data *ctx_data);
void sja1105p_debugfs_remove(struct sja1105p_context_data *ctx_data);

#endif /* _SJA1105P_DEBUGFS_H__ */
