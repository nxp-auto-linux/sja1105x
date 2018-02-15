/*
 * Copyright 2017 - 2018 NXP
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 * \file nxp_sja1105p_switchdev.c
 *
 * \author Marco Hartmann
 *
 * \date 2017-08-02
 *
 */

#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/rtnetlink.h>
#include <net/switchdev.h>
#include <net/netlink.h>
#include <linux/of_mdio.h>

#include "NXP_SJA1105P_addressResolutionTable.h"
#include "NXP_SJA1105P_diagnostics.h"
#include "NXP_SJA1105P_vlan.h"
#include "NXP_SJA1105P_config.h"
#include "NXP_SJA1105P_portConfig.h"

#include "sja1105p_switchdev.h"

#define PRODUCT_NAME "SJA1105P"
#define PNAME_LEN 22U
#define ARL_TABLE_SIZE 1024U
#define DTS_NAME_LEN 8U

extern int verbosity;
static struct sja1105p_context_data **sja1105p_context_arr;
typedef enum {UP, DOWN} linkstatus_t;

/* struct declarations */
struct nxp_port_data_struct {
	unsigned long ppid;
	int port_num;
	int is_host;
	unsigned char *base_mac;
	struct net_device *netdev;
	int link_state;
};

struct nxp_private_data_struct {
	struct nxp_port_data_struct **ports;
};


/* global struct that holds port information */
static struct nxp_private_data_struct nxp_private_data;

/****************************nw stubs******************************************/

/* Adds an FDB entry to dev for addr */
static int nxp_port_fdb_add(struct ndmsg *ndm, struct nlattr *tb[],
			    struct net_device *netdev,
			    const unsigned char *addr, u16 vid,
			    u16 nlm_flags)
{
	int err;
	struct nxp_port_data_struct *nxp_port;
	SJA1105P_addressResolutionTableEntry_t entry;

	nxp_port = netdev_priv(netdev);
	memset(&entry, 0, sizeof(SJA1105P_addressResolutionTableEntry_t));

	if (verbosity > 1)
		netdev_alert(netdev, "nxp_port_fdb_add was called [%d]! Add [%02x:%02x:%02x:%02x:%02x:%02x] in vlan [%x] to device [%s], flags [%x]\n",
		nxp_port->port_num, *(addr+0), *(addr+1), *(addr+2), *(addr+3), *(addr+4), *(addr+5), vid, netdev->name, nlm_flags);

	/* in case there already exists an entry corresponding to the MAC addr,
	 * retrieve it to add current port to the port mask
	 */
	memcpy (&entry.dstMacAddress, addr, sizeof (entry.dstMacAddress));
	err = SJA1105P_readArlTableEntryByAddress(&entry);
	if (err)
		netdev_alert(netdev, "No existing entry found, creating new one\n");

	/* build table entry,
	 * the index and enabled fields are filled
	 * by the SJA1105P_addArlTableEntry function
	 */
	entry.ports |= (1 << nxp_port->port_num);
	entry.vlanId = vid;

	/* add to sw using function from the sja1105p driver module */
	err = SJA1105P_addArlTableEntry(&entry);
	if(err)
		goto sja1105p_write_error;

	return 0;

sja1105p_write_error:
	netdev_err(netdev, "Could not add entry to arl table of sja1105p!");
	return err;
}

/* Deletes the FDB entry from dev coresponding to addr */
static int nxp_port_fdb_del(struct ndmsg *ndm, struct nlattr *tb[],
			    struct net_device *netdev,
			    const unsigned char *addr, u16 vid)
{
	int err;
	struct nxp_port_data_struct *nxp_port;
	SJA1105P_addressResolutionTableEntry_t entry;

	nxp_port = netdev_priv(netdev);
	memset(&entry, 0, sizeof(SJA1105P_addressResolutionTableEntry_t));

	if (verbosity > 1)
		netdev_alert(netdev, "nxp_port_fdb_del was called [%d]! Del [%02x:%02x:%02x:%02x:%02x:%02x] in vlan [%x] from device [%s]\n",
		nxp_port->port_num, *(addr+0), *(addr+1), *(addr+2), *(addr+3), *(addr+4), *(addr+5), vid, netdev->name);

	memcpy (&entry.dstMacAddress, addr, sizeof (entry.dstMacAddress));
	err = SJA1105P_readArlTableEntryByAddress(&entry);
	if (err)
		goto sja1105p_entry_not_found;

	/* disable current port */
	entry.ports &= ~(1 << nxp_port->port_num);

	/* if some  other port is still active:
	 * add the modified entry to the table
	 * otherwise: delete entry from table
	 */
	if (entry.ports) {
		if (verbosity > 1)
			netdev_alert(netdev, "deactivated port, upload modified entry\n");

		err = SJA1105P_addArlTableEntry(&entry);
		if(err)
			goto sja1105p_write_error;
	} else {
		if (verbosity > 1)
			netdev_alert(netdev, "deactivated last port, also remove table entry\n");

		/* build table entry,
		 * entry to be removed is identified by MAC and vid,
		 * the index is retrived by SJA1105P_removeArlTableEntryByAddress
		 */
		entry.vlanId = vid;

		/* del from sw using function from the sja1105p driver module */
		err = SJA1105P_removeArlTableEntryByAddress(&entry);
		if(err) {
			netdev_err(netdev, "Could not delete entry!");
			return err;
		}
	}

	return 0;

sja1105p_entry_not_found:
	netdev_err(netdev, "Entry does not exist!");
	return err;

sja1105p_write_error:
	netdev_err(netdev, "Could not add entry to arl table of sja1105p!");
	return err;
}

