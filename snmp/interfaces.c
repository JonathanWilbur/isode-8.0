/* interfaces.c - MIB realization of the Interfaces group */

#include <net/if.h>
#include <stdlib.h>
#include <sys/socket.h>
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/snmp/RCS/interfaces.c,v 9.0 1992/06/16 12:38:11 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/snmp/RCS/interfaces.c,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 * Contributed by NYSERNet Inc.  This work was partially supported by the
 * U.S. Defense Advanced Research Projects Agency and the Rome Air Development
 * Center of the U.S. Air Force Systems Command under contract number
 * F30602-88-C-0016.
 *
 *
 * $Log: interfaces.c,v $
 * Revision 9.0  1992/06/16  12:38:11  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>
#include <string.h>
#include "mib.h"
#include "interfaces.h"
#ifdef	BSD44
#include <net/if_types.h>
#endif
#ifndef SVR4_UCB
#include <sys/ioctl.h>
#endif
#ifdef LINUX
#include <unistd.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <linux/if_packet.h>
#include <net/if_arp.h>
#endif

/*  */

#define	TYPE_MIN	1		/* ifType */
#define	TYPE_OTHER	1
#define	TYPE_ETHER	6
#define	TYPE_P10	12
#define	TYPE_P80	13
#define	TYPE_MAX	28


#define	ADMIN_MIN	1		/* ifAdminStatus */
#define	ADMIN_MAX	3

#define	OPER_UP		1		/* ifOperStatus */
#define	OPER_DOWN	2


/* we assume that all interfaces are present at startup time and that they
   don't move around in memory... */

int	ifNumber = 0;

struct interface *ifs = NULL;

static struct address	 *afs = NULL;
struct address *afs_inet = NULL;
#ifdef	BSD44
struct address *afs_iso = NULL;
#endif

static	int	flush_if_cache = 0;

int	get_interfaces ();
static struct address *find_address ();

/*  */

#define	ifIndex		0
#define	ifDescr		1
#define	ifType		2		/* SEMI IMPLEMENTED */
#define	ifMtu		3
#define	ifSpeed		4		/* SEMI IMPLEMENTED */
#define	ifPhysAddress	5
#define	ifAdminStatus	6
#define	ifOperStatus	7
#ifdef	BSD44
#define	ifLastChange	8
#endif
#ifdef	BSD44
#define	ifInOctets	9
#endif
#define	ifInUcastPkts	10
#ifdef	BSD44
#define	ifInNUcastPkts	11
#define	ifInDiscards	12
#endif
#define	ifInErrors	13
#ifdef	BSD44
#define	ifInUnknownProtos 14
#define	ifOutOctets	15
#endif
#define	ifOutUcastPkts	16
#ifdef	BSD44
#define	ifOutNUcastPkts	17
#endif
#define	ifOutDiscards	18
#define	ifOutErrors	19
#ifndef LINUX
#define	ifOutQLen	20
#endif
#define	ifSpecific	21


