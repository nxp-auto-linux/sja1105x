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

import struct

# Schedule Table
# ID: 0
schedule_table_layout = [
   ['WINSTINDEX', 10, 0x0],
   ['WINEND', 1, 0x0],
   ['WINST', 1, 0x0],
   ['DESTPORTS', 5, 0x0],
   ['SETVALID', 1, 0x0],
   ['TXEN', 1, 0x0],
   ['RESMEDIA', 9, 0x0],
   ['VLINDEX', 10, 0x0],
   ['DELTA', 18, 0x0],
]

# Schedule Entry Points Table
# ID: 1
schedule_entry_points_table_layout = [
   ['SUBSCHINDX', 3, 0x0],
   ['DELTA', 18, 0x0],
   ['ADDRESS', 10, 0x0],
]

# VL Lookup Table
# ID: 2
vl_lookup_table_layout_0 = [
   ['DESTPORTS', 5, 0x0],
   ['ISCRITICAL', 1, 0x0],
   ['MACADDR', 48, 0x0],
   ['VLANID', 12, 0x0],
   ['PORT', 3, 0x0],
   ['VLANPRIO', 3, 0x0],
]

vl_lookup_table_layout_1 = [
   ['EGRMIRR', 5, 0x0],
   ['INGRMIRR', 1, 0x0],
   ['VLID', 16, 0x0],
   ['PORT', 3, 0x0],
]

# VL Policing Table
# ID: 3
tt_vl_layout = [
   ['1', 1, 0x0],
   ['MAXLEN', 11, 0x0],
   ['SHARINDX', 10, 0x0],
]

rc_vl_layout = [
   ['0', 1, 0x0],
   ['MAXLEN', 11, 0x0],
   ['SHARINDX', 10, 0x0],
   ['BAG', 14, 0x0],
   ['JITTER', 10, 0x0],
]

# VL Forwarding Table
# ID: 4
vl_forwarding_table_layout = [
   ['TYPE', 1, 0x0],
   ['PRIORITY', 3, 0x0],
   ['PARTITION', 3, 0x0],
   ['DESTPORTS', 5, 0x0],
]

# MAC Configuration Table
# ID: 9
mac_configuration_table_layout = [
   ['TOP[7]', 9, 0x0],
   ['BASE[7]', 9, 0x0],
   ['ENABLED[7]', 1, 0x0],
   ['TOP[6]', 9, 0x0],
   ['BASE[6]', 9, 0x0],
   ['ENABLED[6]', 1, 0x0],
   ['TOP[5]', 9, 0x0],
   ['BASE[5]', 9, 0x0],
   ['ENABLED[5]', 1, 0x0],
   ['TOP[4]', 9, 0x0],
   ['BASE[4]', 9, 0x0],
   ['ENABLED[4]', 1, 0x0],
   ['TOP[3]', 9, 0x0],
   ['BASE[3]', 9, 0x0],
   ['ENABLED[3]', 1, 0x0],
   ['TOP[2]', 9, 0x0],
   ['BASE[2]', 9, 0x0],
   ['ENABLED[2]', 1, 0x0],
   ['TOP[1]', 9, 0x0],
   ['BASE[1]', 9, 0x0],
   ['ENABLED[1]', 1, 0x0],
   ['TOP[0]', 9, 0x0],
   ['BASE[0]', 9, 0x0],
   ['ENABLED[0]', 1, 0x0],
   ['IFG', 5, 0x0],
   ['SPEED', 2, 0x0],
   ['TP_DELIN', 16, 0x0],
   ['TP_DELOUT', 16, 0x0],
   ['MAXAGE', 8, 0x0],
   ['VLANPRIO', 3, 0x0],
   ['VLANID', 12, 0x0],
   ['ING_MIRR', 1, 0x0],
   ['EGR_MIRR', 1, 0x0],
   ['DRPNONA664', 1, 0x0],
   ['DRPDTAG', 1, 0x0],
   ['DRPSOTAG', 1, 0x0],
   ['DRPSITAG', 1, 0x0],
   ['DRPUNTAG', 1, 0x0],
   ['RETAG', 1, 0x0],
   ['DYN_LEARN', 1, 0x1],
   ['EGRESS', 1, 0x1],
   ['INGRESS', 1, 0x1],
   ['MIRRCIE', 1, 0x0],
   ['MIRRCETAG', 1, 0x0],
   ['INGMIRRVID', 12, 0x0],
   ['INGMIRRPCP', 3, 0x0],
   ['INGMIRRDEI', 1, 0x0],
]

