/* susaplose.c - SPM: UNITDATA you lose */

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

#include <stdio.h>
#include <stdarg.h>
#include "spkt.h"
#include "tailor.h"

/*  */

#ifndef	lint
static int _susaplose ();

int	susaplose (struct SSAPindication *si, ...) {
	int	    reason,
		    result;
	va_list ap;

	va_start (ap, si);

	reason = va_arg (ap, int);

	result = _susaplose (si, reason, ap);

	va_end (ap);

	return result;
}
#else
/* VARARGS */

int
susaplose (struct SSAPindication *si, int reason, char *what, char *fmt) {
	return susaplose (si, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_susaplose (	/* what, fmt, args ... */
	struct SSAPindication *si,
	int reason,
	va_list ap
) {
	char  *bp;
	char    buffer[BUFSIZ];
	struct SSAPabort *sa;

	if (si) {
		bzero ((char *) si, sizeof *si);
		si -> si_type = SI_ABORT;
		sa = &si -> si_abort;

		asprintf (bp = buffer, ap);
		bp += strlen (bp);

		sa -> sa_peer = 0;
		sa -> sa_reason = reason;
		copySSAPdata (buffer, bp - buffer, sa);
	}

	return NOTOK;
}
#endif


/*  */

int
ts2suslose (struct SSAPindication *si, char *event, struct TSAPdisconnect *td) {
	int     reason;
	char   *cp,
		   buffer[BUFSIZ];

	if ((ssaplevel & ISODELOG_EXCEPTIONS) && event)
		xsprintf (NULLCP, NULLCP,
				  td -> td_cc > 0 ? "%s: %s\n\t%*.*s": "%s: %s", event,
				  TuErrString (td -> td_reason), td -> td_cc, td -> td_cc,
				  td -> td_data);

	cp = "";
	switch (td -> td_reason) {
	case DR_REMOTE:
	case DR_CONGEST:
		reason = SC_CONGEST;
		break;

	case DR_SESSION:
	case DR_ADDRESS:
		reason = SC_ADDRESS;
		break;

	case DR_REFUSED:
		reason = SC_REFUSED;
		break;

	default:
		sprintf (cp = buffer, " (%s at transport)",
				 TuErrString (td -> td_reason));
	case DR_NETWORK:
		reason = SC_TRANSPORT;
		break;
	}

	return susaplose (si, reason, NULLCP, "%s", *cp ? cp + 1 : cp);
}