static int  o_interfaces (oi, v, offset)
OI	oi;
struct type_SNMP_VarBind *v;
int	offset;
{
	int	    ifnum,
			ifvar;
	struct interface *is;
	struct ifnet *ifn;
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
#ifdef	ifLastChange
	static   int lastq = -1;
	static   integer diff;
#endif

	if (get_interfaces (offset) == NOTOK)
		return generr (offset);

	ifvar = (int) ot -> ot_info;
	switch (offset) {
	case type_SNMP_PDUs_get__request:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1)
			return int_SNMP_error__status_noSuchName;
		ifnum = oid -> oid_elements[oid -> oid_nelem - 1];
		for (is = ifs; is; is = is -> ifn_next)
			if (is -> ifn_index == ifnum)
				break;
		if (is == NULL || !is -> ifn_ready)
			return int_SNMP_error__status_noSuchName;
		break;

	case type_SNMP_PDUs_get__next__request:
		if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
			OID	new;

			for (is = ifs; is; is = is -> ifn_next)
				if (is -> ifn_ready)
					break;
			if (!is)
				return NOTOK;
			ifnum = is -> ifn_index;

			if ((new = oid_extend (oid, 1)) == NULLOID)
				return NOTOK;
			new -> oid_elements[new -> oid_nelem - 1] = ifnum;

			if (v -> name)
				free_SNMP_ObjectName (v -> name);
			v -> name = new;
		} else {
			int	i = ot -> ot_name -> oid_nelem;
			struct interface *iz;

			if ((ifnum = oid -> oid_elements[i]) == 0) {
				if ((is = ifs) == NULL)
					return NOTOK;
				if (is -> ifn_ready)
					goto stuff_ifnum;
				ifnum = 1;
			}
			for (is = iz = ifs; is; is = is -> ifn_next)
				if ((iz = is) -> ifn_index == ifnum)
					break;
			for (is = iz -> ifn_next; is; is = is -> ifn_next)
				if (is -> ifn_ready)
					break;
			if (!is)
				return NOTOK;
stuff_ifnum:
			;
			ifnum = is -> ifn_index;

			oid -> oid_elements[i] = ifnum;
			oid -> oid_nelem = i + 1;
		}
		break;

	default:
		return int_SNMP_error__status_genErr;
	}
	ifn = &is -> ifn_interface.ac_if;

	switch (ifvar) {
	case ifIndex:
		return o_integer (oi, v, is -> ifn_index);

	case ifDescr:
		return o_string (oi, v, is -> ifn_descr, strlen (is -> ifn_descr));

	case ifType:
		if (is -> ifn_type < TYPE_MIN || is -> ifn_type > TYPE_MAX)
			is -> ifn_type = TYPE_OTHER;
		return o_integer (oi, v, is -> ifn_type);

	case ifMtu:
		return o_integer (oi, v, ifn -> if_mtu);

	case ifSpeed:
		return o_integer (oi, v, is -> ifn_speed);

	case ifPhysAddress:
#ifdef	NEW_AT
		return o_string (oi, v,
						 (char *) is -> ifn_interface.ac_enaddr,
						 sizeof is -> ifn_interface.ac_enaddr);
#else
		return o_string (oi, v,
						 (char *) is -> ifn_interface.ac_enaddr.ether_addr_octet,
						 sizeof is -> ifn_interface.ac_enaddr.ether_addr_octet);
#endif

	case ifAdminStatus:
		return o_integer (oi, v, is -> ifn_admin);

	case ifOperStatus:
		return o_integer (oi, v, ifn -> if_flags & IFF_UP ? OPER_UP
						  : OPER_DOWN);

#ifdef	ifLastChange
	case ifLastChange:
		if ((diff = (ifn -> if_lastchange.tv_sec - my_boottime.tv_sec)
					* 100
					+ ((ifn -> if_lastchange.tv_usec - my_boottime.tv_usec)
					   / 10000))
				< 0)
			diff = 0;
		return o_number (oi, v, (caddr_t) &diff);
#endif

#ifdef	ifInOctets
	case ifInOctets:
		return o_integer (oi, v, ifn -> if_ibytes);
#endif

	case ifInUcastPkts:
#ifndef	BSD44
		return o_integer (oi, v, ifn -> if_ipackets);
#else
		return o_integer (oi, v, ifn -> if_ipackets - ifn -> if_imcasts);
#endif

#ifdef	ifInNUcastPkts
	case ifInNUcastPkts:
		return o_integer (oi, v, ifn -> if_imcasts);
#endif

#ifdef	ifInDiscards
	case ifInDiscards:
		return o_integer (oi, v, ifn -> if_iqdrops);
#endif

	case ifInErrors:
		return o_integer (oi, v, ifn -> if_ierrors);

#ifdef	ifInUnknownProtos
	case ifInUnknownProtos:
		return o_integer (oi, v, ifn -> if_noproto);
#endif

#ifdef	ifOutOctets
	case ifOutOctets:
		return o_integer (oi, v, ifn -> if_obytes);
#endif

	case ifOutUcastPkts:
#ifndef	BSD44
		return o_integer (oi, v, ifn -> if_opackets);
#else
		return o_integer (oi, v, ifn -> if_opackets - ifn -> if_omcasts);
#endif

#ifdef	ifOutNUcastPkts
	case ifOutNUcastPkts:
		return o_integer (oi, v, ifn -> if_omcasts);
#endif

	case ifOutDiscards:
#ifdef LINUX
		return o_integer (oi, v, ifn -> if_oqdrops);
#else
		return o_integer (oi, v, ifn -> if_snd.ifq_drops);
#endif

	case ifOutErrors:
		return o_integer (oi, v, ifn -> if_oerrors);

#ifdef ifOutQLen
	case ifOutQLen:
		return o_integer (oi, v, ifn -> if_snd.ifq_len);
#endif

	case ifSpecific:
		return o_specific (oi, v, (caddr_t) nullSpecific);

	default:
		return int_SNMP_error__status_noSuchName;
	}
}

/*  */