static int nxp_send_netlink_msg(struct nxp_port_data_struct *nxp_port,
				struct sk_buff *skb,
				struct netlink_callback *cb,
				int vid, char *mac_addr)
{
	int portid, seq, type, flags;
	struct nlmsghdr *nlmsg_header;
	struct ndmsg *ndm;

	/* numerical identifier that is assigned by Netlink,
	 * different port-ID values are used to identify several socket
	 * channels opened by the same user-space process
	 */
	portid = NETLINK_CB(cb->skb).portid;

	/* the message sequence number */
	seq = cb->nlh->nlmsg_seq;

	/* NEWNEIGH := New or updated neighbour entry */
	type = RTM_NEWNEIGH;

	/* NLM_F_MULTI := multipart message */
	flags = NLM_F_MULTI;

	/* Add a new netlink message to the skb */
	nlmsg_header = nlmsg_put(skb, portid, seq, type, sizeof(*ndm), flags);
	if (!nlmsg_header)
		return -EMSGSIZE;

	/* get the head of the message payload */
	ndm = nlmsg_data(nlmsg_header);

	/* fill out ndmsg structure (RTM_NEWNEIGH indicates presence of ndm) */
	ndm->ndm_family	 = AF_BRIDGE; /* Multiprotocol bridge */
	ndm->ndm_pad1    = 0;
	ndm->ndm_pad2    = 0;
	ndm->ndm_flags	 = NTF_SELF;
	ndm->ndm_type	 = 0;
	ndm->ndm_ifindex = nxp_port->netdev->ifindex; /* interface index */
	ndm->ndm_state   = NUD_REACHABLE; /* Network Unreachability Detection */

	/* add addr and vid as additional attributes */
	if (nla_put(skb, NDA_LLADDR, ETH_ALEN, mac_addr))
		goto nla_put_failure;

	if (vid && nla_put_u16(skb, NDA_VLAN, vid))
		goto nla_put_failure;

	nlmsg_end(skb, nlmsg_header);
	return 0;

nla_put_failure:
	nlmsg_cancel(skb, nlmsg_header);
	return -EMSGSIZE;
}

/* Used to add FDB entries to dump requests. Implementers should add entries
 * to skb and update idx with the number of entries
 */
