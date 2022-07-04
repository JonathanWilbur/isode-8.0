/* report.c -- event reporting */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/callback/RCS/report.c,v 9.0 1992/06/16 12:41:50 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/callback/RCS/report.c,v 9.0 1992/06/16 12:41:50 isode Rel $
 *
 *
 * $Log: report.c,v $
 * Revision 9.0  1992/06/16  12:41:50  isode
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
#include <stdarg.h>
#include "general.h"
#include "manifest.h"
#include "logger.h"


static LLog _pgm_log = {
	"callback.log", NULLCP, NULLCP, LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE,
	LLOG_FATAL, -1, LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
static LLog *pgm_log = &_pgm_log;

/*  */

int
reportailor (char *myname) {
	isodetailor (myname, 0);
	ll_hdinit (pgm_log, myname);
}

/*  */

#ifndef	lint
void	adios (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_ll_log (pgm_log, LLOG_FATAL, what, fmt, ap);

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
void	advise (int code, char *what, char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);

    _ll_log (pgm_log, code, what, fmt, ap);

	va_end (ap);
}
#else
/* VARARGS */

void
advise (int code, char *what, char *fmt) {
	advise (code, what, fmt);
}
#endif