# Retagging Table
# ID: 18
retagging_table_layout = [
   ['EGR_PORT', 5, 0x0],
   ['ING_PORT', 5, 0x0],
   ['VLAN_ING', 12, 0x0],
   ['VLAN_EGR', 12, 0x0],
   ['DO_NOT_LEARN', 1, 0x0],
   ['USE_DEST_PORTS', 1, 0x0],
   ['DESTPORTS', 5, 0x0],
]

# Schedule Parameters
# ID: 10
schedule_parameters_table_layout = [
   ['SUBSCHEIND[7]', 10, 0x0],
   ['SUBSCHEIND[6]', 10, 0x0],
   ['SUBSCHEIND[5]', 10, 0x0],
   ['SUBSCHEIND[4]', 10, 0x0],
   ['SUBSCHEIND[3]', 10, 0x0],
   ['SUBSCHEIND[2]', 10, 0x0],
   ['SUBSCHEIND[1]', 10, 0x0],
   ['SUBSCHEIND[0]', 10, 0x0],
]

# Schedule Entry Points Parameters
# ID: 11
schedule_entry_points_parameters_table_layout = [
   ['CLKSRC', 2, 0x0],
   ['ACTSUBSCH', 3, 0x0],
]

# VL Forwarding Parameters Table
# ID: 12
vl_forwarding_parameters_table_layout = [
   ['PART_SPC[7]', 10, 0x0],
   ['PART_SPC[6]', 10, 0x0],
   ['PART_SPC[5]', 10, 0x0],
   ['PART_SPC[4]', 10, 0x0],
   ['PART_SPC[3]', 10, 0x0],
   ['PART_SPC[2]', 10, 0x0],
   ['PART_SPC[1]', 10, 0x0],
   ['PART_SPC[0]', 10, 0x0],
   ['DEBUGEN', 1, 0x0],
]