static int nxp_port_fdb_dump(struct sk_buff *skb,
			     struct netlink_callback *cb,
			     struct net_device *netdev,
			     struct net_device *filter_dev,
			     int idx)
{
	int vid;
	char *mac_addr;

	int index, err;
	struct nxp_port_data_struct *nxp_port;

	nxp_port = netdev_priv(netdev);

	if (verbosity > 1)
		netdev_alert(netdev, "nxp_port_fdb_dump was called (%d)! idx [%d], arg is [%ld]%s\n",
		nxp_port->port_num, idx, cb->args[0], ((idx < cb->args[0])?" (skipping)":""));

	for (index = 0; index < ARL_TABLE_SIZE; index++) {
		SJA1105P_addressResolutionTableEntry_t entry;
		entry.index = index;

		/* get table entry at position index from arl table */
		err = SJA1105P_readArlTableEntryByIndex(&entry);
		if (err)
			goto sja1105p_read_error;

		/* skip without incrementing idx if entry is not valid (empty)
		 * or belongs to a different port
		 */
		if (!entry.enabled || !(entry.ports & BIT(nxp_port->port_num))) {
			continue;
		}

		/* On the first pass, cb->args[0] is always 0, idx is
		 * incremented with every valid table entry.
		 * Skip the netlink send on second pass,
		 * cb->args[0] now holds the total number
		 * of entries for all devices,
		 * idx must still be incremented per valid table entry.
		 */
		if (idx < cb->args[0]) {
			idx++;
			continue;
		}

		mac_addr = kzalloc(17*sizeof(char), GFP_KERNEL);
		if (!mac_addr)
			return -ENOMEM;

		vid = entry.vlanId;
		ether_addr_copy(mac_addr, (char*)&entry.dstMacAddress);

		if (verbosity > 1)
			netdev_alert(netdev, "discovered [%02x:%02x:%02x:%02x:%02x:%02x] in vlan [%x] on device [%s]\n",
			*(mac_addr+0), *(mac_addr+1), *(mac_addr+2), *(mac_addr+3), *(mac_addr+4), *(mac_addr+5), vid, netdev->name);

		/* send data via netlink message */
		err = nxp_send_netlink_msg(nxp_port, skb, cb, vid, mac_addr);
		kfree(mac_addr);
		if (err)
			goto send_error;

		idx++;
	}

	return idx;

sja1105p_read_error:
	netdev_err(netdev, "Could not read table entry from sja1105p!\n");
	return err;

send_error:
	netdev_err(netdev, "nla put faillure, could not send netlink msg\n");
	return err;
}

static int nxp_port_get_phys_port_name(struct net_device *netdev,
				       char *buf, size_t len)
{
	struct nxp_port_data_struct *nxp_port;
	nxp_port = netdev_priv(netdev);

	if (snprintf(buf, len, netdev->name) >= len)
		return -EINVAL;

	if (verbosity > 3) {
		netdev_alert(netdev, "nxp_port_get_phys_port_name was called for [%d], name is [%s]\n",
		nxp_port->port_num, buf);
	}

	return 0;
}

static void set_port_linkstatus(struct net_device *netdev, linkstatus_t s)
{
	int needlock, flags;

	flags = dev_get_flags(netdev);
	if (s == UP)
		flags |= IFF_UP;
	else
		flags &= ~IFF_UP;

	/* dev_change_flags requires rtnl mutex to be locked when called.
	 * register_netdev calls nxp_get_stats with rtnl locked already,
	 * syscalls however (for example by ifconfig)
	 * do not lock rtnl before calling nxp_get_stats.
	 * Thus we must check if the mutex is locked,
	 * and only lock it if it is not (which defeats its purpose)
	 */
	needlock =  !rtnl_is_locked();
	if (needlock)
		rtnl_lock();

	netdev->operstate = (s == UP) ? IF_OPER_UP : IF_OPER_DOWN;
	dev_change_flags(netdev, flags);

	if (needlock)
		rtnl_unlock();
}

struct rtnl_link_stats64* nxp_get_stats(struct net_device *netdev,
					struct rtnl_link_stats64 *storage)
{
	int err;
	uint64_t tx_bytes, tx_packets, rx_bytes, rx_packets;
	uint32_t rx_crc_errors, rx_length_errors, polerr, vlanerr, n664err;
	uint32_t not_reach, egr_disabled, part_drop, qfull;
	uint32_t addr_not_learned_drop, empty_route_drop, illegal_double_drop;
	uint32_t double_tagged_drop, single_outer_drop, single_inner_drop, untagged_drop;
	SJA1105P_macLevelErrors_t p_macLevelErrors;
	struct nxp_port_data_struct *nxp_port = netdev_priv(netdev);

	/* indicate the linkstate of the port, show host port as always up */
	if (nxp_port->link_state || nxp_port->is_host) {
		if (verbosity > 0)
			netdev_alert(netdev, "Change link status of [%s] to: up\n", netdev->name);

		netdev->flags |= IFF_UP;

		/* if flag change is not visible via dev_get_flags,
		 * use dev_change_flags to commit the change
		 */
		if (!(dev_get_flags(netdev) & IFF_UP))
			set_port_linkstatus(netdev, UP);
	} else {
		if (verbosity > 0)
			netdev_alert(netdev, "Change link status of [%s] to: down\n", netdev->name);

		netdev->flags &= ~IFF_UP;

		if (dev_get_flags(netdev) & IFF_UP)
			set_port_linkstatus(netdev, DOWN);
	}

