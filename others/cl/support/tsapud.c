/* tsapud.c - ISO UNITDATA transport server */

/*
 ****************************************************************
 *                                                              *
 *  HULA project						*
 *                                             			*
 *  program:  	tsapud.c	                                *
 *  author:   	kurt dobbins                                    *
 *  date:     	5/89                                            *
 *                                                              *
 *  This program implements the connectionless tsap unitdata    *
 *  daemon.  It is designed to provide dynamic service          *
 *  invocation for unitdata services.  Note that because of the *
 *  way DATAGRAM sockets work (we can't accect on them and get  *
 *  a new sd), a new socket is created for each unitdata        *
 *  indication and a new service is spawned each time as well.  *
 *  Performance-minded services really should be run statically.*
 *								*
 ****************************************************************
 *								*
 *								*
 *			     NOTICE		   		*
 *								*
 *    Acquisition, use, and distribution of this module and     *
 *    related materials are subject to the restrictions of a    *
 *    license agreement.					*
 *								*
 *    This software is for prototype purposes only.		*
 *								*
 ****************************************************************
 */


#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <varargs.h>
#include "manifest.h"
#include <sys/ioctl.h>
#include <sys/stat.h>
#ifdef	BSD42
#include <sys/file.h>
#endif
#ifdef	SYS5
#include <fcntl.h>
#endif
#ifndef	X_OK
#define	X_OK	1
#endif

#ifndef	NOGOSIP
#include "rosap.h"
#include "rtsap.h"
#include "psap2.h"
#include "ssap.h"
#endif
#include "tpkt.h"


#ifdef	TCP
#include "internet.h"
#endif
#ifdef	X25
#include "x25.h"
#endif
#ifdef	CONS
#include "cons.h"
#endif
#ifdef	TP4
#include "tp4.h"
#endif
#include "isoservent.h"
#include "logger.h"
#include "tailor.h"
#include "tusap.h"


/*  */

static int  debug = 1;
static int  nbits = FD_SETSIZE;

static char *myname = "tsapud";
static char myhost[BUFSIZ];

static int	udpservice = 1;
static int	tcpservice = 0;
static int	x25service = 0;
static int	consservice = 0;
static int	bridgeservice = 0;
static int	tp4service = 0;


#define	NTADDRS		FD_SETSIZE

static struct TSAPaddr *tz;
static struct TSAPaddr  tas[NTADDRS];


void	adios (), advise ();
void	ts_advise ();





/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
	  **envp;
{
	int	    listen,
			vecp;
	int     sd;
	char   *vec[4];
	struct NSAPaddr  *na;
	struct TSAPaddr  *ta;
	struct TSAPunitdata       tuds;
	struct TSAPunitdata *tud = &tuds;
	struct TSAPdisconnect   tds;
	struct TSAPdisconnect  *td = &tds;
	int	    pid;
	char buffer1[BUFSIZ];



	arginit (argv);
	envinit ();

#ifdef CAMTEC
	if (x25service && (tcpservice || bridgeservice || tp4service))
		adios (NULLCP, "CAMTEC X.25 daemon must run stand-alone");
#endif

	listen = 0;

	for (ta = tas; ta < tz; ta++) {
		if (ta -> ta_naddr == 0) {
			if (!tp4service)
				continue;

			advise (NULLCP, LOG_INFO, "listening on TSEL %s",
					sel2str (ta -> ta_selector, ta -> ta_selectlen, 1));
		} else {
			na = ta -> ta_addrs;

			switch (na -> na_type) {
			case NA_TCP:
				if (!udpservice)
					continue;
				advise (NULLCP, LOG_INFO, "listening on UDP %s %d",
						na2str (na), ntohs (na -> na_port));
				break;

			default:
				adios (NULLCP, "unknown network type 0x%x", na -> na_type);
				/* NOTREACHED */
			}

			if (ta -> ta_selectlen)
				advise (NULLCP, LOG_INFO, "   %s",
						sel2str (ta -> ta_selector, ta -> ta_selectlen, 1));
		}

		/*  set up a listen socket for unitdata service */

		if (( sd = TUnitDataBind ( NOTOK, ta, NULL, NULL, td)) == NOTOK) {
			ts_advise (td, LOG_ERR, "UNITDATA listen failed");
			_exit (1);
		}

		listen++;
	}

	if (!listen)
		adios (NULLCP, "no network services selected");

	/*  keep reading until we get a datagram */

	for (;;) {
		if (TUnitDataRead (sd, tud, NOTOK) == NOTOK) {
			ts_advise (td, LOG_ERR, "UNITDATA read failed");
			continue;
		}

		/*
		 *  Setup the vec pointers to pass.
		 */

		vec[0] = "tusap";
		vec[1] = sd;
		vec[2] = tud;
		vec[3] = NULLCP;

		vecp = 3;

		/*
		     *  Fork a new process for this unitdata instance.
		     */

		switch (pid = fork ()) {
		case OK:
			break;

		default:
			tusaplose (td, DR_CONGEST, "unitddata",
					   "unable to fork ");

		}
	}
}

