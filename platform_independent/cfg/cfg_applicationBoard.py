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

import os
from ethsw.configuration import *
from ethsw.tables_sja1105pqrs import *

NO_CBS_BLOCKS = 16
NO_ETH_PORTS = 5
NO_PRIORITIES = 8

c = Configuration()

#############################################################################
# General Parameters
#############################################################################

general_parameters = make_table_by_layout(general_parameters_table_layout, layoutid_map)
c.append(general_parameters)
general_parameters.append({
    "VLLUPFORMAT": 0,
    "MIRR_PTACU": 1,
    "SWITCHID": 0,
    "HOSTPRIO": 5,
    "MAC_FLTRES[0]": 0x0180C200000E,
    "MAC_FLTRES[1]": 0x0180C2000003,
    "MAC_FLT[0]": 0xFFFFFF0000FF,
    "MAC_FLT[1]": 0xFFFFFF0000FF,
    "INCL_SRCPT[0]": 1,
    "INCL_SRCPT[1]": 1,
    "SEND_META[0]": 1,
    "SEND_META[1]": 0,
#   "CASC_PORT": 6,
    "CASC_PORT": 7,
    "MIRR_PORT": 6,
    "HOST_PORT": 0,
    "VIMARKER": 0xFFFFFFFF,
    "VIMASK": 0xFFFFFFFF,
    "TPID": 0x88A8,
    "IGNORE2STF": 0,
    "TPID2": 0x8100,
    "QUEUE_TS": 0,
    "EGRMIRRVID": 0,
    "EGRMIRRPCP": 0,
    "EGRMIRRDEI": 0,
    "REPLAY_PORT": 7
})

#############################################################################
# MAC Configuration Table
#############################################################################

SPEED_HOST = 0  # speed set by host during run-time
SPEED_1GBPS = 1
SPEED_100MBPS = 2
SPEED_10MBPS = 3

speed = [SPEED_1GBPS, SPEED_100MBPS, SPEED_100MBPS, SPEED_100MBPS, SPEED_100MBPS]
speed_Mbps = [10 ** (4 - x) for x in speed]

default_vlan = 0xFFF  # Default VLAN ID on all ports for untagged frames

queue_enable = [1, 1, 1, 1, 1, 1, 1, 1]
prio_queue0 = [0, 63]
prio_queue1 = [64, 127]
prio_queue2 = [128, 191]
prio_queue3 = [192, 255]
prio_queue4 = [256, 319]
prio_queue5 = [320, 383]
prio_queue6 = [384, 447]
prio_queue7 = [448, 511]

mac_configuration_table = make_table_by_layout(mac_configuration_table_layout, layoutid_map)
c.append(mac_configuration_table)

for i in range(NO_ETH_PORTS):
    mac_configuration_table.append({
        "INGMIRRDEI": 0,
        "INGMIRRPCP": 0,
        "INGMIRRVID": 0,
        "MIRRCETAG": 0,
        "MIRRCIE": 0,
        "INGRESS": 1,
        "EGRESS": 1,
        "DYN_LEARN": 1,
        "DRPNONA664": 0,
        "EGR_MIRR": 0,
        "ING_MIRR": 0,
        "VLANID": default_vlan,
        "VLANPRIO": 0,
        "MAXAGE": 255,
        "TP_DELOUT": 0,
        "TP_DELIN": 0,
        "SPEED": speed[i],
        "IFG": 0,
        "ENABLED[0]": queue_enable[0],  # enable the queue
        "BASE[0]": prio_queue0[0],  # start
        "TOP[0]": prio_queue0[1],  # set the size of the queue to maximum size
        "ENABLED[1]": queue_enable[1],
        "BASE[1]": prio_queue1[0],
        "TOP[1]": prio_queue1[1],
        "ENABLED[2]": queue_enable[2],
        "BASE[2]": prio_queue2[0],
        "TOP[2]": prio_queue2[1],
        "ENABLED[3]": queue_enable[3],
        "BASE[3]": prio_queue3[0],
        "TOP[3]": prio_queue3[1],
        "ENABLED[4]": queue_enable[4],
        "BASE[4]": prio_queue4[0],
        "TOP[4]": prio_queue4[1],
        "ENABLED[5]": queue_enable[5],
        "BASE[5]": prio_queue5[0],
        "TOP[5]": prio_queue5[1],
        "ENABLED[6]": queue_enable[6],
        "BASE[6]": prio_queue6[0],
        "TOP[6]": prio_queue6[1],
        "ENABLED[7]": queue_enable[7],
        "BASE[7]": prio_queue7[0],
        "TOP[7]": prio_queue7[1]})