static int  s_interfaces (oi, v, offset)
OI	oi;
struct type_SNMP_VarBind *v;
int	offset;
{
	int	    ifnum;
	int    i;
	struct interface *is;
	struct ifreq    ifreq;
	OID    oid = oi -> oi_name;
	OT	    ot = oi -> oi_type;
	OS	    os = ot -> ot_syntax;
	caddr_t value;

	switch (offset) {
	case type_SNMP_PDUs_set__request:
		if (get_interfaces (offset) == NOTOK)
			return generr (offset);
	/* and fall... */

	case type_SNMP_PDUs_commit:
	case type_SNMP_PDUs_rollback:
		if (oid -> oid_nelem != ot -> ot_name -> oid_nelem + 1)
			return int_SNMP_error__status_noSuchName;
		ifnum = oid -> oid_elements[oid -> oid_nelem - 1];
		for (is = ifs; is; is = is -> ifn_next)
			if (is -> ifn_index == ifnum)
				break;
		if (is == NULL || !is -> ifn_ready)
			return int_SNMP_error__status_noSuchName;
		break;

	default:
		return int_SNMP_error__status_genErr;
	}

	if (os == NULLOS) {
		advise (LLOG_EXCEPTIONS, NULLCP,
				"no syntax defined for object \"%s\"", ot -> ot_text);
		return int_SNMP_error__status_genErr;
	}

	switch (offset) {
	case type_SNMP_PDUs_set__request:
		if ((*os -> os_decode) (&value, v -> value) == NOTOK)
			return int_SNMP_error__status_badValue;
		i = *((integer *) value);
		(*os -> os_free) (value);
		switch (i) {
		case OPER_UP:
		case OPER_DOWN:
			break;

		default:
			return int_SNMP_error__status_badValue;
		}
		is -> ifn_touched = i;
		break;

	case type_SNMP_PDUs_commit:
		is -> ifn_admin = is -> ifn_touched;
		bzero ((char *) &ifreq, sizeof ifreq);
		strcpy (ifreq.ifr_name, is -> ifn_descr);
		if (ioctl (nd, SIOCGIFFLAGS, &ifreq) == NOTOK) {
			advise (LLOG_EXCEPTIONS, "failed", "SIOCGIFFLAGS on %s",
					is -> ifn_descr);
			break;
		} else {
			switch (is -> ifn_admin) {
			case OPER_UP:
				ifreq.ifr_flags |= IFF_UP;
				break;

			case OPER_DOWN:
				ifreq.ifr_flags &= ~IFF_UP;
				break;
			}
			if (ioctl (nd, SIOCSIFFLAGS, &ifreq) == NOTOK)
				advise (LLOG_EXCEPTIONS, "failed", "SIOCSIFFLAGS on %s",
						is -> ifn_descr);
		}
		flush_if_cache = 1;
	/* and fall... */

	case type_SNMP_PDUs_rollback:
		is -> ifn_touched = 0;
		break;
	}

	return int_SNMP_error__status_noError;
}

/*  */

set_interface (name, ava)
char   *name,
	   *ava;
{
	int	    i;
	u_long  l;
	char   *cp;
	struct interface *is;

	for (is = ifs; is; is = is -> ifn_next)
		if (strcmp (is -> ifn_descr, name) == 0)
			break;
	if (!is) {
		advise (LLOG_DEBUG, NULLCP, "no such interface as \"%s\"", name);
		return;
	}

	if ((cp = index (ava, '=')) == NULL)
		return;
	*cp++ = NULL;

	if (lexequ (ava, "ifType") == 0) {
		if (sscanf (cp, "%d", &i) != 1 || i < TYPE_MIN || i > TYPE_MAX) {
malformed:
			;
			advise (LLOG_EXCEPTIONS, NULLCP, "malformed attribute \"%s=%s\"",
					ava, cp);
			return;
		}

		switch (is -> ifn_type = i) {
		case TYPE_ETHER:
		case TYPE_P10:
			is -> ifn_speed = 10000000;
			break;

		case TYPE_P80:
			is -> ifn_speed = 80000000;
			break;

		default:
			break;
		}
		return;
	}

	if (lexequ (ava, "ifSpeed") == 0) {
		if (sscanf (cp, "%U", &l) != 1)
			goto malformed;

		is -> ifn_speed = l;
		return;
	}

	advise (LLOG_EXCEPTIONS, NULLCP, "unknown attribute \"%s=%s\"", ava, cp);
}

/*  */

static struct interface *_find_interface_by_name (struct interface *list, const char *ifname)
{
	for (; list; list = list -> ifn_next) {
		if (!strncmp(list -> ifn_interface.ac_if.if_name, ifname, IFNAMSIZ))
			return list;
	}
	return NULL;
}

#ifdef LINUX
static struct address *_upate_addresses (struct interface *list, int *addr_number)
{
    struct ifaddrs *ifaddr, *ifa;
	struct interface *is;
	struct ifnet *ifn;
	struct address *addresses, *addresses_tail, *adrp;

	*addr_number = 0;

	if (getifaddrs (&ifaddr) == -1) {
		advise (LLOG_EXCEPTIONS, "failed", "getifaddrs");
		return NULL;
	}