# Clock Synchronization Parameters
# ID: 15
clock_synchronization_parameters_table_layout = [
   ['ETSSRCPCF', 48, 0x0],
   ['WAITTHSYNC', 4, 0x0],
   ['WFINTMOUT', 20, 0x0],
   ['UNSYTOTSYTH', 4, 0x0],
   ['UNSYTOSYTH', 4, 0x0],
   ['TSYTOSYTH', 4, 0x0],
   ['TSYTH', 4, 0x0],
   ['TSYTOUSYTH', 4, 0x0],
   ['SYTH', 4, 0x0],
   ['SYTOUSYTH', 4, 0x0],
   ['SYPRIORITY', 2, 0x0],
   ['SYDOMAIN', 4, 0x0],
   ['STTH', 4, 0x0],
   ['STTOINTTH', 4, 0x0],
   ['PCFSZE', 11, 0x40],
   ['PCFPRIORITY', 4, 0x9],
   ['OBVWINSZ', 15, 0x0],
   ['NUMUNSTBCY', 7, 0x0],
   ['NUMSTBCY', 7, 0x0],
   ['MAXTRANSPCLK', 27, 0x0],
   ['MAXINTEGCY', 8, 0x0],
   ['LISTENTMOUT', 30, 0x0],
   ['INTCYDUR', 27, 0x0],
   ['INTTOTENTTH', 4, 0x0],
   ['INTTOSYNCTH', 4, 0x0],
   ['VLIDOUT[4]', 16, 0x0],
   ['VLIDOUT[3]', 16, 0x0],
   ['VLIDOUT[2]', 16, 0x0],
   ['VLIDOUT[1]', 16, 0x0],
   ['VLIDOUT[0]', 16, 0x0],
   ['VLIDIMNMIN', 16, 0x0],
   ['VLIDINMAX', 16, 0x0],
   ['CAENTMOUT', 20, 0x0],
   ['ACCDEVWIN', 15, 0x0],
   ['VLIDSELECT', 1, 0x0],
   ['TENTSYRELEN', 1, 0x0],
   ['ASYTENSYEN', 1, 0x0],
   ['SYTOSTBEN', 1, 0x0],
   ['SYRELEN', 1, 0x0],
   ['SYSYEN', 1, 0x0],
   ['SYASYEN', 1, 0x0],
   ['IPCFRAMESY', 1, 0x0],
   ['STABASYEN', 1, 0x0],
   ['SWMASTER', 1, 0x0],
   ['FULLCBG', 1, 0x0],
   ['SRCPORT[7]', 3, 0x0],
   ['SRCPORT[6]', 3, 0x0],
   ['SRCPORT[5]', 3, 0x0],
   ['SRCPORT[4]', 3, 0x0],
   ['SRCPORT[3]', 3, 0x0],
   ['SRCPORT[2]', 3, 0x0],
   ['SRCPORT[1]', 3, 0x0],
   ['SRCPORT[0]', 3, 0x0],
   ['OUTPRTSLAVE[4]', 1, 0x0],
   ['OUTPRTSLAVE[3]', 1, 0x0],
   ['OUTPRTSLAVE[2]', 1, 0x0],
   ['OUTPRTSLAVE[1]', 1, 0x0],
   ['OUTPRTSLAVE[0]', 1, 0x0],
   ['OUTPRTMASTER[4]', 1, 0x0],
   ['OUTPRTMASTER[3]', 1, 0x0],
   ['OUTPRTMASTER[2]', 1, 0x0],
   ['OUTPRTMASTER[1]', 1, 0x0],
   ['OUTPRTMASTER[0]', 1, 0x0],
]

# General Parameters
# ID: 17
general_parameters_table_layout = [
   ['VLLUPFORMAT', 1, 0x1],
   ['MIRR_PTACU', 1, 0x0],
   ['SWITCHID', 3, 0x0],
   ['HOSTPRIO', 3, 0x0],
   ['MAC_FLTRES[1]', 48, 0x180c2000000],
   ['MAC_FLTRES[0]', 48, 0x180c2000000],
   ['MAC_FLT[1]', 48, 0xffffff000000],
   ['MAC_FLT[0]', 48, 0xffffff000000],
   ['INCL_SRCPT[1]', 1, 0x0],
   ['INCL_SRCPT[0]', 1, 0x0],
   ['SEND_META[1]', 1, 0x0],
   ['SEND_META[0]', 1, 0x0],
   ['CASC_PORT', 3, 0x5],
   ['HOST_PORT', 3, 0x5],
   ['MIRR_PORT', 3, 0x5],
   ['VIMARKER', 32, 0x0],
   ['VIMASK', 32, 0xffffffff],
   ['TPID', 16, 0x9100],
   ['IGNORE2STF', 1, 0x1],
   ['TPID2', 16, 0x8100],
   ['QUEUE_TS', 1, 0x0],
   ['EGRMIRRVID', 12, 0x0],
   ['EGRMIRRPCP', 3, 0x0],
   ['EGRMIRRDEI', 1, 0x0],
   ['REPLAY_PORT', 3, 0x7],
]

# L2 Address Lookup Table
# ID: 5
l2_address_lookup_table_layout = [
   ['TSREG', 1, 0x0],
   ['MIRRVLAN', 12, 0x0],
   ['TAKETS', 1, 0x0],
   ['MIRR', 1, 0x0],
   ['RETAG', 1, 0x0],
   ['MASK_IOTAG', 1, 0x1],
   ['MASK_VLANID', 12, 0xfff],
   ['MASK_MACADDR', 48, 0xffffffffffff],
   ['IOTAG', 1, 0x0],
   ['VLANID', 12, 0x0],
   ['MACADDR', 48, 0x0],
   ['DESTPORTS', 5, 0x0],
   ['ENFPORT', 1, 0x0],
   ['INDEX', 10, 0x0],
]

