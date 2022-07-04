/*
 ****************************************************************
 *                                                              *
 *  ISODECL  -  HULA project for connectionless ISODE           *
 *                                             			*
 *  module:  	ryresponder.c                                   *
 *  author:   	Bill Haggerty                                   *
 *  date:     	4/89                                            *
 *                                                              *
 *  This code is boilerplate to help write application services.*
 *  It does not call isodeserver.				*
 *                                  				*
 *  entry points:                   				*
 *                                  				*
 *      ryresponder()			                        *
 *	adios(), ros_adios() 					*
 *      advise(), ros_advise(), acs_advise(), ryr_advise()	*
 *                                                              *
 *  internal routines:						*
 *								*
 *	ros_work(), ros_indication(), _advise()			*
 *								*
 ****************************************************************
 *								*
 *			     NOTICE		   		*
 *								*
 *    Use of this module is subject to the restrictions of the  *
 *    ISODE license agreement.					*
 *								*
 ****************************************************************
*/
/* HULA */

#include <errno.h>
#include <ctype.h>
/* ryresponder.c - generic idempotent responder */
/* modified for UNIT-DATA protocol stack */


#include <stdio.h>
#include <setjmp.h>
#include <stdarg.h>
#include "ryresponder.h"

#define ACSE
#include  "acupkt.h"

/*  #include "tsap.h"  for listening */

/*    DATA */

int	debug = 0;

static char *myname = "ryresponder";


static jmp_buf toplevel;


static IFP	startfnx;
static IFP	stopfnx;

int	ros_init (), ros_work (), ros_indication (), ros_lose ();




/*    RESPONDER */

/* HULA changed call */
/* ryresponder (argc, argv, host, myservice, dispatches, ops, start, stop) */

ryresponder (argc, argv, host, myservice, mycontext, mypci,
			 dispatches, ops, start, stop)
int	argc;
char  **argv,
	  *host,
	  *myservice,
	  *mycontext,
	  *mypci;
struct dispatch *dispatches;
struct RyOperation *ops;
IFP	start,
	stop;
{
	struct dispatch   *ds;
	AEI	    aei;
	/*
	    struct TSAPdisconnect   tds;
	    struct TSAPdisconnect  *td = &tds;
	*/
	struct RoSAPindication  rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject   *rop = &roi -> roi_preject;
	struct PSAPaddr *pa;

	/* HULA added for connectionless ******************/

	int	  sd;
	static    struct QOStype qos;
	static    OID	    ctx;
	static    OID	    pci;
	static    struct PSAPctxlist pcs;
	static    struct PSAPctxlist *pc = &pcs;

	static    struct AcSAPindication  acis;
	static    struct AcSAPindication *aci = &acis;
	static    struct AcSAPabort *aca = &acis.aci_abort;

	static    struct  AcuSAPstart	cacs, sacs;
	static    struct  AcuSAPstart	*pcacs = &cacs;
	static    struct  AcuSAPstart	*psacs = &sacs;

	/* end of HULA added stuff */

	if (myname = rindex (argv[0], '/'))
		myname++;
	if (myname == NULL || *myname == NULL)
		myname = argv[0];

	if (debug = isatty (fileno (stderr)))
		setlog ("service.out");

	openlog (myname, LOG_PID);

	advise (LOG_INFO, NULLCP, "starting");

	if ((aei = str2aei (host, myservice)) == NULLAEI)
		adios (NULLCP, "%s-%s: unknown application-entity", host, myservice);

	for (ds = dispatches; ds -> ds_name; ds++)
		if (RyDispatch (NOTOK, ops, ds -> ds_operation, ds -> ds_vector, roi)
				== NOTOK) {
			ros_advise (rop, ds -> ds_name);
			exit (1);
		}

	/********** HULA inserted for connectionless *************/

	if ((pa = aei2addr (aei)) == NULLPA)
		adios (NULLCP, "%s-%s: address unknown", host, myservice);
	if ((ctx = ode2oid (mycontext)) == NULLOID)
		adios (NULLCP, "%s: unknown object descriptor", mycontext);
	if ((ctx = oid_cpy (ctx)) == NULLOID)
		adios (NULLCP, "out of memory");
	if ((pci = ode2oid (mypci)) == NULLOID)
		adios (NULLCP, "%s: unknown object descriptor", mypci);
	if ((pci = oid_cpy (pci)) == NULLOID)
		adios (NULLCP, "out of memory");
	pc -> pc_nctx = 1;
	pc -> pc_ctx[0].pc_id = 1;
	pc -> pc_ctx[0].pc_asn = pci;
	pc -> pc_ctx[0].pc_atn = NULLOID;

	if ((sd = AcUnitDataBind (NOTOK, BIND_DYNAMIC, ctx, aei, NULLAEI,
							  pa, NULLPA, pc, qos, aci) ) == NOTOK ) {
		acs_advise (aca, "A-UNIT-DATA BIND");
		return NOTOK;
	}


	/*  dynamic invocation by tsap daemon must save tpdu for later read */
	if ( argc > 1 )
		if ( AcuSave ( sd, argc, argv, aci ) == NOTOK ) {
			acs_advise (aca, "A-UNIT-DATA SAVE");
			return NOTOK;
		}


	if (RoSetService (sd, RoAcuService, roi) == NOTOK)
		ros_adios (rop, "set RO/Acu fails");

	for (;;)
		ros_work (sd);

	/* end of HULA inserted */


	startfnx = start;
	stopfnx = stop;
	/*
	************* removed for connectionless *************

	    if (isodeserver (argc, argv, aei, ros_init, ros_work, ros_lose, td)
		    == NOTOK) {
		if (td -> td_cc > 0)
		    adios (NULLCP, "isodeserver: [%s] %*.*s",
			    TErrString (td -> td_reason),
			    td -> td_cc, td -> td_cc, td -> td_data);
		else
		    adios (NULLCP, "isodeserver: [%s]",
			    TErrString (td -> td_reason));
	    }
	*****************************************************
	*/

	exit (0);
}