	/* get stats from switch using the sja1105p drv functions */
	err = SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_OCTETS,
					       &tx_bytes,
					       nxp_port->port_num,
					       SJA1105P_e_etherStatDirection_EGRESS);

	err += SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_PKTS,
						&tx_packets,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_EGRESS);

	err += SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_OCTETS,
						&rx_bytes,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_INGRESS);

	err += SJA1105P_get64bitEtherStatCounter(SJA1105P_e_etherStat64_N_PKTS,
						&rx_packets,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_INGRESS);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_CRCERR,
						&rx_crc_errors,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_SIZEERR,
						&rx_length_errors,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_NOT_REACH,
						&not_reach,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_EGR_DISABLED,
						&egr_disabled,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_PART_DROP,
						&part_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_QFULL,
						&qfull,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_POLERR,
						&polerr,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_VLANERR,
						&vlanerr,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_N664ERR,
						&n664err,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_ADDR_NOT_LEARNED_DROP,
						&addr_not_learned_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_EMPTY_ROUTE_DROP,
						&empty_route_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_ILLEGAL_DOUBLE_DROP,
						&illegal_double_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_DOUBLE_TAGGED_DROP,
						&double_tagged_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_SINGLE_OUTER_DROP,
						&single_outer_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_SINGLE_INNER_DROP,
						&single_inner_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_get32bitEtherStatCounter(SJA1105P_e_etherStat32_N_UNTAGGED_DROP,
						&untagged_drop,
						nxp_port->port_num,
						SJA1105P_e_etherStatDirection_BOTH);

	err += SJA1105P_getMacErrors(&p_macLevelErrors, nxp_port->port_num);

	if (err)
		goto sja1105p_read_error;

	/* fill out the provided struct */
	storage->tx_bytes = tx_bytes;
	storage->tx_packets = tx_packets;
	storage->rx_bytes = rx_bytes;
	storage->rx_packets = rx_packets;

	storage->rx_crc_errors = rx_crc_errors;
	storage->rx_length_errors = rx_length_errors;
	storage->rx_frame_errors = p_macLevelErrors.nAlignerr;

	storage->tx_errors = not_reach + egr_disabled;
	storage->rx_errors = rx_crc_errors + rx_length_errors + part_drop + p_macLevelErrors.nSoferr + p_macLevelErrors.nMiierr + p_macLevelErrors.nAlignerr;

	storage->tx_dropped = qfull;
	storage->rx_dropped = part_drop + polerr + vlanerr + n664err;
	storage->rx_dropped += addr_not_learned_drop + empty_route_drop + illegal_double_drop + double_tagged_drop + single_outer_drop + single_inner_drop + untagged_drop;

	if (verbosity > 3) {
		netdev_alert(netdev, "nxp_get_stats was called for [%d]: rxb [%llu], txb [%llu],"
		"rxp [%llu], txp [%llu], rx_crc_errors[%u], rx_length_errors[%u],"
		"not_reach[%u], egr_disabled[%u], part_drop[%u], qfull[%u],"
		"polerr[%u], vlanerr[%u], n664err[%u]\n",
		nxp_port->port_num, tx_bytes, tx_packets, rx_bytes, rx_packets,
		rx_crc_errors, rx_length_errors, not_reach, egr_disabled, part_drop,
		qfull, polerr, vlanerr, n664err);
	}

	return storage;

sja1105p_read_error:
	netdev_err(netdev, "Could not read stats from sja1105p!\n");
	return storage;
}

/* this function is called when a VLAN id is registered */
static int nxp_port_vlan_rx_add_vid(struct net_device *netdev,
				    __be16 proto, u16 vid)
{
	int err, i;
	uint16_t ports_enable;
	struct nxp_port_data_struct *nxp_port;
	SJA1105P_vlanForwarding_t vlanFwd[SJA1105P_N_LOGICAL_PORTS];

	nxp_port = netdev_priv(netdev);

	if (verbosity > 1)
		netdev_alert(netdev, "nxp_port_vlan_rx_add_vid was called for [%d], vid is [%d], proto is [%d]\n",
		nxp_port->port_num, vid, proto);

	/* retrieve current configuration from switch */
	err = SJA1105P_readVlanConfig(vid, vlanFwd, &ports_enable);

	if (err == 1) {
		/* configuration for the specified vid was not yet present,
		 * so create an initial configuration
		 */
		ports_enable = 0;
		for (i=0; i<SJA1105P_N_LOGICAL_PORTS; i++) {
			vlanFwd[i] = SJA1105P_e_vlanForwarding_NOT;
		}
	}

	/* change forwarding status of current port */
	vlanFwd[nxp_port->port_num] = SJA1105P_e_vlanForwarding_TAGGED;

	/* enable output from the current port */
	ports_enable |= (1 << nxp_port->port_num);

	/* write modified cfg to switch */
	err = SJA1105P_writeVlanConfig(vid, vlanFwd, ports_enable);
	if (err)
		goto sja1105p_write_error;

	return 0;

sja1105p_write_error:
	netdev_err(netdev, "Could not write vlan cfg to sja1105p!\n");
	return err;
}

