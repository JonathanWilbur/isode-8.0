/* ryopinvoke.c - ROSY: invoke */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosy/RCS/ryopinvoke.c,v 9.0 1992/06/16 12:37:29 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/rosy/RCS/ryopinvoke.c,v 9.0 1992/06/16 12:37:29 isode Rel $
 *
 *
 * $Log: ryopinvoke.c,v $
 * Revision 9.0  1992/06/16  12:37:29  isode
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
#include "rosy.h"

#ifdef __STDC__
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", #p); \
}
#else
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", "p"); \
}
#endif

/*    INVOKE */

int	RyOpInvoke (sd, ryo, op, in, out, rfx, efx, class, invokeID, linkedID,
				priority, roi)
int	sd;
struct RyOperation *ryo;
int	op,
	class,
	invokeID,
	*linkedID,
	priority;
caddr_t	in,
		*out;
IFP	rfx,
	efx;
struct RoSAPindication *roi;
{
	int	    result;
	PE	    pe;
	struct opsblk *opb;

	missingP (ryo);
	missingP (roi);

	if (opb = findopblk (sd, invokeID, OPB_INITIATOR))
		return rosaplose (roi, ROS_IP_DUP, NULLCP, NULLCP);

	for (; ryo -> ryo_name; ryo++)
		if (ryo -> ryo_op == op)
			break;
	if (!ryo -> ryo_name)
		return rosaplose (roi, ROS_PARAMETER, NULLCP,
						  "unknown operation code %d", op);

#ifdef PEPSY_DEFINITIONS
	if (ryo -> ryo_arg_mod) {
#else
	if (ryo -> ryo_arg_encode) {
#endif
#ifdef	notdef
		missingP (in);
#endif
		PY_pepy[0] = 0;
#ifdef PEPSY_DEFINITIONS
		if (enc_f (ryo -> ryo_arg_index, ryo -> ryo_arg_mod, &pe, 1, NULL,
				   NULLCP, in) == NOTOK)
#else
		if ((*ryo -> ryo_arg_encode) (&pe, 1, NULL, NULLCP, in) == NOTOK)
#endif
			return rosaplose (roi, ROS_CONGEST, NULLCP,
							  "error encoding argument for invocation %d operation %s/%d [%s]",
							  invokeID, ryo -> ryo_name, ryo -> ryo_op, PY_pepy);
	} else {
		if (in)
			return rosaplose (roi, ROS_PARAMETER, NULLCP,
							  "argument not permitted with operation %s/%d",
							  ryo -> ryo_name, ryo -> ryo_op);

		pe = NULLPE;
	}

	if (ryo -> ryo_result || ryo -> ryo_errors) {
		if (out) {
			if (rfx || efx)
				return rosaplose (roi, ROS_PARAMETER, NULLCP,
								  "out is exclusive with rfx/efx parameters");
		} else {
			if (ryo -> ryo_result) {
				missingP (rfx);
			}
			missingP (efx);
		}

		if ((opb = newopblk (sd, invokeID)) == NULLOPB) {
			if (pe)
				pe_free (pe);
			return rosaplose (roi, ROS_CONGEST, NULLCP, NULLCP);
		}

		opb -> opb_ryo = ryo;
		opb -> opb_resfnx = rfx, opb -> opb_errfnx = efx;
	} else {
		if (class != ROS_ASYNC) {
			if (pe)
				pe_free (pe);
			return rosaplose (roi, ROS_PARAMETER, NULLCP,
							  "ASYNC class must be used with operation %s/%d",
							  ryo -> ryo_name, ryo -> ryo_op);
		}

		opb = NULLOPB;
	}

	result = RoInvokeRequest (sd, ryo -> ryo_op, class, pe, invokeID, linkedID,
							  priority, roi);

	if (pe)
		pe_free (pe);

	switch (result) {
	case NOTOK:
	case DONE:
		break;

	case OK:
		if (class == ROS_ASYNC)
			return OK;
		return RyWaitAux (sd, opb, out, NOTOK, roi);

	default:
		result = rosaplose (roi, ROS_PROTOCOL, NULLCP,
							"unknown return from RoInvokeRequest=%d", result);
		break;
	}

	freeopblk (opb);

	return result;
}
