/* actions5.c - VTPM: FSM sector 5 actions */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/vt/RCS/actions5.c,v 9.0 1992/06/16 12:41:08 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/vt/RCS/actions5.c,v 9.0 1992/06/16 12:41:08 isode Rel $
 *
 *
 * $Log: actions5.c,v $
 * Revision 9.0  1992/06/16  12:41:08  isode
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


#include "vtpm.h"
#include "sector1.h"

/************************************************************************/
/*	This file contains the functions that are executed when the	*/
/*	VT Protocol machine is in a Sector 5 state and a protocol	*/
/*	event occurs.  The state transition matrix is specified in	*/
/*	Table 32 of ISO 9041 (July 1987 version).  The actions which	*/
/*	this collection of functions perform are specified in Table 40	*/
/*	of ISO 9041.							*/
/************************************************************************/

extern int sd;		/*Global Session Descriptor (ISODE) */

void	adios (char *, char *, ...);
void	advise (int, char *, char *, ...);

/*  xx1x xxxx = awaiting ack from peer i.e., 420 */
/*  xxxx xx1x = awaiting ack from user */

/* T = got token, N = no got token */


/*
   req: usr==>vtpm
   ind: vtpm==>usr
*/
int
ce_104 (	/* common event 104 */
	PE pe
) {
	/* if (vnt > 0) */
	if(pe != NULLPE) vdatind(SEQUENCED,pe);
	vnt = 0;
	return(OK);
}

int
ce_105 (void) {	/* common event 105 */
	/* if (vns > 0)  for(... */
	if(p_ondq != NULLPE)
		p_data(p_ondq);  /* send NDQ	*/
	vns = 0;
	return(OK);
}


/* ARGSUSED */
int
a5_0 (	/*VDATreq-sqtr in states 400B or 402B */
	/* V data request addressing sequenced trigger co */
	PE pe
) {
	return(ce_105());
	/*
		==> SAMESTATE;
	*/
}


/* ARGSUSED */
int
a5_1 (	/*VDATreq-n in states 400B, 402B or 40T */
	/* V data request addressing sequenced trigger co */
	PE pe
) {

	/*
	vns++;
	==> SAMESTATE
	*/
	return(ce_105());	/*Autonomous Event to Ship it*/
}


int
a5_2 (	/*NDQ-tr in states 400B, 420B */
	PE pe
) {
	/*
	vnt++;
	*/

	return(ce_104(pe));
	/*
	==> SAMESTATE
	*/
}


int
a5_3 (	/*NDQ-ntr in states 400B, 420B */
	PE pe
) {
	/*
	vnt++;
	*/
	/*
	==> SAMESTATE
	*/
	return(ce_104(pe));	/*Autonomous Event to Deliver to User*/
}

int
a5_5 (	/* VBRKreq  */
	PE pe
) {
	vtok = 0; /* giving the token away */
	vnt = 0;
	vns = 0;
	/* vtkp was set in vbrkreq so it could be coded in to the pe */
	p_resync_req(pe,SYNC_RESTART); /* send break request */
	state = S5_61;
	return(OK);
}

int
a5_6 (	/* VBRKrsp in state 62 */
	PE pe
) {
	p_resync_resp(pe); /* send out break response */
	if (vsmd && vtok)
		state = S5_40T;
	else if (vsmd)
		state = S5_40N;
	else {
		vtkp = INITIATOR;
		if (vtok)
			vtkp = ACCEPTOR;
		state = S5_400B;
	}
	return(OK);
}

int
a5_9 (	/*VDELreq in states 400B, 402B */
	PE pe
) {
	if (dcno) { /* no delivery control */
		advise(LLOG_DEBUG,NULLCP,"a5_9: dcno hasn't been set");
		/* ==> SAMESTATE */
		return(NOTOK);
	}
	ce_105();
	/* send out dlq */
	/* this will be replace by the new-fangled pepy schtuff;
		will use this now for compatability */

	p_data(pe);
	state = (vra) ? state + 2 : state; /* pretty neeto eh? */
	return(OK);
}

int
a5_11 (	/*HDQ request in 400B*/
	PE pe
) {
	p_typed_data(pe);
	return(OK);
}

/*ARGSUSED*/
int
a5_17 (	/*VRELreq in states 400B */
	PE pe
) {
	/*	ce_105(); */
	sector = 1;
	if(vtok) {
		state = S1_51Q;		/*Must change state first because
					  vt_disconnect gets RLR & calls
					  state machine again. */
		vt_disconnect();	/*May be only TEMP*/
	} else {
		request_token();
		/*Need call to ISODE to request token*/
		state = S1_50B;
	}

	return(OK);
}

int
a5_28 (	/*UDQ request in 400B*/
	PE pe
) {
	p_typed_data(pe);
	return(OK);
}

int
a5_31 (	/* BKR in 61 */
	PE pe
) {
	if (vsmd && vtok) state = S5_40T;
	else if (vsmd) state = S5_40N;
	else state = S5_400B;
	vbrkcnf(pe);
	return(OK);
}

int
a5_32 (	/* BKQ could occur in any state except 62 */
	PE pe
) {
	vnt = 0;
	vns = 0;
	/*
	   vbrkind clears queues etc.
	   and then map the break character to user
	   and sets vtok to 1
	   (we should have received the token)
	*/
	state = S5_62;
	vbrkind(pe);
	return(OK);
}

int
a5_34 (	/*UDQ in 400B*/
	PE pe
) {
	if(pe != NULLPE) vudatind(pe);
	return(OK);
}

int
a5_35 (	/* DEL in states 400B, 420B */
	PE pe
) {

	if ((vra = prim2flag(pe)) == NOTOK)
		adios("a5_35: bogus PDU (%s)", pe_error (pe -> pe_errno));
	ce_104(NULLPE);
	vdelind(pe,vra);
	state = (vra) ? state + 2 : state;
	return(OK);
}


int
a5_38 (	/* RLQ in states 400B */
	PE pe
) {

	ce_104(pe);
	sector = 1;
	state = S1_51R;
	vrelind();
	return(OK);
}

int
a5_106 (PE pe) {
	if(pe != NULLPE) vhdatind(pe);
	return(OK);
}