	for (is = list; is; is = is -> ifn_next) {
		struct ifaddr *ifb, *next;
		ifn = &is -> ifn_interface.ac_if;
		ifn -> if_flags = 0;
		for (ifb = ifn -> if_addrlist; ifb; ifb = next) {
			next = ifb -> ifa_next;
			free ((char *) ifb);
		}
	}

	addresses = addresses_tail = NULL;

	for (ifa = ifaddr; ifa; ifa = ifa->ifa_next) {

		if (ifa->ifa_addr == NULL)
			continue;

		is = _find_interface_by_name (list, ifa -> ifa_name);
		if (is == NULL)
			continue;

		ifn = &is -> ifn_interface.ac_if;
		ifn -> if_flags |= ifa -> ifa_flags;

		if (ifa -> ifa_addr -> sa_family == AF_INET) {
			struct ifaddr *addr, **tail;

			tail = &ifn -> if_addrlist;
			for (; *tail; tail = &(*tail) -> ifa_next) {}

			if ((*tail = calloc (1, sizeof (struct ifaddr))) == NULL)
				adios (NULLCP, "out of memory");
			addr = *tail;

			if ((adrp = calloc (1, sizeof (*addresses))) == NULL)
				adios (NULLCP, "out of memory");

			addr -> ifa_addr = *ifa -> ifa_addr;
			adrp -> adr_address.sa = *ifa -> ifa_addr;

			if (ifa -> ifa_flags & IFF_BROADCAST && ifa -> ifa_broadaddr ->sa_family == AF_INET) {
				addr -> ifa_broadaddr = *ifa -> ifa_broadaddr;
				adrp -> adr_broadaddr.sa = *ifa -> ifa_broadaddr;
			} else if (ifa -> ifa_flags & IFF_POINTOPOINT && ifa -> ifa_broadaddr ->sa_family == AF_INET) {
				addr -> ifa_dstaddr = *ifa -> ifa_dstaddr;
			}
			adrp -> adr_netmask.sa = *ifa -> ifa_netmask;
			adrp -> adr_indexmask |= is -> ifn_indexmask;

			adrp -> adr_insize = ipaddr2oid (adrp -> adr_instance, &adrp -> adr_address.un_in);

			if (addresses_tail) {
				addresses_tail -> adr_next = adrp;
				addresses_tail = adrp;
			} else {
				addresses = addresses_tail = adrp;
			}

			++*addr_number;

		} else if (ifa -> ifa_addr -> sa_family == AF_PACKET) {
			if (ifa->ifa_data != NULL) {
                struct rtnl_link_stats *stats = ifa -> ifa_data;
				ifn -> if_ipackets = stats -> rx_packets;
				ifn -> if_ierrors= stats -> rx_errors;
				ifn -> if_opackets = stats -> tx_packets;
				ifn -> if_oerrors= stats -> tx_errors;
            }
		}
	}
	return addresses;
}
#endif

