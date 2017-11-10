"""
The MIT License (MIT)
Copyright (c) 2017 NXP

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

"""
#   from UM10851 (SJA1105EL)
#   Name                                    ID  Mandatory
#   -------------------------------------------------------------------------------
#   L2 Address Lookup table                 05h no
#   L2 Policing table                       06h yes, at least one entry
#   VLAN Lookup table                       07h yes, at least the default untagging VLAN
#   L2 Forwarding table                     08h yes
#   MAC Configuration table                 09h yes
#   L2 Lookup Parameters table              0Dh no
#   L2 Forwarding Parameters table          0Eh yes
#   AVB Parameters table                    10h no
#   General Parameters table                11h yes
#   Retagging table                         12h no
#   xMII Mode Parameters table              4Eh yes

############################################################################
# SJA1105 minimalistic example configuration
#
# This is a baseline SJA1105 example 
#
############################################################################

import os

from ethsw.configuration import *
from ethsw.tables_sja1105 import *

NO_CBS_BLOCKS = 16
NO_ETH_PORTS = 5
NO_PRIORITIES = 8
SJA1105_DEVICEID = 0x9f00030e

c = Configuration(deviceid = SJA1105_DEVICEID)

#############################################################################
# General Parameters
#############################################################################

general_parameters_table = make_table_by_layout(general_parameters_table_layout, layoutid_map)
c.append(general_parameters_table)
general_parameters_table.append({
    "MIRR_PTACU"             : 1,                   # Dynamic change of Mirror Port is enabled
    "SWITCHID"               : 3,
    "HOSTPRIO"               : 0,
    "MAC_FLTRES[0]"          : 0x000000000000,
    "MAC_FLTRES[1]"          : 0x000000000000,
    "MAC_FLT[0]"             : 0xFFFFFFFFFFFF,
    "MAC_FLT[1]"             : 0xFFFFFFFFFFFF,
    "INCL_SRCPT[0]"          : 0,
    "INCL_SRCPT[1]"          : 0,
    "SEND_META[0]"           : 0,
    "SEND_META[1]"           : 0,
    "CASC_PORT"              : 6,
    "MIRR_PORT"              : 4,                   # Port 4 is defined as the mirror port
    "HOST_PORT"              : 6,
    "TPID"                   : 0x8100,
    "IGNORE2STF"             : 0,
})

#############################################################################
# MAC Configuration Table
#############################################################################

SPEED_HOST      = 0  # speed set by host during run-time
SPEED_1GBPS     = 1
SPEED_100MBPS   = 2
SPEED_10MBPS    = 3

speed = [SPEED_1GBPS, SPEED_1GBPS, SPEED_1GBPS, SPEED_1GBPS, SPEED_1GBPS]
speed_Mbps = [10 ** (4 - x) for x in speed]

default_vlan = 0  #Default VLAN ID on all ports for untagged frames is 0

queue_enable = [1,0,0,0,0,0,0,0]
prio_queue0 = [0,511]
prio_queue1 = [0,0]
prio_queue2 = [0,0]
prio_queue3 = [0,0]
prio_queue4 = [0,0]
prio_queue5 = [0,0]
prio_queue6 = [0,0]
prio_queue7 = [0,0]

mac_configuration_table = make_table_by_layout(mac_configuration_table_layout, layoutid_map)
c.append(mac_configuration_table)

for i in range(NO_ETH_PORTS):
	mac_configuration_table.append({
        "INGRESS"    : 1,
        "EGRESS"     : 1,
        "DYN_LEARN"  : 1,
        "RETAG"      : 0,
        "DRPUNTAG"   : 0,
        "DRPDTAG"    : 0,
        "DRPNONA664" : 0,
        "EGR_MIRR"   : 0,
        "ING_MIRR"   : 0,
        "VLANID"     : default_vlan,
        "VLANPRIO"   : 0,
        "TP_DELOUT"  : 0,
        "TP_DELIN"   : 0,
        "SPEED"      : speed[i], 
        "IFG"        : 0,
        "ENABLED[0]" : queue_enable[0],         # enable the queue
        "BASE[0]"    : prio_queue0[0],          # start
        "TOP[0]"     : prio_queue0[1],          # set the size of the queue to maximum size
        "ENABLED[1]" : queue_enable[1],
        "BASE[1]"    : prio_queue1[0],
        "TOP[1]"     : prio_queue1[1],
        "ENABLED[2]" : queue_enable[2],
        "BASE[2]"    : prio_queue2[0],
        "TOP[2]"     : prio_queue2[1],
        "ENABLED[3]" : queue_enable[3],
        "BASE[3]"    : prio_queue3[0],
        "TOP[3]"     : prio_queue3[1],
        "ENABLED[4]" : queue_enable[4],
        "BASE[4]"    : prio_queue4[0],
        "TOP[4]"     : prio_queue4[1],
        "ENABLED[5]" : queue_enable[5],
        "BASE[5]"    : prio_queue5[0],
        "TOP[5]"     : prio_queue5[1],
        "ENABLED[6]" : queue_enable[6],
        "BASE[6]"    : prio_queue6[0],
        "TOP[6]"     : prio_queue6[1],
        "ENABLED[7]" : queue_enable[7],
        "BASE[7]"    : prio_queue7[0],
        "TOP[7]"     : prio_queue7[1]})

#############################################################################
# VLAN Lookup Table
#############################################################################

vlan_lookup_table = make_table_by_layout(vlan_lookup_table_layout, layoutid_map)
c.append(vlan_lookup_table)