/*  */

/*
****************** removed for connectionless *************

static int  ros_init (vecp, vec)
int	vecp;
char  **vec;
{
    int	    reply,
	    result,
	    sd;
    struct AcSAPstart   acss;
    struct AcSAPstart *acs = &acss;
    struct AcSAPindication  acis;
    struct AcSAPindication *aci = &acis;
    struct AcSAPabort   *aca = &aci -> aci_abort;
    struct PSAPstart *ps = &acs -> acs_start;
    struct RoSAPindication  rois;
    struct RoSAPindication *roi = &rois;
    struct RoSAPpreject   *rop = &roi -> roi_preject;

    if (AcInit (vecp, vec, acs, aci) == NOTOK) {
	acs_advise (aca, "initialization fails");
	return NOTOK;
    }
    advise (LOG_INFO, NULLCP,
		"A-ASSOCIATE.INDICATION: <%d, %s, %s, %s, %d>",
		acs -> acs_sd, sprintoid (acs -> acs_context),
		sprintaei (&acs -> acs_callingtitle),
		sprintaei (&acs -> acs_calledtitle), acs -> acs_ninfo);

    sd = acs -> acs_sd;

    for (vec++; *vec; vec++)
	advise (LOG_INFO, NULLCP, "unknown argument \"%s\"", *vec);

    reply = startfnx ? (*startfnx) (sd, acs) : ACS_ACCEPT;

    result = AcAssocResponse (sd, reply,
		reply != ACS_ACCEPT ? ACS_USER_NOREASON : ACS_USER_NULL,
		NULLOID, NULLAEI, NULLPA, NULLPC, ps -> ps_defctxresult,
		ps -> ps_prequirements, ps -> ps_srequirements, SERIAL_NONE,
		ps -> ps_settings, &ps -> ps_connect, NULLPEP, 0, aci);

    ACSFREE (acs);

    if (result == NOTOK) {
	acs_advise (aca, "A-ASSOCIATE.RESPONSE");
	return NOTOK;
    }
    if (reply != ACS_ACCEPT)
	return NOTOK;

    if (RoSetService (sd, RoPService, roi) == NOTOK)
	ros_adios (rop, "set RO/PS fails");

    return sd;
}
*****************************************************
*/

/*  */

static int
ros_work (int fd) {
	int	    result;
	caddr_t out;
	struct AcSAPindication  acis;
	struct RoSAPindication  rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject   *rop = &roi -> roi_preject;

	switch (setjmp (toplevel)) {
	case OK:
		break;

	default:
		if (stopfnx)
			(*stopfnx) (fd, (struct AcSAPfinish *) 0);
	case DONE:
		/* HULA	     AcUAbortRequest (fd, NULLPEP, 0, &acis); */
		RyLose (fd, roi);
		return NOTOK;
	}

	switch (result = RyWait (fd, NULLIP, &out, OK, roi)) {
	case NOTOK:
		if (rop -> rop_reason == ROS_TIMER)
			break;
	case OK:
	case DONE:
		ros_indication (fd, roi);
		break;

	default:
		adios (NULLCP, "unknown return from RoWaitRequest=%d", result);
	}

	return OK;
}

/*  */