/* this function is called when a VLAN id is unregistered */
static int nxp_port_vlan_rx_kill_vid(struct net_device *netdev,
				     __be16 proto, u16 vid)
{
	int err;
	uint16_t ports_enable;
	struct nxp_port_data_struct *nxp_port;
	SJA1105P_vlanForwarding_t vlanFwd[SJA1105P_N_LOGICAL_PORTS];

	nxp_port = netdev_priv(netdev);

	if (verbosity > 1)
		netdev_alert(netdev, "nxp_port_vlan_rx_kill_vid was called for [%d], vid is [%d], proto is [%d]\n",
		nxp_port->port_num, vid, proto);

	/* retrieve current configuration from switch */
	err = SJA1105P_readVlanConfig(vid, vlanFwd, &ports_enable);

	/* only modify cfg if a cfg for the specified vid exists already */
	if (err == 0) {
		/* change forwarding status of current port */
		vlanFwd[nxp_port->port_num] = SJA1105P_e_vlanForwarding_NOT;

		/* disable output from the current port */
		ports_enable &= ~(1 << nxp_port->port_num);

		/* write modified cfg to switch */
		err = SJA1105P_writeVlanConfig(vid, vlanFwd, ports_enable);
		if (err)
			goto sja1105p_write_error;
	}

	return 0;

sja1105p_write_error:
	netdev_err(netdev, "Could not write vlan cfg to sja1105p!\n");
	return err;
}

/************************swdev stubs*******************************************/

/* led to get an ID of the switch chip this port is part of.
 * If driver implements this, it indicates that it represents a port of
 * a switch chip.
 */
static int nxp_port_swdev_parent_id_get(struct net_device *netdev,
					struct netdev_phys_item_id *psid)
{
	struct nxp_port_data_struct *nxp_port = netdev_priv(netdev);
	u32 n = nxp_port->ppid;

	psid->id[0] = (n >> 24) & 0xFF;
	psid->id[1] = (n >> 16) & 0xFF;
	psid->id[2] = (n >>  8) & 0xFF;
	psid->id[3] = (n >>  0) & 0xFF;
	psid->id_len = 4;

	if (verbosity > 3)
		netdev_alert(netdev, "nxp_port_swdev_parent_id_get was called for [%d], ppid is [%lu]\n",
		nxp_port->port_num, nxp_port->ppid);

	return 0;
}

/* Called to notify switch device port of bridge port STP state change */
static int nxp_port_swdev_port_stp_update(struct net_device *netdev, u8 state)
{
	char *s;
	struct nxp_port_data_struct *nxp_port;
	nxp_port = netdev_priv(netdev);

	/* implementation not needed, as sja1105p does not support STP */

	switch (state) {
	case BR_STATE_DISABLED:
		s = "DISABLED";
		break;
	case BR_STATE_LISTENING:
		s = "LISTENING";
		break;
	case BR_STATE_LEARNING:
		s = "LEARNING";
		break;
	case BR_STATE_FORWARDING:
		s = "FORWARDING";
		break;
	case BR_STATE_BLOCKING:
		s = "BLOCKING";
		break;
	default:
		s = "unknown_state";
	}
	if (verbosity > 3)
		netdev_alert(netdev, "nxp_port_swdev_port_stp_update was called [%d], change state to [%s]!\n",
		nxp_port->port_num, s);

	return 0;
}

/* Called to add/modify IPv4 route to switch device */
static int nxp_port_swdev_fib_ipv4_add(struct net_device *netdev,
				       __be32 dst, int dst_len,
				       struct fib_info *fi,
				       u8 tos, u8 type,
				       u32 nlflags, u32 tb_id)
{
	struct nxp_port_data_struct *nxp_port;
	nxp_port = netdev_priv(netdev);

	//TODO implement

	if (verbosity > 3)
		netdev_alert(netdev, "nxp_port_swdev_fib_ipv4_add was called [%d], IP: [%d.%d.%d.%d]!\n",
		nxp_port->port_num, (((unsigned char*) &dst)[0]), (((unsigned char*) &dst)[1]), (((unsigned char*) &dst)[2]), (((unsigned char*) &dst)[3]));

	return 0;
}

