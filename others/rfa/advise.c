/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * advise.c - log error messages
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/advise.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: advise.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/advise.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
#endif

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include "manifest.h"
#include "logger.h"

static LLog _pgm_log = {
	"rfa.log", NULLCP, NULLCP,
	LLOG_NOTICE| LLOG_FATAL | LLOG_EXCEPTIONS, LLOG_NOTICE, 100,
	LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *pgm_log = &_pgm_log;


void
initLog (char *myname) {
	/*    if (isatty (fileno (stderr)))
	        ll_dbinit (pgm_log, myname);
	    else */ {

		static char  myfile[BUFSIZ];

		sprintf (myfile, "%s.log", (strncmp (myname, "ros.", 4)
									&& strncmp (myname, "lpp.", 4))
				 || myname[4] == NULL
				 ? myname : myname + 4);
		pgm_log -> ll_file = myfile;
		ll_hdinit (pgm_log, myname);
	}
}


#ifndef	lint
void	adios (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_ll_log (pgm_log, LLOG_FATAL, what, fmt, ap);

	va_end (ap);

	cleanup ();

	_exit (1);
}
#else
/* VARARGS2 */

void
adios (char *what, char *fmt) {
	adios (what, fmt);
}
#endif

#ifndef	lint
void	advise (int code, char *what, char *fmt, ...) {
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



#ifndef	lint
void	ryr_advise (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_ll_log (pgm_log, LLOG_NOTICE, what, fmt, ap);

	va_end (ap);
}
#else
/* VARARGS2 */
void
ryr_advise (char *what, char *fmt) {
	ryr_advise (what, fmt);
}
#endif