#############################################################################
# Credit-Based Shaping Table
#############################################################################

# bandwidth given in Mbit/s
# Example configuration for 4 shapers in port 0
# shapers = [
#     {"bandwidth": 0.5, "prio": 7, "port": 0},
#     {"bandwidth": 5, "prio": 6, "port": 0},
#     {"bandwidth": 10, "prio": 5, "port": 0},
#     {"bandwidth": 20, "prio": 4, "port": 0}]
shapers = []

if len(shapers) > 0:
	credit_based_shaping_table = make_table_by_layout(credit_based_shaping_table_layout, layoutid_map)
	c.append(credit_based_shaping_table)

	assert len(shapers) <= NO_CBS_BLOCKS, "No of shapers configured exceeds %d" % (
	    NO_CBS_BLOCKS)

	for shaper in shapers:
	    idle_slope = shaper[
	                     "bandwidth"] / 8.0 * 10 ** 6  # conversion from Mbit/s to byte/s
	    rate = speed_Mbps[
	               shaper["port"]] / 8.0 * 10 ** 6  # rate at the port in byte/s
	    send_slope = rate - idle_slope
	    credit_based_shaping_table.append({
	        "IDLE_SLOPE": long(idle_slope),
	        "SEND_SLOPE": long(send_slope),
	        "CREDIT_HI": 0x6FFFFFFF,
	        "CREDIT_LO": 0x6FFFFFFF,
	        "CBS_PRIO": shaper["prio"],
	        "CBS_PORT": shaper["port"]})
	    print(idle_slope)

#############################################################################
# VLAN Lookup Table
#############################################################################

vlan_lookup_table = make_table_by_layout(vlan_lookup_table_layout, layoutid_map)
c.append(vlan_lookup_table)

# Default VLAN
vlan_lookup_table.append({
    "VING_MIRR": 0,
    "VEGR_MIRR": 0,
    "VMEMB_PORT": 0x1F,  # All ports are member of the VLAN
    "VLAN_BC": 0x1F,  # Broadcast domain for the VLAN
    "TAG_PORT": 0x00,  # Egress frames are untagged
    "VLANID": default_vlan})

# Enable VLANs 0 to 15
for i in range(16):
    vlan_lookup_table.append({
        "VING_MIRR": 0,
        "VEGR_MIRR": 0,
        "VMEMB_PORT": 0x1F,  # all ports are member
        "VLAN_BC": 0x1F,  # Broadcast domain
        "TAG_PORT": 0x1F,  # Egress frames are tagged
        "VLANID": i})

#############################################################################
# L2 Lookup Parameters Table
#############################################################################

l2_lookup_parameters_table = make_table_by_layout(l2_lookup_parameters_table_layout, layoutid_map)
c.append(l2_lookup_parameters_table)

l2_lookup_parameters_table.append({
    "LEARN_ONCE": 0,
    "OWR_DYN": 0,
    "USE_STATIC": 0,
    "NO_MGMT_LEARN": 1,
    "NO_ENF_HOSTPRT": 0,
    "DRPNOLEARN": 0,
    "START_DYNSPC": 0,
    "MAXAGE": 0,
    "MAXADDRP[0]": 1024,
    "MAXADDRP[1]": 1024,
    "MAXADDRP[2]": 1024,
    "MAXADDRP[3]": 1024,
    "MAXADDRP[4]": 1024,
    "DRPUNI": 0,
    "DRPMC": 0,
    "DRPBC": 0})