/* Called to delete IPv4 route from switch device */
static int nxp_port_swdev_fib_ipv4_del(struct net_device *netdev,
					  __be32 dst, int dst_len,
					  struct fib_info *fi,
					  u8 tos, u8 type, u32 tb_id)
{
	struct nxp_port_data_struct *nxp_port;
	nxp_port = netdev_priv(netdev);

	//TODO implement

	if (verbosity > 3)
		netdev_alert(netdev, "nxp_port_swdev_fib_ipv4_del was called [%d], IP: [%d.%d.%d.%d]!\n",
		nxp_port->port_num, (((unsigned char*) &dst)[0]), (((unsigned char*) &dst)[1]), (((unsigned char*) &dst)[2]), (((unsigned char*) &dst)[3]));

	return 0;
}

/* Called when ports are toggled to reset delay lines */
static void nxp_toggle_port(int port_num)
{
	SJA1105P_port_t physicalPort;
	SJA1105P_portStatusMiixArgument_t portStatus;

	SJA1105P_getPhysicalPort(port_num, &physicalPort);

	SJA1105P_getPortStatusMiix(&portStatus, physicalPort.physicalPort,
				   physicalPort.switchId);

	if (portStatus.xmiiMode == SJA1105P_e_xmiiMode_RGMII) {
		SJA1105P_resetClockDelay(physicalPort.physicalPort,
					 physicalPort.switchId,
					 SJA1105P_e_direction_RX);
	}
}

/***************************link_state callback********************************/

// is called on phydev state machine changes
static void nxp_adjust_link(struct net_device *netdev)
{
	struct nxp_port_data_struct *nxp_port;
	struct phy_device *phydev;

	nxp_port = netdev_priv(netdev);
	phydev = netdev->phydev;

	if (phydev) {
		if (verbosity > 3)
			netdev_alert(netdev, "nxp_adjust_link was called for %s: phy [%x]: state: [%x], link [%x]\n",
			netdev->name, phydev->addr, phydev->state, phydev->link);

		if (phydev->link) {
			/* If we just came up */
			if (nxp_port->link_state == 0) {
				nxp_toggle_port(nxp_port->port_num);
			}
			nxp_port->link_state = 1;
		} else {
			nxp_port->link_state = 0;
		}
	}
}

/**********************************nw_ops**************************************/
static const struct net_device_ops nxp_port_netdev_ops = {
	.ndo_fdb_add			= nxp_port_fdb_add,
	.ndo_fdb_del			= nxp_port_fdb_del,
	.ndo_fdb_dump			= nxp_port_fdb_dump,
	.ndo_get_stats64		= nxp_get_stats,
	.ndo_vlan_rx_add_vid		= nxp_port_vlan_rx_add_vid,
	.ndo_vlan_rx_kill_vid		= nxp_port_vlan_rx_kill_vid,
	.ndo_get_phys_port_name		= nxp_port_get_phys_port_name,
};

/**********************************sw_ops**************************************/
static const struct swdev_ops nxp_port_swdev_ops = {
	.swdev_parent_id_get		= nxp_port_swdev_parent_id_get,
	.swdev_port_stp_update		= nxp_port_swdev_port_stp_update,
	.swdev_fib_ipv4_add		= nxp_port_swdev_fib_ipv4_add,
	.swdev_fib_ipv4_del		= nxp_port_swdev_fib_ipv4_del,
};




/******************************registration************************************/
/* get the device tree node corresponding to a given logical port,
 * returns NULL if not found
 * Returns node with incremented refcount, needs to decremented again when done
 */
struct device_node *get_dt_node_for_port(int lport)
{
	struct device_node *switch_dt_node = NULL;
	struct device_node *port_dt_node;

	char phy_dts_name[DTS_NAME_LEN];
	SJA1105P_port_t pport;

	/* device tree uses the the physical port numbers,
	 * need to get the physical port number corresponing to logical lport
	 */
	if (SJA1105P_getPhysicalPort(lport, &pport))
		goto mapping_error;

	/* get the DT node of the SJA1105P */
	if (sja1105p_context_arr)
		switch_dt_node = sja1105p_context_arr[pport.switchId]->of_node;
	if (!switch_dt_node)
		goto node_not_found;