static int
ros_indication (int sd, struct RoSAPindication *roi) {
	int	    reply,
			result;

	switch (roi -> roi_type) {
	case ROI_INVOKE:
	case ROI_RESULT:
	case ROI_ERROR:
		adios (NULLCP, "unexpected indication type=%d", roi -> roi_type);
		break;

	case ROI_UREJECT: {
		struct RoSAPureject   *rou = &roi -> roi_ureject;

		if (rou -> rou_noid)
			advise (LOG_INFO, NULLCP, "RO-REJECT-U.INDICATION/%d: %s",
					sd, RoErrString (rou -> rou_reason));
		else
			advise (LOG_INFO, NULLCP,
					"RO-REJECT-U.INDICATION/%d: %s (id=%d)",
					sd, RoErrString (rou -> rou_reason),
					rou -> rou_id);
	}
	break;

	case ROI_PREJECT: {
		struct RoSAPpreject   *rop = &roi -> roi_preject;

		if (ROS_FATAL (rop -> rop_reason))
			ros_adios (rop, "RO-REJECT-P.INDICATION");
		ros_advise (rop, "RO-REJECT-P.INDICATION");
	}
	break;
	/*
		case ROI_FINISH:
		    {
			struct AcSAPfinish *acf = &roi -> roi_finish;
			struct AcSAPindication  acis;
			struct AcSAPabort *aca = &acis.aci_abort;

			advise (LOG_INFO, NULLCP, "A-RELEASE.INDICATION/%d: %d",
				sd, acf -> acf_reason);

			reply = stopfnx ? (*stopfnx) (sd, acf) : ACS_ACCEPT;

			result = AcRelResponse (sd, reply, ACR_NORMAL, NULLPEP, 0,
				    &acis);

			ACFFREE (acf);

			if (result == NOTOK)
			    acs_advise (aca, "A-RELEASE.RESPONSE");
			else
			    if (reply != ACS_ACCEPT)
				break;
			longjmp (toplevel, DONE);
		    }
	*/
	/* NOTREACHED */

	default:
		adios (NULLCP, "unknown indication type=%d", roi -> roi_type);
	}
}

/* HULA removed for connectionless **********************/
/*  */

/*
static int  ros_lose (td)
struct TSAPdisconnect *td;
{
    if (td -> td_cc > 0)
	advise (LOG_INFO, NULLCP, "TNetAccept: [%s] %*.*s",
		TErrString (td -> td_reason), td -> td_cc, td -> td_cc,
		td -> td_data);
    else
	advise (LOG_INFO, NULLCP, "TNetAccept: [%s]",
		TErrString (td -> td_reason));
}
*/

/*    ERRORS */

void
ros_adios (struct RoSAPpreject *rop, char *event) {
	ros_advise (rop, event);

	longjmp (toplevel, NOTOK);
}


void
ros_advise (struct RoSAPpreject *rop, char *event) {
	char    buffer[BUFSIZ];

	if (rop -> rop_cc > 0)
		sprintf (buffer, "[%s] %*.*s", RoErrString (rop -> rop_reason),
				 rop -> rop_cc, rop -> rop_cc, rop -> rop_data);
	else
		sprintf (buffer, "[%s]", RoErrString (rop -> rop_reason));

	advise (LOG_INFO, NULLCP, "%s: %s", event, buffer);
}

/*  */

void
acs_advise (struct AcSAPabort *aca, char *event) {
	char    buffer[BUFSIZ];

	if (aca -> aca_cc > 0)
		sprintf (buffer, "[%s] %*.*s",
				 AcuErrString (aca -> aca_reason),
				 aca -> aca_cc, aca -> aca_cc, aca -> aca_data);
	else
		sprintf (buffer, "[%s]", AcErrString (aca -> aca_reason));

	advise (LOG_INFO, NULLCP, "%s: %s (source %d)", event, buffer,
			aca -> aca_source);
}

/*  */

#ifndef	lint
static void	_advise ();


void	adios (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_advise (LOG_ERR, what, fmt, ap);

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
void	advise (int code, char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_advise (code, what, fmt, ap);

	va_end (ap);
}


static void  _advise (int code, char *what, va_list ap) {
	char    buffer[BUFSIZ];

	_asprintf (buffer, what, fmt, ap);

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
advise (int code, char *what, char *fmt) {
	advise (code, what, , fmt);
}
#endif


#ifndef	lint
void	ryr_advise (char *what, char *fmt, ...) {
	va_list ap;

	va_start (ap, fmt);

	_advise (LOG_INFO, what, fmt, ap);

	va_end (ap);
}
#else
/* VARARGS */

void
ryr_advise (char *what, char *fmt) {
	ryr_advise (what, fmt);
}
#endif
