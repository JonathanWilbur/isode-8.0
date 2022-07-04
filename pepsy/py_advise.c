/* py_advise.c - standard "advise" routine for pepsy/pepy */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepsy/RCS/py_advise.c,v 9.0 1992/06/16 12:24:03 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/pepsy/RCS/py_advise.c,v 9.0 1992/06/16 12:24:03 isode Rel $
 *
 *
 * $Log: py_advise.c,v $
 * Revision 9.0  1992/06/16  12:24:03  isode
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

#include <stdio.h>
#include <stdarg.h>

/*  */

#ifndef	lint
char   PY_pepy[BUFSIZ] = "";


void	PY_advise (char* what, char* fmt, ...) {
	va_list	ap;

	va_start (ap, fmt);

	_asprintf (PY_pepy, what, fmt, ap);

	va_end (ap);
}
#else
/* VARARGS */

void
PY_advise (char *what, char *fmt) {
	PY_advise (what, fmt);
}
#endif