# VLAN Lookup Table
# ID: 7
vlan_lookup_table_layout = [
   ['VING_MIRR', 5, 0x0],
   ['VEGR_MIRR', 5, 0x0],
   ['VMEMB_PORT', 5, 0x0],
   ['VLAN_BC', 5, 0x0],
   ['TAG_PORT', 5, 0x0],
   ['VLANID', 12, 0x0],
]

# L2 Lookup Parameters Table
# ID: 13
l2_lookup_parameters_table_layout = [
   ['DRPBC', 5, 0x0],
   ['DRPMC', 5, 0x0],
   ['DRPUNI', 5, 0x0],
   ['MAXADDRP[4]', 11, 0x400],
   ['MAXADDRP[3]', 11, 0x400],
   ['MAXADDRP[2]', 11, 0x400],
   ['MAXADDRP[1]', 11, 0x400],
   ['MAXADDRP[0]', 11, 0x400],
   ['MAXAGE', 15, 0x0],
   ['START_DYNSPC', 10, 0x0],
   ['DRPNOLEARN', 5, 0x0],
   ['SHARED_LEARN', 1, 0x0],
   ['NO_ENF_HOSTPRT', 1, 0x0],
   ['NO_MGMT_LEARN', 1, 0x0],
   ['USE_STATIC', 1, 0x0],
   ['OWR_DYN', 1, 0x1],
   ['LEARN_ONCE', 1, 0x0],
]

# L2 Policing Table
# ID: 6
l2_policing_table_layout = [
   ['SHARINDX', 6, 0x0],
   ['SMAX', 16, 0x0],
   ['RATE', 16, 0x0],
   ['MAXLEN', 11, 0x0],
   ['PARTITION', 3, 0x0],
]

# L2 Forwarding Table
# ID: 8
l2_forwarding_table_layout = [
   ['BC_DOMAIN', 5, 0x0],
   ['REACH_PORT', 5, 0x0],
   ['FL_DOMAIN', 5, 0x0],
   ['VLAN_PMAP[7]', 3, 0x0],
   ['VLAN_PMAP[6]', 3, 0x0],
   ['VLAN_PMAP[5]', 3, 0x0],
   ['VLAN_PMAP[4]', 3, 0x0],
   ['VLAN_PMAP[3]', 3, 0x0],
   ['VLAN_PMAP[2]', 3, 0x0],
   ['VLAN_PMAP[1]', 3, 0x0],
   ['VLAN_PMAP[0]', 3, 0x0],
]

# L2 Forwarding Parameters Table
# ID: 14
l2_forwarding_parameters_table_layout = [
   ['MAX_DYNP', 3, 0x0],
   ['PART_SPC[7]', 10, 0x0],
   ['PART_SPC[6]', 10, 0x0],
   ['PART_SPC[5]', 10, 0x0],
   ['PART_SPC[4]', 10, 0x0],
   ['PART_SPC[3]', 10, 0x0],
   ['PART_SPC[2]', 10, 0x0],
   ['PART_SPC[1]', 10, 0x0],
   ['PART_SPC[0]', 10, 0x0],
]

# AVB Parameters
# ID: 16
avb_parameters_table_layout = [
   ['L2CBS', 1, 0x0],
   ['CAS_MASTER', 1, 0x0],
   ['DESTMETA', 48, 0x0],
   ['SRCMETA', 48, 0x0],
]

# MII Mode Parameters
# ID: 78
mii_mode_parameters_table_layout = [
   ['PHY_MAC[4]', 1, 0x0],
   ['xMII_MODE[4]', 2, 0x0],
   ['PHY_MAC[3]', 1, 0x0],
   ['xMII_MODE[3]', 2, 0x0],
   ['PHY_MAC[2]', 1, 0x0],
   ['xMII_MODE[2]', 2, 0x0],
   ['PHY_MAC[1]', 1, 0x0],
   ['xMII_MODE[1]', 2, 0x0],
   ['PHY_MAC[0]', 1, 0x0],
   ['xMII_MODE[0]', 2, 0x0],
]