# Default VLAN
vlan_lookup_table.append({
    "VING_MIRR"  : 0,
    "VEGR_MIRR"  : 0,
    "VMEMB_PORT" : 0x1F,        # All ports are member of this VLAN
    "VLAN_BC"    : 0x1F,        # Broadcast domain for this VLAN
    "TAG_PORT"   : 0x00,        # Egress frames are untagged
    "VLANID"     : default_vlan})

# VLAN 1
vlan_lookup_table.append({
    "VING_MIRR"  : 0,
    "VEGR_MIRR"  : 0,
    "VMEMB_PORT" : 0x1F,        # All ports are member of this VLAN
    "VLAN_BC"    : 0x1F,        # Broadcast domain for this VLAN
    "TAG_PORT"   : 0x1F,        # Egress frames are untagged
    "VLANID"     : 1})

#############################################################################
# L2 Lookup Parameters - optional
#############################################################################

l2_lookup_parameters_table = make_table_by_layout(l2_lookup_parameters_table_layout, layoutid_map)
c.append(l2_lookup_parameters_table)

l2_lookup_parameters_table.append({
    "MAXAGE"            : 0,    # No aging
    "DYN_TBSZ"          : 4,
    "POLY"              : 0x97, # def. by Koopman: HD=4 at max. length 501
    "SHARED_LEARN"      : 0})

#############################################################################
# L2 Policing Table
#############################################################################

# No policing - only minimal necessary entries

l2_policing_table = make_table_by_layout(l2_policing_table_layout, layoutid_map)
c.append(l2_policing_table)

for i in range(NO_ETH_PORTS * NO_PRIORITIES):
    l2_policing_table.append({
        "SHARINDX"         : i / NO_PRIORITIES, #setting nSharedIdx for a given port to a single policing block for all prios
        "SMAX"             : 2**16-1,
        "RATE"             : 2**16-1,           # 1000*2**6
        "MAXLEN"           : 1518,
        "PARTITION"        : 0})

############################################################################
# L2 Forwarding Table
#############################################################################

l2_forwarding_table = make_table_by_layout(l2_forwarding_table_layout, layoutid_map)
c.append(l2_forwarding_table)

# retain the priority of the frames at ingress
for i in range(NO_ETH_PORTS):
    reachable_ports  = 0x1F & ~(1 << i)
    broadcast_domain = 0x1F & ~(1 << i)
    default_route    = 0x1F & ~(1 << i)

    # Priority regeneration
    priority_map     = [0,1,2,3,4,5,6,7]  # No PCP modification	

    l2_forwarding_table.append({
        "FL_DOMAIN"    : default_route,
        "BC_DOMAIN"    : broadcast_domain,
        "REACH_PORT"   : reachable_ports,
        "VLAN_PMAP[0]" : priority_map[0],
        "VLAN_PMAP[1]" : priority_map[1],
        "VLAN_PMAP[2]" : priority_map[2],
        "VLAN_PMAP[3]" : priority_map[3],
        "VLAN_PMAP[4]" : priority_map[4],
        "VLAN_PMAP[5]" : priority_map[5],
        "VLAN_PMAP[6]" : priority_map[6],
        "VLAN_PMAP[7]" : priority_map[7]})

# how to map it to the egress queue: all get mapped to queue 0
for i in range(NO_PRIORITIES):
    l2_forwarding_table.append({
        "VLAN_PMAP[0]" : 0,
        "VLAN_PMAP[1]" : 0,
        "VLAN_PMAP[2]" : 0,
        "VLAN_PMAP[3]" : 0,
        "VLAN_PMAP[4]" : 0})
  

#############################################################################
# L2 Forwarding Parameters Table
#############################################################################

l2_forwarding_parameters_table = make_table_by_layout(l2_forwarding_parameters_table_layout, layoutid_map)
c.append(l2_forwarding_parameters_table)

l2_forwarding_parameters_table.append({
    "MAX_DYNP"    : 0,
    "PART_SPC[7]" : 0,
    "PART_SPC[6]" : 0,
    "PART_SPC[5]" : 0,
    "PART_SPC[4]" : 0,
    "PART_SPC[3]" : 0,
    "PART_SPC[2]" : 0,
    "PART_SPC[1]" : 0,
    "PART_SPC[0]" : 910}
)

assert sum([l2_forwarding_parameters_table.entries[0]["PART_SPC[%d]" % i] for i in range(8)]) <= 910, 'sum of paritions must not exceed 910 (if retagging used)'

#############################################################################
# MII Mode Control Parameters
#############################################################################

mii_mode_parameters_table = make_table_by_layout(mii_mode_parameters_table_layout, layoutid_map)
c.append(mii_mode_parameters_table)

MII    = 0
RMII   = 1
RGMII  = 2
UNUSED = 3

PHY_MODE = 1
MAC_MODE = 0

mii_mode_parameters_table.append({
    "xMII_MODE[0]"    : RGMII,
    "PHY_MAC[0]"      : 0,  # not applicable for RGMII,
    "xMII_MODE[1]"    : RGMII,
    "PHY_MAC[1]"      : 0,  # not applicable for RGMII,
    "xMII_MODE[2]"    : RGMII,
    "PHY_MAC[2]"      : 0,  # not applicable for RGMII,
    "xMII_MODE[3]"    : RGMII,
    "PHY_MAC[3]"      : 0,  # not applicable for RGMII,
    "xMII_MODE[4]"    : RGMII,
    "PHY_MAC[4]"      : 0,  # not applicable for RGMII,
})

#############################################################################
# Write out hex file
#############################################################################

filename = os.path.basename(__file__)
filename   = filename.replace(".py", "_SJA1105.hex")

c.deviceid = SJA1105_DEVICEID
c.to_hex(filename)

#############################################################################
# Executive Summary
#############################################################################

print "Number of bytes: %d" %(len(c.to_bytes()))
print "======================"

print c