/*  */

static char buffer1[4096];
static char buffer2[32768];


static int
tsapud (int vecp, char **vec) {
	char    buffer[BUFSIZ];
	struct isoservent *is;
	struct tsapblk *tb;
	struct TSAPunitdata   tuds;
	struct TSAPunitdata *tud = &tuds;
	struct TSAPdisconnect   tds;
	struct TSAPdisconnect  *td = &tds;
	IFP	    hook;

	/* begin UGLY */
	strcpy (buffer1, vec[1]);
	strcpy (buffer2, vec[2]);
	/* end UGLY */

	/*  save the datagram in the transport layer for the 1st read */

	if (TuSave (vecp, vec, tud, td) == NOTOK) {
		ts_advise (td, LOG_ERR, "T-UNITDATA.SAVE");
		return;
	}

	advise (NULLCP, LOG_INFO,
			"T-UNITDATA.INDICATION: <%d, <%s, %s>, <%s, %s>",
			tud -> tud_sd,
			na2str (tud -> tud_calling.ta_addrs),
			sel2str (tud -> tud_calling.ta_selector,
					 tud -> tud_calling.ta_selectlen, 1),
			na2str (tud -> tud_called.ta_addrs),
			sel2str (tud -> tud_called.ta_selector,
					 tud -> tud_called.ta_selectlen, 1) );

	if (tud -> tud_called.ta_selectlen) {
		if ((is = getisoserventbyselector ("tsap", tud -> tud_called.ta_selector,
										   tud -> tud_called.ta_selectlen))
				== NULL) {
			sprintf (buffer, "ISO service tsap/%s not found",
					 sel2str (tud -> tud_called.ta_selector,
							  tud -> tud_called.ta_selectlen, 1));
			goto out;
		}
	}

	*is -> is_tail++ = buffer1;
	*is -> is_tail++ = buffer2;
	*is -> is_tail = NULL;

	if (tb = findtublk (tud -> tud_sd))
		tb -> tb_fd = NOTOK;

	/*
	 *  Now exec the service process.
	 */

	execv (*is -> is_vec, is -> is_vec);

	sprintf (buffer, "unable to exec %s: %s",
			 *is -> is_vec, sys_errname (errno));

out:
	;
	advise (NULLCP, LOG_ERR, "%s", buffer);
	if (strlen (buffer) >= TD_SIZE)
		buffer[0] = NULL;

	exit (1);
}

/*  */

static void
ts_advise (struct TSAPdisconnect *td, int code, char *event) {
	char    buffer[BUFSIZ];

	if (td -> td_cc > 0)
		sprintf (buffer, "[%s] %*.*s",
				 TuErrString (td -> td_reason),
				 td -> td_cc, td -> td_cc, td -> td_data);
	else
		sprintf (buffer, "[%s]", TuErrString (td -> td_reason));

	advise (NULLCP, code, "%s: %s", event, buffer);
}


/*  */