# CGU Configuration
# ID: 128
cgu_configuration_table_layout = [
   ['RBUS', 32, 0x0],
   ['BASE_TPR_CLK_C', 32, 0x0],
   ['BASE_ISO_CLK_C', 32, 0x0],
   ['MII4_EXT_RX_CLK', 32, 0x15000000],
   ['MII4_EXT_TX_CLK', 32, 0x15000000],
   ['MII4_RGMII_TX_CLK', 32, 0x15000000],
   ['MII4_RMII_REF_CLK', 32, 0x15000000],
   ['MII4_MII_RX_CLK', 32, 0x15000000],
   ['MII4_MII_TX_CLK', 32, 0x15000000],
   ['MII3_EXT_RX_CLK', 32, 0x14000000],
   ['MII3_EXT_TX_CLK', 32, 0x14000000],
   ['MII3_RGMII_TX_CLK', 32, 0x14000000],
   ['MII3_RMII_REF_CLK', 32, 0x14000000],
   ['MII3_MII_RX_CLK', 32, 0x14000000],
   ['MII3_MII_TX_CLK', 32, 0x14000000],
   ['MII2_EXT_RX_CLK', 32, 0x13000000],
   ['MII2_EXT_TX_CLK', 32, 0x13000000],
   ['MII2_RGMII_TX_CLK', 32, 0x13000000],
   ['MII2_RMII_REF_CLK', 32, 0x13000000],
   ['MII2_MII_RX_CLK', 32, 0x13000000],
   ['MII2_MII_TX_CLK', 32, 0x13000000],
   ['MII1_EXT_RX_CLK', 32, 0x12000000],
   ['MII1_EXT_TX_CLK', 32, 0x12000000],
   ['MII1_RGMII_TX_CLK', 32, 0x12000000],
   ['MII1_RMII_REF_CLK', 32, 0x12000000],
   ['MII1_MII_RX_CLK', 32, 0x12000000],
   ['MII1_MII_TX_CLK', 32, 0x12000000],
   ['MII0_EXT_RX_CLK', 32, 0x11000000],
   ['MII0_EXT_TX_CLK', 32, 0x11000000],
   ['MII0_RGMII_TX_CLK', 32, 0x11000000],
   ['MII0_RMII_REF_CLK', 32, 0x11000000],
   ['MII0_MII_RX_CLK', 32, 0x11000000],
   ['MII0_MII_TX_CLK', 32, 0x11000000],
   ['BASE_PERPH_CLK_C', 32, 0x0],
   ['BASE_SWITCH_CLK_C', 32, 0x0],
   ['BASE_SAFE_CLK_C', 32, 0x0],
   ['IDIV_4_C', 32, 0xa000000],
   ['IDIV_3_C', 32, 0xa000000],
   ['IDIV_2_C', 32, 0xa000000],
   ['IDIV_1_C', 32, 0xa000000],
   ['IDIV_0_C', 32, 0xa000000],
   ['PLL_1_C', 32, 0xa000083],
   ['PLL_0_C', 32, 0x0],
   ['RFRQ', 32, 0x0],
]

# RGU Configuration
# ID: 129
rgu_configuration_table_layout = [
   ['DISABLE_IF', 32, 0x0],
]

