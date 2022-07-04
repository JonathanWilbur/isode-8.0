/* logger.c - system logging routines */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/logger.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/logger.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: logger.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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


/* LINTLIBRARY */

#include <unistd.h>
#define getdtablesize() (sysconf (_SC_OPEN_MAX))
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "general.h"
#include "manifest.h"
#include "logger.h"
#include "tailor.h"

#ifdef	NULL
#undef	NULL
#endif
#include <sys/param.h>
#ifndef	NULL
#define	NULL	0
#endif
#include "sys.file.h"
#include <sys/stat.h>

#ifndef	SYS5
#include <syslog.h>

extern void closelog();
#endif

/*  */

#ifndef	lint
static
#endif
int  _ll_printf (LLog*lp, va_list ap);

struct ll_private {
	int	    ll_checks;
#define	CHKINT	15		/* call ll_check 1 in every 15 uses... */
};

static struct ll_private *llp = NULL;
static IFP _ll_header_routine = ll_defmhdr;

long	lseek ();

/*  */

int
ll_open (LLog *lp) {
	int	    mask,
			mode;
	char   *bp,
		   buffer[BUFSIZ];

	if (llp == NULL
			&& (llp = (struct ll_private *)
					  calloc ((unsigned int) sysconf (_SC_OPEN_MAX),
							  sizeof *llp)) == NULL)
		goto you_lose;

	if (lp -> ll_file == NULLCP
			|| *lp -> ll_file == NULL) {
you_lose:
		;
		ll_close (lp);
		lp -> ll_stat |= LLOGERR;
		return NOTOK;
	}

	lp -> ll_stat &= ~LLOGERR;

	if (lp -> ll_fd != NOTOK)
		return OK;

	if (strcmp (lp -> ll_file, "-") == 0) {
		lp -> ll_stat |= LLOGTTY;
		return OK;
	}

	sprintf (bp = buffer, _isodefile (isodelogpath, lp -> ll_file),
			 getpid ());

	mode = O_WRONLY | O_APPEND;
	if (lp -> ll_stat & LLOGCRT)
		mode |= O_CREAT;

	mask = umask (~0666);
	lp -> ll_fd = open (bp, mode, 0666);
	umask (mask);

	if (ll_check (lp) == NOTOK)
		return (NOTOK);
	if (lp -> ll_fd != NOTOK)
		llp[lp -> ll_fd].ll_checks = CHKINT;

	return (lp -> ll_fd != NOTOK ? OK : NOTOK);
}

/*  */

int
ll_close (LLog *lp) {
	int	    status;

	if (lp -> ll_fd == NOTOK)
		return OK;

	status = close (lp -> ll_fd);
	lp -> ll_fd = NOTOK;

	return status;
}

/*  */

#ifndef	lint
int	ll_log (LLog*lp, ...) {
	int	    event, result;
	char *what, *fmt;
	va_list ap;

	va_start (ap, lp);

	event = va_arg (ap, int);
	what = va_arg (ap, char*);
	fmt = va_arg (ap, char*);

	result = _ll_log (lp, event, what, fmt, ap);

	va_end (ap);

	return result;
}
#else
/* VARARGS4 */

int
ll_log (LLog *lp, int event, char *what, char *fmt) {
	return ll_log (lp, event, what, fmt);
}
#endif

/*  */