static
arginit (char **vec) {
	int	    rflag;
	char  *ap;
#ifdef	TCP
	int	    port;
	struct NSAPaddr *tcp_na;
	struct servent *sp;
#endif
#ifdef	X25
	struct NSAPaddr *x25_na;
#endif
#ifdef        CONS
	struct NSAPaddr *cons_na;
#endif
#ifdef	BRIDGE_X25
	struct NSAPaddr *bridge_na;
#endif
#ifdef	TP4
	struct isoservent *is;
#endif
	struct stat st;

	if (myname = rindex (*vec, '/'))
		myname++;
	if (myname == NULL || *myname == NULL)
		myname = *vec;

	openlog (myname, LOG_PID);

	isodetailor (myname);

	rflag = 0;

	strcpy (myhost, TLocalHostName ());

	bzero ((char *) tas, sizeof tas);

	tz = tas;

#ifdef	TCP
	if ((sp = getservbyname ("tsap", "tcp")) == NULL)
		adios (NULLCP, "tcp/tsap: unknown service");

	tcp_na = tz -> ta_addrs;
	tcp_na -> na_type = NA_TCP;
	tcp_na -> na_domain[0] = NULL;
	tcp_na -> na_port = sp -> s_port;
	tz -> ta_naddr = 1;

	tz++;
#endif


	for (vec++; ap = *vec; vec++) {
		if (*ap == '-')
			switch (*++ap) {
			case 't':
				udpservice = 1;
				tcpservice = x25service = consservice = bridgeservice = tp4service = 0;
				continue;

			case 'r':
				rflag = 1;
				continue;

#ifdef	TCP
			case 'p':
				if ((ap = *++vec) == NULL
						|| *ap == '-'
						|| (port = atoi (ap)) <= 0)
					adios (NULLCP, "usage: %s -p portno", myname);
				tcp_na -> na_port = htons ((u_short) port);
				continue;
#endif


			default:
				adios (NULLCP, "-%s: unknown switch", ap);
			}

		adios (NULLCP, "usage: %s [switches]", myname);
	}

	if (!rflag
			&& getuid () == 0
			&& stat (ap = isodefile ("isoservices"), &st) != NOTOK
			&& st.st_uid != 0)
		adios (NULLCP, "%s not owned by root", ap);
}

/*  */

static
envinit () {
	int     i,
	sd;

	nbits = getdtablesize ();

	if (!(debug = isatty (2))) {
		for (i = 0; i < 5; i++) {
			switch (fork ()) {
			case NOTOK:
				sleep (5);
				continue;

			case OK:
				break;

			default:
				_exit (0);
			}
			break;
		}

		chdir ("/");

		if ((sd = open ("/dev/null", O_RDWR)) == NOTOK)
			adios ("/dev/null", "unable to read");
		if (sd != 0)
			dup2 (sd, 0),  close (sd);
		dup2 (0, 1);
		dup2 (0, 2);

#ifdef	TIOCNOTTY
		if ((sd = open ("/dev/tty", O_RDWR)) != NOTOK) {
			ioctl (sd, TIOCNOTTY, NULLCP);
			close (sd);
		}
#else
#ifdef	SYS5
		setpgrp ();
		signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
#endif
#endif
	} else
		setlog ("tsapud.out");

	for (sd = 3; sd < nbits; sd++)
		close (sd);

	signal (SIGPIPE, SIG_IGN);

	closelog ();
	openlog (myname, LOG_PID);

	advise (NULLCP, LOG_INFO, "starting");
}

/*    ERRORS */

#ifndef	lint
void	_advise ();


void	adios (va_alist)
va_dcl {
	char   *what;
	va_list ap;

	va_start (ap);

	what = va_arg (ap, char *);

	_advise (LOG_ERR, what, ap);

	va_end (ap);

	_exit (1);
}
#else
/* VARARGS */

void
adios (char *what, char *fmt) {
	adios (what, fmt);
}
#endif


#ifndef	lint
void	advise (va_alist)
va_dcl {
	int	    code;
	char   *what;
	va_list ap;

	va_start (ap);

	what = va_arg (ap, char *);
	code = va_arg (ap, int);

	_advise (code, what, ap);

	va_end (ap);
}


static void
_advise (int code, char *what, va_list ap) {
	char    buffer[BUFSIZ];

	_asprintf (buffer, what, ap);

	syslog (code, "%s", buffer);

	if (debug) {
		fflush (stdout);

		fprintf (stderr, "[%d] %s", code, buffer);
		fputc ('\n', stderr);
		fflush (stderr);
	}
}
#else
/* VARARGS */

void
advise (char *what, int code, char *fmt) {
	advise (what, code, fmt);
}
#endif