# ACU Configuration
# ID: 130
acu_configuration_table_layout = [
   ['DISABLE_IF', 32, 0x0],
   ['RGMII_MEAS_SETUP', 32, 0x0],
   ['TS_CONFIG', 32, 0x65],
   ['CFG_PAD_JTAG', 32, 0x2000000],
   ['CFG_PAD_SPI', 32, 0x12040407],
   ['CFG_PAD_MISC', 32, 0x120412],
   ['CFG_PAD_MII4_ID', 32, 0x2323],
   ['CFG_PAD_MII3_ID', 32, 0x2323],
   ['CFG_PAD_MII2_ID', 32, 0x2323],
   ['CFG_PAD_MII1_ID', 32, 0x2323],
   ['CFG_PAD_MII0_ID', 32, 0x2323],
   ['CFG_PAD_MII4_RX', 32, 0x2020212],
   ['CFG_PAD_MII4_TX', 32, 0x12121212],
   ['CFG_PAD_MII3_RX', 32, 0x2020212],
   ['CFG_PAD_MII3_TX', 32, 0x12121212],
   ['CFG_PAD_MII2_RX', 32, 0x2020212],
   ['CFG_PAD_MII2_TX', 32, 0x12121212],
   ['CFG_PAD_MII1_RX', 32, 0x2020212],
   ['CFG_PAD_MII1_TX', 32, 0x12121212],
   ['CFG_PAD_MII0_RX', 32, 0x2020212],
   ['CFG_PAD_MII0_TX', 32, 0x12121212],
]

# SGMII Configuration
# ID: 200
sgmii_configuration_table_layout = [
   ['DIGITAL_ERROR_CNT', 32, 0x0],
   ['DIGITAL_CONTROL_2', 32, 0x0],
   ['VR_MII_SNPS_CR_DATA', 32, 0x0],
   ['VR_MII_SNPS_CR_ADDR', 32, 0x0],
   ['VR_MII_SNPS_CR_CTRL', 32, 0x0],
   ['VR_MII_Gen1_MISC_CTRL', 32, 0x100],
   ['VR_MII_Gen1_LVL_CTRL', 32, 0x23f],
   ['VR_MII_Gen1_MPLL_CTRL2', 32, 0xa],
   ['VR_MII_Gen1_MPLL_CTRL1', 32, 0x1c22],
   ['VR_MII_Gen1_MPLL_CTRL0', 32, 0x1],
   ['VR_MII_Gen1_RLOS_CTRL', 32, 0x3],
   ['VR_MII_Gen1_RDPLL_RST', 32, 0x0],
   ['VR_MII_Gen1_DPLL_MCTRL', 32, 0x1],
   ['VR_MII_Gen1_RXEQ_CTRL', 32, 0x5],
   ['VR_MII_Gen1_RXGCTRL', 32, 0x101],
   ['VR_MII_Gen1_TX_EDGRT_CTRL', 32, 0x0],
   ['VR_MII_Gen1_TX_GENCTRL', 32, 0x1],
   ['VR_MII_Gen1_TX_ATTN_CTRL', 32, 0x0],
   ['VR_MII_Gen1_TX_BSTCTRL', 32, 0xa],
   ['VR_MII_GPIO', 32, 0x0],
   ['VR_MII_LINK_TIMER_CTRL', 32, 0x0],
   ['VR_MII_EEE_RXTIMER', 32, 0x0],
   ['VR_MII_EEE_TXTIMER', 32, 0x0],
   ['VR_MII_EEE_MCTRL', 32, 0x899c],
   ['DEBUG_CONTROL', 32, 0x0],
   ['TEST_CONTROL', 32, 0x0],
   ['AUTONEG_INTR_STATUS', 32, 0xa],
   ['AUTONEG_CONTROL', 32, 0x0],
   ['DIGITAL_CONTROL_1', 32, 0x2400],
   ['AUTONEG_ADV', 32, 0x20],
   ['BASIC_CONTROL', 32, 0x1140],
   ['SR_VSMMD_CTRL', 32, 0x4],
   ['SR_VSMMD_PCS_ID2', 32, 0x0],
   ['SR_VSMMD_PCS_ID1', 32, 0x0],
   ['SR_VSMMD_DEV_ID2', 32, 0x0],
   ['SR_VSMMD_DEV_ID1', 32, 0x0],
]

