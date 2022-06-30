// if.h

struct ifnet {
	char	*if_name;		/* name, e.g. ``en'' or ``lo'' */
	struct	ifnet *if_next;		/* all struct ifnets are chained */
	struct	ifaddr *if_addrlist;	/* linked list of addresses per if */
        int	if_pcount;		/* number of promiscuous listeners */
	caddr_t	if_bpf;			/* packet filter structure */
	u_short	if_index;		/* numeric abbreviation for this if  */
	short	if_unit;		/* sub-unit for lower level driver */
	short	if_timer;		/* time 'til if_watchdog called */
	short	if_flags;		/* up/down, broadcast, etc. */
	struct	if_data {
/* generic interface information */
		u_char	ifi_type;	/* ethernet, tokenring, etc */
		u_char	ifi_addrlen;	/* media address length */
		u_char	ifi_hdrlen;	/* media header length */
		u_long	ifi_mtu;	/* maximum transmission unit */
		u_long	ifi_metric;	/* routing metric (external only) */
		u_long	ifi_baudrate;	/* linespeed */
/* volatile statistics */
		u_long	ifi_ipackets;	/* packets received on interface */
		u_long	ifi_ierrors;	/* input errors on interface */
		u_long	ifi_opackets;	/* packets sent on interface */
		u_long	ifi_oerrors;	/* output errors on interface */
		u_long	ifi_collisions;	/* collisions on csma interfaces */
		u_long	ifi_ibytes;	/* total number of octets received */
		u_long	ifi_obytes;	/* total number of octets sent */
		u_long	ifi_imcasts;	/* packets received via multicast */
		u_long	ifi_omcasts;	/* packets sent via multicast */
		u_long	ifi_iqdrops;	/* dropped on input, this interface */
		u_long	ifi_noproto;	/* destined for unsupported protocol */
		struct	timeval ifi_lastchange;/* last updated */
	}	if_data;
/* procedure handles */
	int	(*if_init)		/* init routine */
		__P((int));
	int	(*if_output)		/* output routine (enqueue) */
		__P((struct ifnet *, struct mbuf *, struct sockaddr *,
		     struct rtentry *));
	int	(*if_start)		/* initiate output routine */
		__P((struct ifnet *));
	int	(*if_done)		/* output complete routine */
		__P((struct ifnet *));	/* (XXX not used; fake prototype) */
	int	(*if_ioctl)		/* ioctl routine */
		__P((struct ifnet *, int, caddr_t));
	int	(*if_reset)	
		__P((int));		/* new autoconfig will permit removal */
	int	(*if_watchdog)		/* timer routine */
		__P((int));
	struct	ifqueue {
		struct	mbuf *ifq_head;
		struct	mbuf *ifq_tail;
		int	ifq_len;
		int	ifq_maxlen;
		int	ifq_drops;
	} if_snd;			/* output queue */
};
#define	if_mtu		if_data.ifi_mtu
#define	if_type		if_data.ifi_type
#define	if_addrlen	if_data.ifi_addrlen
#define	if_hdrlen	if_data.ifi_hdrlen
#define	if_metric	if_data.ifi_metric
#define	if_baudrate	if_data.ifi_baudrate
#define	if_ipackets	if_data.ifi_ipackets
#define	if_ierrors	if_data.ifi_ierrors
#define	if_opackets	if_data.ifi_opackets
#define	if_oerrors	if_data.ifi_oerrors
#define	if_collisions	if_data.ifi_collisions
#define	if_ibytes	if_data.ifi_ibytes
#define	if_obytes	if_data.ifi_obytes
#define	if_imcasts	if_data.ifi_imcasts
#define	if_omcasts	if_data.ifi_omcasts
#define	if_iqdrops	if_data.ifi_iqdrops
#define	if_noproto	if_data.ifi_noproto
#define	if_lastchange	if_data.ifi_lastchange