	/* get the DT node of specified port, node is child of switch_dt_node */
	scnprintf(phy_dts_name, DTS_NAME_LEN, "port-%d", pport.physicalPort);
	port_dt_node = of_get_child_by_name(switch_dt_node, phy_dts_name);

	/* decrement refcount, was incremented by of_find_node_by_name */
	of_node_put(switch_dt_node);

	return port_dt_node;

mapping_error:
	dev_err(&nxp_private_data.ports[lport]->netdev->dev, "SJA1105P_getPhysicalPort returned an error \n");
	return NULL;

node_not_found:
	dev_err(&nxp_private_data.ports[lport]->netdev->dev, "Could not find dt node for port %d\n", lport);
	return NULL;
}

/* find phydev corresponding to port number, return NULL if not found */
struct phy_device *find_phydev(struct net_device *netdev, int lport)
{
	struct device_node *port_dt_node;
	struct device_node *phy_dt_node;
	struct phy_device *phydev;

	port_dt_node = get_dt_node_for_port(lport);
	if (!port_dt_node)
		return NULL;

	/* the switch port node contains a reference (phandle) called "phy-ref"
	 * to the ethernet phy it is connected to. Retrieve the phy node
	 */
	phy_dt_node = of_parse_phandle(port_dt_node, "phy-ref", 0);

	/* decrement refcount, was incremented by get_dt_node_for_port */
	of_node_put(port_dt_node);

	if (!phy_dt_node)
		goto phandle_error;

	/* retrieve the phydev associated to the DT node */
	phydev = of_phy_find_device(phy_dt_node);

	/* decrement refcount, was incremented by of_parse_phandle */
	of_node_put(phy_dt_node);

	return phydev;

phandle_error:
	if (verbosity > 0)
		netdev_alert(netdev, "Port %d does not have a (valid) phandle to an ethernet phy\n", lport);
	return NULL;
}

/* connect corresponding phydev to the port-netdev */
void attach_phydev(struct net_device *netdev)
{
	int err;
	struct nxp_port_data_struct *nxp_port;
	struct phy_device *phydev;

	nxp_port = netdev_priv(netdev);
	phydev = find_phydev(netdev, nxp_port->port_num);
	if(!phydev)
		goto phydev_not_found;

	/* this sets the phy state to PHY_READY and starts the phy state machine
	 * If no Phy driver is loaded until now, a generic one is assumed.
	 * Later loading of phy driver will have no effect, as phy already bound
	 */
	err = phy_connect_direct(netdev, phydev, &nxp_adjust_link, PHY_INTERFACE_MODE_RGMII);
	if (err)
		goto phydev_attach_error;

	/* force one iteration of the phy state machine
	 * This ensures adjust_link() is called at least once
	 */
	phydev->state = PHY_CHANGELINK;

	if (verbosity > 0)
		netdev_alert(netdev, "connected phy [%x] (state: [%x], link [%x]) to port [%d], attached_dev [%s]\n",
			phydev->addr, phydev->state, phydev->link, nxp_port->port_num, (phydev->attached_dev)?phydev->attached_dev->name:"none");

	return;

phydev_not_found:
	if (verbosity > 0)
		netdev_alert(netdev, "No phydev found for %s\n", netdev->name);
	return;

phydev_attach_error:
	netdev_err(netdev, "Error: could not attach phydev to %s [err=%d]\n", netdev->name, err);
	return;
}

int is_hostport(struct device *dev, int lport)
{
	int err;
	SJA1105P_port_t pport;
	struct sja1105p_context_data *sw_ctx;

	err = SJA1105P_getPhysicalPort(lport, &pport);
	if (err)
		goto mapping_error;

	sw_ctx = sja1105p_context_arr[pport.switchId];

	if (verbosity > 0)
		dev_alert(dev, "lport %d belongs to sw %d, pport %d: is host: %d\n", lport, pport.switchId, pport.physicalPort, sw_ctx->pdata->ports[pport.physicalPort].is_host);

	return sw_ctx->pdata->ports[pport.physicalPort].is_host;

mapping_error:
	dev_err(&nxp_private_data.ports[lport]->netdev->dev, "SJA1105P_getPhysicalPort returned an error \n");
	return 0;
}