# Credit-Based Shaping Table
# ID: 19
credit_based_shaping_table_layout = [
   ['CBS_PORT', 3, 0x0],
   ['CBS_PRIO', 3, 0x0],
   ['CREDIT_LO', 32, 0x7fffffff],
   ['CREDIT_HI', 32, 0x7fffffff],
   ['SEND_SLOPE', 32, 0x0],
   ['IDLE_SLOPE', 32, 0x0],
]

def chk_vl_lookup_table_layout_0(configuration, bytes):
   for t in configuration.tables:
      if t.tableid == 17:
         assert len (t.entries) == 1, "General Configuration Table is expected to have a single entry"
         return t.entries[0]['VLLUPFORMAT'] == 0
   print "WARN: table 17 not found"

def chk_vl_lookup_table_layout_1(configuration, bytes):
   for t in configuration.tables:
      if t.tableid == 17:
         assert len (t.entries) == 1, "General Configuration Table is expected to have a single entry"
         return t.entries[0]['VLLUPFORMAT'] == 1
   print "WARN: table 17 not found"

def chk_tt_vl_layout(configuration, bytes):
   selector = struct.unpack("<I",bytes[:4])[0] >> 31
   if selector == 1: return True
   return False

def chk_rc_vl_layout(configuration, bytes):
   selector = struct.unpack("<I",bytes[:4])[0] >> 31
   if selector == 0: return True
   return False

layoutid_map = [
   (schedule_table_layout, 0, None),
   (schedule_entry_points_table_layout, 1, None),
   (vl_lookup_table_layout_0, 2, chk_vl_lookup_table_layout_0),
   (vl_lookup_table_layout_1, 2, chk_vl_lookup_table_layout_1),
   (tt_vl_layout, 3, chk_tt_vl_layout),
   (rc_vl_layout, 3, chk_rc_vl_layout),
   (vl_forwarding_table_layout, 4, None),
   (mac_configuration_table_layout, 9, None),
   (retagging_table_layout, 18, None),
   (schedule_parameters_table_layout, 10, None),
   (schedule_entry_points_parameters_table_layout, 11, None),
   (vl_forwarding_parameters_table_layout, 12, None),
   (clock_synchronization_parameters_table_layout, 15, None),
   (general_parameters_table_layout, 17, None),
   (l2_address_lookup_table_layout, 5, None),
   (vlan_lookup_table_layout, 7, None),
   (l2_lookup_parameters_table_layout, 13, None),
   (l2_policing_table_layout, 6, None),
   (l2_forwarding_table_layout, 8, None),
   (l2_forwarding_parameters_table_layout, 14, None),
   (avb_parameters_table_layout, 16, None),
   (mii_mode_parameters_table_layout, 78, None),
   (cgu_configuration_table_layout, 128, None),
   (rgu_configuration_table_layout, 129, None),
   (acu_configuration_table_layout, 130, None),
   (sgmii_configuration_table_layout, 200, None),
   (credit_based_shaping_table_layout, 19, None),
]

tableid_map = {
   'L2 Forwarding Table' : 8,
   'VL Lookup Table' : 2,
   'L2 Lookup Parameters Table' : 13,
   'L2 Forwarding Parameters Table' : 14,
   'VL Forwarding Parameters Table' : 12,
   'CGU Configuration' : 128,
   'Schedule Table' : 0,
   'MII Mode Parameters' : 78,
   'General Parameters' : 17,
   'SGMII Configuration' : 200,
   'Retagging Table' : 18,
   'Schedule Entry Points Parameters' : 11,
   'VLAN Lookup Table' : 7,
   'Clock Synchronization Parameters' : 15,
   'AVB Parameters' : 16,
   'Schedule Entry Points Table' : 1,
   'MAC Configuration Table' : 9,
   'Schedule Parameters' : 10,
   'VL Forwarding Table' : 4,
   'L2 Policing Table' : 6,
   'Credit-Based Shaping Table' : 19,
   'VL Policing Table' : 3,
   'RGU Configuration' : 129,
   'ACU Configuration' : 130,
   'L2 Address Lookup Table' : 5,
}