int
_ll_log (LLog *lp, int event, char *what, char *fmt, va_list ap) {	/* fmt, args ... */
	int	    cc, status;
	char *bp;
	char buffer[BUFSIZ];

	if (!(lp -> ll_events & event))
		return OK;

	bp = buffer;

	/* Create header */
	(*_ll_header_routine)(bp, lp -> ll_hdr, lp -> ll_dhdr);

	bp += strlen (bp);

	_asprintf (bp, what, fmt, ap);

#ifndef	SYS5
	if (lp -> ll_syslog & event) {
		int	priority;

		switch (event) {
		case LLOG_FATAL:
			priority = LOG_ERR;
			break;

		case LLOG_EXCEPTIONS:
			priority = LOG_WARNING;
			break;

		case LLOG_NOTICE:
			priority = LOG_INFO;
			break;

		case LLOG_PDUS:
		case LLOG_TRACE:
		case LLOG_DEBUG:
			priority = LOG_DEBUG;
			break;

		default:
			priority = LOG_NOTICE;
			break;
		}

		syslog (priority, "%s", buffer + 13);

		if (lp -> ll_stat & LLOGCLS)
			closelog ();
	}
#endif

	if (!(lp -> ll_stat & LLOGTTY)
			&& lp -> ll_fd == NOTOK
			&& strcmp (lp -> ll_file, "-") == 0)
		lp -> ll_stat |= LLOGTTY;

	if (lp -> ll_stat & LLOGTTY) {
		fflush (stdout);

		if (lp -> ll_fd != NOTOK)
			fprintf (stderr, "LOGGING: ");
		fputs (bp, stderr);
		fputc ('\n', stderr);
		fflush (stderr);
	}
	bp += strlen (bp);

	if (lp -> ll_fd == NOTOK) {
		if ((lp -> ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
			return OK;
		if (ll_open (lp) == NOTOK)
			return NOTOK;
	} else if ((!llp || llp[lp -> ll_fd].ll_checks-- < 0)
			   && ll_check (lp) == NOTOK)
		return NOTOK;

	*bp++ = '\n', *bp = NULL;
	cc = bp - buffer;

	if ((status = write (lp -> ll_fd, buffer, cc)) != cc) {
		if (status == NOTOK) {
			ll_close (lp);
error:
			;
			lp -> ll_stat |= LLOGERR;
			return NOTOK;
		}

		status = NOTOK;
	} else
		status = OK;

	if ((lp -> ll_stat & LLOGCLS) && ll_close (lp) == NOTOK)
		goto error;

	return status;
}

/*  */

void
ll_hdinit (LLog *lp, char *prefix) {
	char  *cp,
		  *up;
	char    buffer[BUFSIZ],
			user[10];

	if (prefix == NULLCP) {
		if ((lp -> ll_stat & LLOGHDR) && strlen (lp -> ll_hdr) == 25)
			(cp = lp -> ll_hdr)[8] = NULL;
		else
			cp = "unknown";
	} else {
		if ((cp = rindex (prefix, '/')))
			cp++;
		if (cp == NULL || *cp == NULL)
			cp = prefix;
	}

	if ((up = getenv ("USER")) == NULLCP
			&& (up = getenv ("LOGNAME")) == NULLCP) {
		sprintf (user, "#%d", getuid ());
		up = user;
	}
	sprintf (buffer, "%-8.8s %05d (%-8.8s)",
			 cp, getpid () % 100000, up);

	if (lp -> ll_stat & LLOGHDR)
		free (lp -> ll_hdr);
	lp -> ll_stat &= ~LLOGHDR;

	if ((lp -> ll_hdr = malloc ((unsigned) (strlen (buffer) + 1))) == NULLCP)
		return;

	strcpy (lp -> ll_hdr, buffer);
	lp -> ll_stat |= LLOGHDR;
}

/*  */

void
ll_dbinit (LLog *lp, char *prefix) {
	char  *cp;
	char    buffer[BUFSIZ];

	ll_hdinit (lp, prefix);

	if (prefix) {
		if ((cp = rindex (prefix, '/')))
			cp++;
		if (cp == NULL || *cp == NULL)
			cp = prefix;

		sprintf (buffer, "./%s.log", cp);

		if ((lp -> ll_file = malloc ((unsigned) (strlen (buffer) + 1)))
				== NULLCP)
			return;

		strcpy (lp -> ll_file, buffer);
	}

	lp -> ll_events |= LLOG_ALL;
	lp -> ll_stat |= LLOGTTY;
}

/*  */

#ifndef	lint
int	ll_printf (LLog*lp, ...) {
	int	    result;
	va_list ap;

	va_start (ap, lp);

	result = _ll_printf (lp, ap);

	va_end (ap);

	return result;
}
#else
/* VARARGS2 */

int
ll_printf (LLog *lp, char *fmt) {
	return ll_printf (lp, fmt);
}
#endif

/*  */

#ifndef	lint
static
#endif
int  _ll_printf (LLog*lp, va_list ap) {	/* fmt, args ... */
	int	    cc,
			status;
	char   *bp;
	char     buffer[BUFSIZ];
	char    *fmt;

	fmt = va_arg (ap, char *);
	if (strcmp (fmt, "%s") != 0) {
		bp = buffer;
		_asprintf (bp, NULLCP, fmt, ap);
	} else {
		bp = NULL;
		fmt = va_arg (ap, char *);
	}

	if (!(lp -> ll_stat & LLOGTTY) && lp -> ll_fd == NOTOK && strcmp (lp -> ll_file, "-") == 0)
		lp -> ll_stat |= LLOGTTY;

	if (lp -> ll_stat & LLOGTTY) {
		fflush (stdout);

		if (bp)
			fputs (bp, stderr);
		else
			fputs (fmt, stderr);
		fflush (stderr);
	}
	if (bp)
		bp += strlen (bp);

	if (lp -> ll_fd == NOTOK) {
		if ((lp -> ll_stat & (LLOGERR | LLOGTTY)) == (LLOGERR | LLOGTTY))
			return OK;
		if (ll_open (lp) == NOTOK)
			return NOTOK;
	} else if ((!llp || llp[lp -> ll_fd].ll_checks-- < 0) && ll_check (lp) == NOTOK)
		return NOTOK;

	if (bp)
		cc = bp - buffer;
	else
		cc = strlen (fmt);

	if ((status = write (lp -> ll_fd, bp ? buffer : fmt, cc)) != cc) {
		if (status == NOTOK) {
			ll_close (lp);
			lp -> ll_stat |= LLOGERR;
			return NOTOK;
		}

		status = NOTOK;
	} else
		status = OK;

	va_end(ap);

	return status;
}

/*  */

int
ll_sync (LLog *lp) {
	if (lp -> ll_stat & LLOGCLS)
		return ll_close (lp);

	return OK;
}

/*  */

#ifndef	lint
char   *ll_preset (char* fmt, ...) {
	va_list ap;
	static char buffer[BUFSIZ];

	va_start (ap, fmt);

	_asprintf (buffer, NULLCP, fmt, ap);

	va_end (ap);

	return buffer;
}
#else
/* VARARGS1 */

char *
ll_preset (char *fmt) {
	return ll_preset (fmt);
}
#endif

/*  */

int
ll_check (LLog *lp) {
#ifndef	BSD42
	int	    fd;
	char    buffer[BUFSIZ];
#endif
	long    size;
	struct stat st;

	if ((size = lp -> ll_msize) <= 0)
		return OK;

	if (llp && lp -> ll_fd != NOTOK)
		llp[lp -> ll_fd].ll_checks = CHKINT;
	if (lp -> ll_fd == NOTOK
			|| (fstat (lp -> ll_fd, &st) != NOTOK && st.st_size < (size <<= 10)))
		return OK;

	if (!(lp -> ll_stat & LLOGZER)) {
		ll_close (lp);

#ifndef	BSD42
error:
		;
#endif
		lp -> ll_stat |= LLOGERR;
		return NOTOK;
	}

#ifdef	BSD42
#ifdef	SUNOS4
	ftruncate (lp -> ll_fd, (off_t) 0);
#else
	ftruncate (lp -> ll_fd, 0);
#endif
	lseek (lp -> ll_fd, 0L, 0);
	return OK;
#else
	sprintf (buffer, _isodefile (isodelogpath, lp -> ll_file), getpid ());
	if ((fd = open (buffer, O_WRONLY | O_APPEND | O_TRUNC)) == NOTOK)
		goto error;
	close (fd);
	return OK;
#endif
}

/*  */

/*
 * ll_defmhdr - Default "make header" routine.
 */
int
ll_defmhdr (
	char *bufferp,		/* Buffer pointer */
	char *headerp,		/* Static header string */
	char *dheaderp		/* Dynamic header string */
) {
	time_t    clock;
	struct tm *tm;

	time (&clock);
	tm = localtime (&clock);

	sprintf (bufferp, "%2d/%2d %2d:%02d:%02d %s %s ",
			 tm -> tm_mon + 1, tm -> tm_mday,
			 tm -> tm_hour, tm -> tm_min, tm -> tm_sec,
			 headerp ? headerp : "",
			 dheaderp ? dheaderp : "");
	return OK;
}

/*  */

/*
 * ll_setmhdr - Set "make header" routine, overriding default.
 */
IFP
ll_setmhdr (IFP make_header_routine) {
	IFP result = _ll_header_routine;

	_ll_header_routine = make_header_routine;

	return result;

}


#ifdef ULTRIX_X25
#ifdef ULTRIX_X25_DEMSA

char *
CAT (char *x, char *y) {
	if ( strlen(x)+strlen(y)-2 > BUFSIZ-1 )
		return (char *) y;
	else {

		strcpy(our_global_buffer,x);
		strcat(our_global_buffer,y);

		return (char *) our_global_buffer;
	}
}

#endif
#endif