/* set up net devices and private memory */
int register_ports(struct nxp_private_data_struct *pr_data)
{
	int err, port = 0, alloc_size;
	struct spi_device *spidev;

	alloc_size = sizeof(struct nxp_port_data_struct *) * SJA1105P_N_LOGICAL_PORTS;

	/* map of all ports */
	pr_data->ports = kzalloc(alloc_size, GFP_KERNEL);
	if (!pr_data->ports)
		goto allocation_error;

	/* create SJA1105P_N_LOGICAL_PORTS new net devices */
	for(port=0; port<SJA1105P_N_LOGICAL_PORTS; port++) {
		char *port_name;
		struct net_device *netdev;
		struct nxp_port_data_struct *nxp_port;
		SJA1105P_port_t physicalPortInfo;

		netdev = alloc_etherdev(sizeof(struct nxp_port_data_struct));
		if (!netdev)
			goto allocation_error;

		/* get private memory from netdev,
		 * which is located behind the netdev struct
		 */
		nxp_port = netdev_priv(netdev);
		pr_data->ports[port] = nxp_port;

		/* get info about the physical port */
		err = SJA1105P_getPhysicalPort(port, &physicalPortInfo);
		if (err)
			goto sja1105p_read_error;

		spidev = sja1105p_context_arr[physicalPortInfo.switchId]->spi_dev;

		/* populate nxp_port */
		nxp_port->netdev = netdev;
		nxp_port->port_num = port;
		nxp_port->ppid = physicalPortInfo.switchId;
		nxp_port->is_host = is_hostport(&spidev->dev, port);

		/* give dev a meaningful name */
		port_name = kzalloc(sizeof(char) * PNAME_LEN, GFP_KERNEL);
		if (!port_name)
			goto allocation_error;
		scnprintf(port_name, PNAME_LEN, "%s_p%d%s", PRODUCT_NAME, port, nxp_port->is_host?"*":"");
		err = dev_alloc_name(netdev, port_name);
		if (err)
			goto allocation_error;

		//TODO setup MAC address

		/* populate netdev */
		netdev->netdev_ops = &nxp_port_netdev_ops;
		netdev->swdev_ops = &nxp_port_swdev_ops;


		/* Flags:
		 * 	- Does not change network namespaces. Device is network
		 * 	  namespace local. These nw devices are not allowed
		 * 	  to move between nw namespaces
		 * 	- HW switch offload: feature flag for all switch asic
		 * 	  offloads. Switch drivers set this flag on switch ports
		*/
		//TODO taken from rocker: recheck, possibly expand
		netdev->features |= NETIF_F_NETNS_LOCAL |
				    NETIF_F_HW_SWITCH_OFFLOAD |
				    NETIF_F_VLAN_FEATURES;

		//TODO set priv_flags

		/* bind a phydev to the netdev */
		attach_phydev(netdev);

		if (verbosity > 4) {
			dev_info(&spidev->dev, "Port [%d] belongs to [%d], netdev name: [%s]\n", port, physicalPortInfo.physicalPort, port_name);
		}

		err = register_netdev(netdev);
		if (err)
			goto netdev_registration_error;

		if (verbosity > 0)
			netdev_info(netdev, "registered netdevice: [%s]\n", netdev->name);
	}

	return 0;

allocation_error:
	pr_err("register_ports failed for port %d: memory allocation error\n", port);
	return -ENOMEM;

sja1105p_read_error:
	pr_err("register_ports failed: could not read physical port data from sja1105p for port %d!\n", port);
	return err;

netdev_registration_error:
	pr_err("register_netdev failed for port [%d]\n", port);
	free_netdev(pr_data->ports[port]->netdev);
	pr_data->ports[port]->netdev = NULL;
	return err;
}

void unregister_ports(struct nxp_private_data_struct *pr_data)
{
	int i;

	if (!pr_data->ports)
		return;

	for(i=0; i<SJA1105P_N_LOGICAL_PORTS; i++) {
		struct net_device *netdev = pr_data->ports[i]->netdev;
		if (!netdev)
			continue;

		if (netdev->phydev) {
			struct phy_device *phydev = netdev->phydev;

			if (verbosity > 0)
				netdev_alert(netdev, "disconnecting phy from [%s]\n", netdev->name);
			phy_disconnect(phydev); 	/* this stops the phy state machine */

			/* decrement the refcount,
			 * that was incremented by of_phy_find_device
			 */
			put_device(&phydev->dev);
		}

		if (verbosity > 0)
			netdev_alert(netdev, "unregistering: [%s]\n", netdev->name);
		unregister_netdev(netdev);
	}
}

/* module init function */
int nxp_swdev_init(struct sja1105p_context_data **ctx_nodes)
{
	sja1105p_context_arr = ctx_nodes;
	return register_ports(&nxp_private_data);
}

/* module exit function */
void nxp_swdev_exit(void)
{
	unregister_ports(&nxp_private_data);
}