#############################################################################
# L2 Address Lookup Table
#############################################################################

l2_address_lookup_table = make_table_by_layout(l2_address_lookup_table_layout, layoutid_map)
c.append(l2_address_lookup_table)

l2_address_lookup_table.append(
    {
        "INDEX": 0,
        "ENFPORT": 0,
        "DESTPORTS": 1 << 0,
        "MACADDR": 0x001094000001,
        "VLANID": 555,
        "IOTAG": 0,
        "MASK_MACADDR": 0xFFFFFFFFFFFF,
        "MASK_VLANID": 0xFFF,
        "MASK_IOTAG": 0x1,
        "RETAG": 0,
        "MIRR": 0,
        "TAKETS": 0,
        "MIRRVLAN": 0,
        "TSREG": 0}
)

#############################################################################
# L2 Policing Table
#############################################################################

l2_policing_table = make_table_by_layout(l2_policing_table_layout, layoutid_map)
c.append(l2_policing_table)

# In this example, every port/priority is assigned a dedicated policer.
# By use of SHARINDX, multiple ports/priorities can be mapped to a single policer.

# defines the ratio of available bandwidth that is admitted for a prio/port combination in %
# a value of 100 means that no policing is performed
ratio = [[100, 100, 100, 100, 100, 100, 100, 100],  # Port 0 - no policing
         [100, 100, 100, 100, 100, 100, 100, 100],  # Port 1 - no policing
         [100, 100, 100, 100, 100, 100, 100, 100],  # Port 2 - no policing
         [100, 100, 100, 100, 100, 100, 100, 100],  # Port 3 - no policing
         [100, 100, 100, 100, 100, 100, 100, 100]]  # Port 4 - no policing

for port in range(NO_ETH_PORTS):
    for prio in range(NO_PRIORITIES):
        l2_policing_table.append({
            "SHARINDX": port * NO_PRIORITIES + prio,  # individual policing block for each priority
            "SMAX": 10 * 1526,        # Maximum burst as 10 maximum sized, double tagged frames
            "RATE": long(speed_Mbps[port] * ratio[port][prio] / 100.0 * 1000 / 15.625),        # Unit: [15.625 kbps]
            "MAXLEN": 1526,
            "PARTITION": prio})  # memory is partitioned towards the priorities

# Broadcast Storm Prevention
# Defines the ratio of available bandwidth that is admitted for a port
# ratio = [port 0, port 1, ..., port 4]
ratio = [100, 100, 100, 100, 100]  # All ports admit up to 100% broadcast traffic

for port in range(5):
    l2_policing_table.append({
        "SHARINDX": 40 + port,  # individual policing block for each priority
        "SMAX": 10 * 1526,    # Maximum burst as 10 maximum sized, double tagged frames
        "RATE": long(speed_Mbps[port] * ratio[port] / 100.0 * 1000 / 15.625),    # Unit: [15.625 kbps]
        "MAXLEN": 1526,
        "PARTITION": 0})  # memory is partitioned towards the priorities

############################################################################
# L2 Forwarding Table
#############################################################################

l2_forwarding_table = make_table_by_layout(l2_forwarding_table_layout, layoutid_map)
c.append(l2_forwarding_table)