init_interfaces () {
	int	    i;
	struct ifnet *ifnet;
	OT	    ot;
	struct interface  *is,
			   **ifp;
#ifndef LINUX
	struct nlist nzs;
	struct nlist *nz = &nzs;

	if (getkmem (nl + N_IFNET, (caddr_t) &ifnet, sizeof ifnet) == NOTOK) {
		struct interface *ip;

disabled:
		;
		advise (LLOG_EXCEPTIONS, NULLCP, "interfaces group disabled!");
		for (is = ifs; is; is = ip) {
			ip = is -> ifn_next;

			free ((char *) is);
		}
		ifs = NULL;

		return;
	}

	ifp = &ifs;
	for (i = 0; ifnet; i++) {
		struct ifnet *ifn;

		if ((is = (struct interface *) calloc (1, sizeof *is)) == NULL)
			adios (NULLCP, "out of memory");
		is -> ifn_index = i + 1;
		is -> ifn_indexmask = 1 << i;

		ifn = &is -> ifn_interface.ac_if;

		is -> ifn_offset = (unsigned long) ifnet;

		nz -> n_name = "struct ifnet", nz -> n_value = is -> ifn_offset;
		if (getkmem (nz, (caddr_t) ifn, sizeof is -> ifn_interface) == NOTOK)
			goto disabled;
		ifnet = ifn -> if_next;

		nz -> n_name = "if_name",
			  nz -> n_value = (unsigned long) ifn -> if_name;
		if (getkmem (nz, (caddr_t) is -> ifn_descr, sizeof is -> ifn_descr - 1)
				== NOTOK)
			goto disabled;
		is -> ifn_descr[sizeof is -> ifn_descr - 1] = NULL;
		sprintf (is -> ifn_descr + strlen (is -> ifn_descr), "%d",
				 ifn -> if_unit);

#ifdef	BSD44
		switch (is -> ifn_type = ifn -> if_type) {
		case IFT_ETHER:
		case IFT_P10:
			is -> ifn_speed = 10000000;
			break;

		case IFT_P80:
			is -> ifn_speed = 80000000;
			break;

		default:
			break;
		}
#endif
		if (is -> ifn_type != TYPE_ETHER)
#ifdef	NEW_AT
			bzero ((char *) is -> ifn_interface.ac_enaddr,
				   sizeof is -> ifn_interface.ac_enaddr);
#else
			bzero ((char *) is -> ifn_interface.ac_enaddr.ether_addr_octet,
				   sizeof is -> ifn_interface.ac_enaddr.ether_addr_octet);
#endif

		is -> ifn_admin = ifn -> if_flags & IFF_UP ? OPER_UP : OPER_DOWN;

		*ifp = is, ifp = &is -> ifn_next;

		if (debug)
			advise (LLOG_DEBUG, NULLCP,
					"add interface %d: %s 0x%x",
					is -> ifn_index, is -> ifn_descr, is -> ifn_offset);
	}
#else
	struct ifaddrs *ifaddr, *ifa;
	if (getifaddrs (&ifaddr) == -1) {
		advise (LLOG_EXCEPTIONS, "failed", "getifaddrs");
		return NOTOK;
	}

	ifp = &ifs;
	for (i = 0, ifa = ifaddr; ifa; ifa = ifa->ifa_next) {
		struct ifnet *ifn;

		if (ifa->ifa_addr == NULL)
			continue;

		is = _find_interface_by_name (ifs, ifa -> ifa_name);
		if (is == NULL) {
			int fd;
			struct ifreq ifr;

			if ((is = (struct interface *) calloc (1, sizeof *is)) == NULL)
				adios (NULLCP, "out of memory");
			is -> ifn_index = i + 1;
			is -> ifn_indexmask = 1 << i;
			++i;
			ifn = &is -> ifn_interface.ac_if;
			ifn -> if_name = strdup (ifa -> ifa_name);
			ifn -> if_flags = ifa -> ifa_flags;
			is -> ifn_speed = 10000000;
			snprintf (is -> ifn_descr, sizeof (is -> ifn_descr), "%s", ifa -> ifa_name);

			/* get mtu using ioctl */
			fd = socket (AF_INET, SOCK_DGRAM, 0);
			snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", ifa -> ifa_name);
			if (ioctl (fd, SIOCGIFMTU, &ifr) == 0)
				ifn -> if_mtu = ifr.ifr_mtu;
			close (fd);

			*ifp = is, ifp = &is -> ifn_next;
		}

		ifn = &is -> ifn_interface.ac_if;
		ifn -> if_flags |= ifa -> ifa_flags;

		if (ifa -> ifa_addr -> sa_family == AF_PACKET) {
			struct sockaddr_ll *addr = (struct sockaddr_ll*) ifa -> ifa_addr;
			is -> ifn_type = addr -> sll_hatype == ARPHRD_ETHER ? TYPE_ETHER : TYPE_OTHER;
			memcpy (is -> ifn_interface.ac_enaddr, addr -> sll_addr, 6);
			is -> ifn_offset = addr -> sll_ifindex;
		}
	}
#endif

	if (ot = text2obj ("ifNumber")) {
		ot -> ot_getfnx = o_generic;
		if ((ot -> ot_info = (caddr_t) malloc (sizeof (integer))) == NULL)
			adios (NULLCP, "out of memory");
		*((integer *) ot -> ot_info) = ifNumber = i;
	}

	get_interfaces (type_SNMP_PDUs_get__request);

	if (ot = text2obj ("ifIndex"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifIndex;
	if (ot = text2obj ("ifDescr"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifDescr;
	if (ot = text2obj ("ifType"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifType;
	if (ot = text2obj ("ifMtu"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifMtu;
	if (ot = text2obj ("ifSpeed"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifSpeed;
	if (ot = text2obj ("ifPhysAddress"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifPhysAddress;
	if (ot = text2obj ("ifAdminStatus"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_setfnx = s_interfaces,
					ot -> ot_info = (caddr_t) ifAdminStatus;
	if (ot = text2obj ("ifOperStatus"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifOperStatus;
#ifdef	ifLastChange
	if (ot = text2obj ("ifLastChange"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifLastChange;
#endif
#ifdef	ifInOctets
	if (ot = text2obj ("ifInOctets"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifInOctets;
#endif
	if (ot = text2obj ("ifInUcastPkts"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifInUcastPkts;
#ifdef	ifInNUcastPkts
	if (ot = text2obj ("ifInNUcastPkts"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifInNUcastPkts;
#endif
#ifdef	ifInDiscards
	if (ot = text2obj ("ifInDiscards"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifInDiscards;
#endif
	if (ot = text2obj ("ifInErrors"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifInErrors;
#ifdef	ifInUnknownProtos
	if (ot = text2obj ("ifInUnknownProtos"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifInUnknownProtos;
#endif
#ifdef	ifOutOctets
	if (ot = text2obj ("ifOutOctets"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifOutOctets;
#endif
	if (ot = text2obj ("ifOutUcastPkts"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifOutUcastPkts;
#ifdef	ifOutNUcastPkts
	if (ot = text2obj ("ifOutNUcastPkts"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifOutNUcastPkts;
#endif
	if (ot = text2obj ("ifOutDiscards"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifOutDiscards;
	if (ot = text2obj ("ifOutErrors"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifOutErrors;
#ifdef ifOutQLen
	if (ot = text2obj ("ifOutQLen"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifOutQLen;
#endif
	if (ot = text2obj ("ifSpecific"))
		ot -> ot_getfnx = o_interfaces,
			  ot -> ot_info = (caddr_t) ifSpecific;
}

/*  */

static int  adr_compar (a, b)
struct address **a,
		   **b;
{
	int    i;

	if ((i = (*a) -> adr_address.sa.sa_family
			 - (*b) -> adr_address.sa.sa_family))
		return (i > 0 ? 1 : -1);

	return elem_cmp ((*a) -> adr_instance, (*a) -> adr_insize,
					 (*b) -> adr_instance, (*b) -> adr_insize);
}


int	get_interfaces (offset)
int	offset;
{
	int	    adrNumber = 0;
	struct interface  *is;
	struct address    *as,
			   *ap,
			   **base,
			   **afe,
			   **afp;
	static   int first_time = 1;
	static   int lastq = -1;

	if (quantum == lastq)
		return OK;
	if (!flush_if_cache
			&& offset == type_SNMP_PDUs_get__next__request
			&& quantum == lastq + 1) {			/* XXX: caching! */
		lastq = quantum;
		return OK;
	}
	lastq = quantum, flush_if_cache = 0;

	for (as = afs; as; as = ap) {
		ap = as -> adr_next;

		free ((char *) as);
	}
	afs = afs_inet = NULL;
#ifdef	BSD44
	afs_iso = NULL;
#endif

#ifdef LINUX
	afs = afs_inet = _upate_addresses (ifs, &adrNumber);
#else
	afp = &afs;
	for (is = ifs; is; is = is -> ifn_next) {
		struct arpcom ifns;
		struct ifnet *ifn = &ifns.ac_if;
#ifdef	BSD43
		struct ifaddr ifaddr;
		struct ifaddr *ifa;
#ifdef	BSD44
		union sockaddr_un ifsocka,
				  ifsockb;
#endif
		union sockaddr_un ifsockc;
		union sockaddr_un *ia,
				  *ib;
		union sockaddr_un *ic = &ifsockc;
#endif
#ifndef	BSD44
		struct ifreq ifreq;
#endif
		struct nlist nzs;
		struct nlist *nz = &nzs;

		nz -> n_name = "struct ifnet", nz -> n_value = is -> ifn_offset;
		if (getkmem (nz, (caddr_t) ifn, sizeof ifns) == NOTOK)
			return NOTOK;

#ifndef	BSD43
		if (ifn -> if_addr.sa_family == AF_UNSPEC)
			continue;

		if (nd != NOTOK) {
			strcpy (ifreq.ifr_name, is -> ifn_descr);
			if (ioctl (nd, SIOCGIFNETMASK, (char *) &ifreq) == NOTOK) {
				if (debug)
					advise (LLOG_EXCEPTIONS, "failed", "SIOCGIFNETMASK on %s",
							is -> ifn_descr);
				bzero ((char *) &ifreq, sizeof ifreq);
			}
		} else
			bzero ((char *) &ifreq, sizeof ifreq);
		if (ifn -> if_addr.sa_family == AF_INET) {
			struct sockaddr_in *sx = (struct sockaddr_in *) &ifreq.ifr_addr;
			struct sockaddr_in *sy = (struct sockaddr_in *) &ifn -> if_addr;

			if (sx -> sin_addr.s_addr == 0) {
				if (IN_CLASSA (sy -> sin_addr.s_addr))
					sx -> sin_addr.s_addr = IN_CLASSA_NET;
				else if (IN_CLASSB (sy -> sin_addr.s_addr))
					sx -> sin_addr.s_addr = IN_CLASSB_NET;
				else
					sx -> sin_addr.s_addr = IN_CLASSC_NET;
			}
		}

		if (as = find_address ((union sockaddr_un *) &ifn -> if_addr))
			as -> adr_indexmask |= is -> ifn_indexmask;
		else {
			if ((as = (struct address *) calloc (1, sizeof *as)) == NULL)
				adios (NULLCP, "out of memory");
			*afp = as, afp = &as -> adr_next, adrNumber++;

			as -> adr_address.sa = ifn -> if_addr;	      /* struct copy */
			if (ifn -> if_addr.sa_family == AF_INET)
				as -> adr_broadaddr.sa = ifn -> if_broadaddr; /*   .. */
			as -> adr_netmask.sa = ifreq.ifr_addr;	      /*   .. */
			as -> adr_indexmask = is -> ifn_indexmask;

			switch (ifn -> if_addr.sa_family) {
			case AF_INET:
				as -> adr_insize =
					ipaddr2oid (as -> adr_instance,
								&((struct sockaddr_in *) &ifn -> if_addr)
								-> sin_addr);
				if (afs_inet == NULL)	/* needed for find_address */
					afs_inet = as;
				break;

			default:
				bzero ((char *) as -> adr_instance,
					   sizeof as -> adr_instance);
				as -> adr_insize = 0;
				break;
			}
		}
#else
#ifndef	BSD44
		ia = (union sockaddr_un *) &ifaddr.ifa_addr,
		ib = (union sockaddr_un *) &ifaddr.ifa_broadaddr;
#else
		ia = &ifsocka, ib = &ifsockb;
#endif

		for (ifa = ifn -> if_addrlist; ifa; ifa = ifaddr.ifa_next) {
			nz -> n_name = "struct ifaddr",
				  nz -> n_value = (unsigned long) ifa;
			if (getkmem (nz, (caddr_t) &ifaddr, sizeof ifaddr) == NOTOK)
				continue;
#ifndef	BSD44
			if (ia -> sa.sa_family == AF_UNSPEC)
				continue;

			if (nd != NOTOK) {
				strcpy (ifreq.ifr_name, is -> ifn_descr);
				if (ioctl (nd, SIOCGIFNETMASK, (char *) &ifreq) == NOTOK) {
					if (debug)
						advise (LLOG_EXCEPTIONS, "failed",
								"SIOCGIFNETMASK on %s", is -> ifn_descr);
					bzero ((char *) ic, sizeof *ic);
				}
				ic -> sa = ifreq.ifr_addr;	/* struct copy */
			} else
				bzero ((char *) ic, sizeof *ic);
#else
			nz -> n_name = "union sockaddr_un",
				  nz -> n_value = (unsigned long) ifaddr.ifa_addr;
			if (getkmem (nz, (caddr_t) ia, sizeof *ia) == NOTOK)
				continue;

			if (ia -> sa.sa_family == AF_UNSPEC)
				continue;

			if (ia -> sa.sa_family == AF_INET) {
				nz -> n_value = (unsigned long) ifaddr.ifa_broadaddr;
				if (getkmem (nz, (caddr_t) ib, sizeof *ib) == NOTOK)
					continue;
			}

			nz -> n_value = (unsigned long) ifaddr.ifa_netmask;
			if (getkmem (nz, (caddr_t) ic, sizeof *ic) == NOTOK)
				continue;
#endif
			if (ia -> sa.sa_family == AF_INET
					&& ic -> un_in.sin_addr.s_addr == 0) {
				if (IN_CLASSA (ia -> un_in.sin_addr.s_addr))
					ic -> un_in.sin_addr.s_addr = IN_CLASSA_NET;
				else if (IN_CLASSB (ia -> un_in.sin_addr.s_addr))
					ic -> un_in.sin_addr.s_addr = IN_CLASSB_NET;
				else
					ic -> un_in.sin_addr.s_addr = IN_CLASSC_NET;
			}

			if (as = find_address (ia))
				as -> adr_indexmask |= is -> ifn_indexmask;
			else {
				if ((as = (struct address *) calloc (1, sizeof *as)) == NULL)
					adios (NULLCP, "out of memory");
				*afp = as, afp = &as -> adr_next, adrNumber++;

				as -> adr_address = *ia;		/* struct copy */
				if (ia -> sa.sa_family == AF_INET)
					as -> adr_broadaddr = *ib;		/* struct copy */
				as -> adr_netmask = *ic;		/*   .. */

				as -> adr_indexmask = is -> ifn_indexmask;

				switch (ia -> sa.sa_family) {
				case AF_INET:
					as -> adr_insize =
						ipaddr2oid (as -> adr_instance,
									&ia -> un_in.sin_addr);
					if (afs_inet == NULL)	/* needed for find_address */
						afs_inet = as;
					break;

#ifdef	BSD44
				case AF_ISO:
					as -> adr_insize =
						clnpaddr2oid (as -> adr_instance,
									  &ia -> un_iso.siso_addr);
					if (afs_iso == NULL)	/* needed for find_address */
						afs_iso = as;
					break;
#endif

				default:
					bzero ((char *) as -> adr_instance,
						   sizeof as -> adr_instance);
					as -> adr_insize = 0;
					break;
				}
			}
		}
#endif

		is -> ifn_interface = ifns;	/* struct copy */

		if (is -> ifn_type != TYPE_ETHER)
#ifdef	NEW_AT
			bzero ((char *) is -> ifn_interface.ac_enaddr,
				   sizeof is -> ifn_interface.ac_enaddr);
#else
			bzero ((char *) is -> ifn_interface.ac_enaddr.ether_addr_octet,
				   sizeof is -> ifn_interface.ac_enaddr.ether_addr_octet);
#endif
	}
#endif

	for (is = ifs; is; is = is -> ifn_next) {
		is -> ifn_ready = 0;

		for (as = afs; as; as = as -> adr_next)
			if (as -> adr_indexmask & is -> ifn_indexmask)
				break;
		if (as)
			is -> ifn_ready = 1;
	}

	if (debug && first_time) {
		first_time = 0;

		for (as = afs; as; as = as -> adr_next) {
			OIDentifier	oids;

			oids.oid_elements = as -> adr_instance;
			oids.oid_nelem = as -> adr_insize;
			advise (LLOG_DEBUG, NULLCP,
					"add address: %d/%s with mask 0x%x",
					as -> adr_address.sa.sa_family, sprintoid (&oids),
					as -> adr_indexmask);
		}
	}

	if (adrNumber <= 1)
		return OK;

	if ((base = (struct address **)
				malloc ((unsigned) (adrNumber * sizeof *base))) == NULL)
		adios (NULLCP, "out of memory");

	afe = base;
	for (as = afs; as; as = as -> adr_next)
		*afe++ = as;

	qsort ((char *) base, adrNumber, sizeof *base, adr_compar);

	afp = base;
	as = afs = *afp++;
	afs_inet = NULL;
#ifdef	BSD44
	afs_iso = NULL;
#endif
	while (afp < afe) {
		switch (as -> adr_address.sa.sa_family) {
		case AF_INET:
			if (afs_inet == NULL)
				afs_inet = as;
			break;

#ifdef	BSD44
		case AF_ISO:
			if (afs_iso == NULL)
				afs_iso = as;
			break;
#endif
		}

		as -> adr_next = *afp;
		as = *afp++;
	}
	switch (as -> adr_address.sa.sa_family) {
	case AF_INET:
		if (afs_inet == NULL)
			afs_inet = as;
		break;

#ifdef	BSD44
	case AF_ISO:
		if (afs_iso == NULL)
			afs_iso = as;
		break;
#endif
	}
	as -> adr_next = NULL;

	free ((char *) base);

	return OK;
}

/*  */

static struct address *find_address (addr)
union sockaddr_un *addr;
{
	struct address *as;
	struct in_addr *in;
#ifdef	BSD44
	struct iso_addr *iso;
#endif

	switch (addr -> sa.sa_family) {
	case AF_INET:
		in = &addr -> un_in.sin_addr;
		for (as = afs_inet; as; as = as -> adr_next)
			if (as -> adr_address.sa.sa_family != AF_INET)
				break;
			else if (bcmp ((char *) in,
						   (char *) &as -> adr_address.un_in.sin_addr,
						   sizeof *in) == 0)
				return as;
		break;

#ifdef	BSD44
	case AF_ISO:
		iso = &addr -> un_iso.siso_addr;
		for (as = afs_iso; as; as = as -> adr_next)
			if (as -> adr_address.sa.sa_family != AF_ISO)
				break;
			else if (bcmp ((char *) iso,
						   (char *) &as -> adr_address.un_iso.siso_addr,
						   sizeof *iso) == 0)
				return as;

		break;
#endif

	default:
		break;
	}

	return NULL;
}

/*  */

struct address *get_addrent (ip, len, head, isnext)
unsigned int *ip;
int	len;
struct address *head;
int	isnext;
{
	int	    family;
	struct address *as;

	if (head)
		family = head -> adr_address.sa.sa_family;
	for (as = head; as; as = as -> adr_next)
		if (as -> adr_address.sa.sa_family != family)
			break;
		else
			switch (elem_cmp (as -> adr_instance, as -> adr_insize, ip, len)) {
			case 0:
				if (!isnext)
					return as;
				if ((as = as -> adr_next) == NULL
						|| as -> adr_address.sa.sa_family != family)
					goto out;
			/* else fall... */

			case 1:
				return (isnext ? as : NULL);
			}

out:
	;
	flush_if_cache = 1;

	return NULL;
}