# retain the priority of the frames at ingress
for i in range(NO_ETH_PORTS):
    if (i == general_parameters.entries[0]["HOST_PORT"]):
        reachable_ports = 0x1F  # host port is reachable by itself, needed for hybrid AVB implementation with endpoint and bridge stack on a single host
    else:
        reachable_ports = 0x1F & ~(1 << i)
    broadcast_domain = 0x1F & ~(1 << i)
    default_route    = 0x1F & ~(1 << i)

    # Priority regeneration
    priority_map     = [0,1,2,3,4,5,6,7]  # No PCP modification

    l2_forwarding_table.append({
        "FL_DOMAIN"    : default_route,
        "BC_DOMAIN"      : broadcast_domain,
        "REACH_PORT"   : reachable_ports,
        "VLAN_PMAP[0]" : priority_map[0],
        "VLAN_PMAP[1]" : priority_map[1],
        "VLAN_PMAP[2]" : priority_map[2],
        "VLAN_PMAP[3]" : priority_map[3],
        "VLAN_PMAP[4]" : priority_map[4],
        "VLAN_PMAP[5]" : priority_map[5],
        "VLAN_PMAP[6]" : priority_map[6],
        "VLAN_PMAP[7]" : priority_map[7]})

# Output PCP to queue mapping
# map priority i to queue i on all ports
for i in range(NO_PRIORITIES):
    l2_forwarding_table.append({
        "VLAN_PMAP[0]" : i,
        "VLAN_PMAP[1]" : i,
        "VLAN_PMAP[2]" : i,
        "VLAN_PMAP[3]" : i,
        "VLAN_PMAP[4]" : i})

#############################################################################
# L2 Forwarding Parameters Table
#############################################################################

l2_forwarding_parameters_table = make_table_by_layout(l2_forwarding_parameters_table_layout, layoutid_map)
c.append(l2_forwarding_parameters_table)

l2_forwarding_parameters_table.append({
    "MAX_DYNP"    : 0,
    "PART_SPC[7]" : 100,
    "PART_SPC[6]" : 100,
    "PART_SPC[5]" : 100,
    "PART_SPC[4]" : 100,
    "PART_SPC[3]" : 100,
    "PART_SPC[2]" : 100,
    "PART_SPC[1]" : 100,
    "PART_SPC[0]" : 210}
)

assert sum([l2_forwarding_parameters_table.entries[0]["PART_SPC[%d]" % i] for i in range(8)]) <= 910, 'sum of paritions must not exceed 910 (if retagging used)'


#############################################################################
# AVB Parameters
#############################################################################

avb_parameters = make_table_by_layout(avb_parameters_table_layout, layoutid_map)
c.append(avb_parameters)

avb_parameters.append({
    "SRCMETA"  : 0x026037C0FFEE,
    "DESTMETA" : 0x026037DECADE,
    "CAS_MASTER": 1,
    "L2CBS" :  0
})

#############################################################################
# MII Mode Control Parameters
#############################################################################

mii_mode_parameters = make_table_by_layout(mii_mode_parameters_table_layout, layoutid_map)
c.append(mii_mode_parameters)

MII    = 0
RMII   = 1
RGMII  = 2
UNUSED = 3
SGMII  = 3

PHY_MODE = 1
MAC_MODE = 0

mii_mode_parameters.append({
    "xMII_MODE[0]": RGMII,
    "PHY_MAC[0]": 0,  # not applicable for RGMII,
    "xMII_MODE[1]": MII,
    "PHY_MAC[1]": MAC_MODE,
    "xMII_MODE[2]": MII,
    "PHY_MAC[2]": MAC_MODE,
    "xMII_MODE[3]": RMII,
    "PHY_MAC[3]": MAC_MODE,
    "xMII_MODE[4]": RMII,
    "PHY_MAC[4]": MAC_MODE,  # not applicable for RGMII,
})



#############################################################################
# Write out hex file
#############################################################################

filename = os.path.basename(__file__)
filename   = filename.replace(".py", ".hex")

c.deviceid = 0xae00030e
c.to_hex("output/" + filename)

#############################################################################
# Executive Summary
#############################################################################

print "Number of bytes: %d" %(len(c.to_bytes()))
print "======================"
