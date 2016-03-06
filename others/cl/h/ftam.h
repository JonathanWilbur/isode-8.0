/* ftam.h - include file for FTAM users (FS-USER) */

/*
 * $Header: /f/iso/h/RCS/ftam.h,v 5.0 88/07/21 14:38:56 mrose Rel $
 *
 *
 * $Log$
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


#ifndef	_FTAM_
#define	_FTAM_

#include "psap2.h"

/*  */

/* service levels */
#define	FLEVEL_RELIABLE	0	/*   reliable */
#define	FLEVEL_USER	1	/*   user-correctable */
#define	MY_FLEVEL	FLEVEL_RELIABLE


/* service classes */
#define	FCLASS_TRANSFER	0	/*   transfer-class */
#define	FCLASS_ACCESS	1	/*   access-class */
#define	FCLASS_MANAGE	2	/*   management-class */
#define	FCLASS_TM	3	/*   transfer-and-management-class */
#define	FCLASS_UNCONS	4	/*   unconstrained-class */

/* functional units */
#define	FUNIT_READ	0x01	/*   read */
#define	FUNIT_WRITE	0x02	/*   write */
#define	FUNIT_ACCESS	0x04	/*   file-access */
#define	FUNIT_LIMITED	0x08	/*   limited-file-managment */
#define	FUNIT_ENHANCED	0x10	/*   enhanced-file-management */
#define	FUNIT_GROUPING	0x20	/*   grouping */
#define	FUNIT_RECOVERY	0x40	/*   recovery */
#define	FUNIT_RESTART	0x80	/*   restart-data-transfer */
#define	MY_FUNIT	(FUNIT_READ | FUNIT_WRITE | FUNIT_ACCESS \
				| FUNIT_LIMITED | FUNIT_ENHANCED \
				| FUNIT_GROUPING)

/* attribute groups */
#define	FATTR_STORAGE	0x01	/*   storage */
#define	FATTR_SECURITY	0x02	/*   security */
#define	FATTR_PRIVATE	0x04	/*   private */
#define	MY_FATTR	(FATTR_STORAGE | FATTR_SECURITY)


/* state results */
#define	FSTATE_SUCCESS	0	/*   success */
#define	FSTATE_FAILURE	1	/*   failure */

/* action result */
#define	FACTION_SUCCESS	0	/*   success */
#define	FACTION_TRANS	1	/*   transient-error */
#define	FACTION_PERM	2	/*   permanent-error */


/*  */

struct FTAMcontent {
	OID	    fc_dtn;		/* document type name */

	/* associated presentation context info */
	int	    fc_id;		/*   identifier */
	int	    fc_result;		/*   status */
};


struct FTAMcontentlist {	/* list of FTAM contents */
	int	    fc_ncontent;	/* number of contents */

#define	NFCONT	(NPCTX - 2)	/* not-so-arbitrary */
	struct FTAMcontent fc_contents[NFCONT];
};

/*  */

struct FTAMdiagnostic {
	int	    ftd_type;		/* diagnostic-type */
#define	DIAG_INFORM	0	/*   informative */
#define	DIAG_TRANS	1	/*   transient */
#define	DIAG_PERM	2	/*   permanent */

	int	    ftd_identifier;	/* error-identifier */
#define	FS_CODE2OFF(c)	((c) % 1000)

#define	FS_GEN_BASE	0	/* General FTAM diagnostics */
#define	FS_GEN_NOREASON	0	/* No reason */
#define	FS_GEN_RESPONDER 1	/* Responder error (unspecific) */
#define	FS_GEN_SHUTDOWN	2	/* System shutdown */
#define	FS_GEN_MGMT	3	/* FTAM management problem (unspecific) */
#define	FS_GEN_MGMTACCT	4	/* FTAM management - bad account */
#define	FS_GEN_MGMTPASS	5	/* FTAM management - security not passed */
#define	FS_GEN_DELAY	6	/* Delay may be encountered */
#define	FS_GEN_INITIATOR 7	/* Initiator error (unspecific) */
#define	FS_GEN_SUBSEQ	8	/* Subsequent error */
#define	FS_GEN_WAITING	9	/* Indications waiting (unofficial) */

#define	FS_PRO_BASE	1000	/* Protocol and supporting service related
				   diagnostics */
#define	FS_PRO_CONFLICT	1000	/* Conflicting parameter values */
#define	FS_PRO_UNSUPORT	1001	/* Unsupported parameter values */
#define	FS_PRO_MANDATORY 1002	/* Mandatory parameter not set */
#define	FS_PRO_UNPARAM	1003	/* Unsupported parameter */
#define	FS_PRO_DUPARAM	1004	/* Duplicated parameter */
#define	FS_PRO_ILLEGAL	1005	/* Illegal parameter type */
#define	FS_PRO_UNTYPE	1006	/* Unsupported parameter type */
#define	FS_PRO_ERR	1007	/* FTAM protocol error (unspecific) */
#define	FS_PRO_ERRPROC	1008	/* FTAM protocol error - procedure error */
#define	FS_PRO_ERRFUNIT	1009	/* FTAM protocol error - functional unit
				   error */
#define	FS_PRO_ERRMSG	1010	/* FTAM protocol error - message error */
#define	FS_PRO_LOWFAIL	1011	/* Lower layer failure */
#define	FS_PRO_LOWADDR	1012	/* Lower layer addressing error */
#define	FS_PRO_TIMEOUT	1013	/* Timeout */
#define	FS_PRO_SHUTDOWN	1014	/* System shutdown */
#define	FS_PRO_GROUPING	1015	/* Illegal grouping sequence */
#define	FS_PRO_THRESH	1016	/* Grouping threshold violation */

#define	FS_ACS_BASE	2000	/* Association related diagnostics */
#define	FS_ACS_USER	2000	/* Association with user not allowed */
#define	FS_ACS_LEVEL	2001	/* Unsupported service level */
#define	FS_ACS_CLASS	2002	/* Unsupported service class */
#define	FS_ACS_FUNIT	2003	/* Unsupported functional unit */
#define	FS_ACS_GRP	2004	/* Attribute group error (unspecific) */
#define	FS_ACS_GRPSUP	2005	/* Attribute group not supported */
#define	FS_ACS_GRPALL	2006	/* Attribute group not allowed */
#define	FS_ACS_ACCT	2007	/* Bad account */
#define	FS_ACS_MGMT	2008	/* Association management (unspecific) */
#define	FS_ACS_MGMTADDR	2009	/* Association management - bad address */
#define	FS_ACS_MGMTACCT	2010	/* Association management - bad account */
#define	FS_ACS_CKPLARGE	2011	/* Checkpoint window error - too large */
#define	FS_ACS_CKPSMALL	2012	/* Checkpoint window error - too small */
#define	FS_ACS_CKPERR	2013	/* Checkpoint window error - unsupported */
#define	FS_ACS_QOS	2014	/* Communications QOS not supported */
#define	FS_ACS_IDENTITY	2015	/* Initiator identity unacceptable */
#define	FS_ACS_CONTEXT	2016	/* Context management refused */
#define	FS_ACS_ROLLBACK	2017	/* Rollback not available */
#define	FS_ACS_CNTRESP	2018	/* Contents type list cut by responder */
#define	FS_ACS_CNTPSAP	2019	/* Contents type list cut by Presentation
				   service */
#define	FS_ACS_PASSWORD	2020	/* Invalid filestore password */

#define	FS_SEL_BASE	3000	/* Selection related diagnostics */
#define	FS_SEL_FILENAME	3000	/* Filename not found */
#define	FS_SEL_MATCHED	3001	/* Selection attributes not matched */
#define	FS_SEL_INITIAL	3002	/* Initial attributes not possible */
#define	FS_SEL_ATTRNAME	3003	/* Bad attribute name */
#define	FS_SEL_NOEXIST	3004	/* Non-existent file */
#define	FS_SEL_EXISTS	3005	/* File already exists */
#define	FS_SEL_CREATE	3006	/* File can not be created */
#define	FS_SEL_DELETE	3007	/* File can not be deleted */
#define	FS_SEL_CONAVAIL	3008	/* Concurrency control not available */
#define	FS_SEL_CONSUPRT	3009	/* Concurrency control not supported */
#define	FS_SEL_CONPOSS	3010	/* Concurrency control not possible */
#define	FS_SEL_RESTRICT	3011	/* More restrictive lock */
#define	FS_SEL_BUSY	3012	/* File busy */
#define	FS_SEL_AVAIL	3013	/* File not available */
#define	FS_SEL_ACCAVAIL	3014	/* Access control not available */
#define	FS_SEL_ACCSUPRT	3015	/* Access control not supported */
#define	FS_SEL_ACCINCON	3016	/* Access control inconsistent */
#define	FS_SEL_TRUNC	3017	/* Filename truncated */
#define	FS_SEL_ALTER	3018	/* Initial attributes altered */
#define	FS_SEL_ACCOUNT	3019	/* Bad account */
#define	FS_SEL_CREEXIST	3020	/* Create override selected existing file */
#define	FS_SEL_CREOLD	3021	/* Create override deleted and recreated file
				   with old attributes */
#define	FS_SEL_CRENEW	3022	/* Create override deleted and recreated file
				   with new attributes */
#define	FS_SEL_CRELOSE	3023	/* Create override - not possible */
#define	FS_SEL_AMBIG	3024	/* Ambiguous file specification */
#define	FS_SEL_CREPASS	3025	/* Invalid create password */
#define	FS_SEL_DELPASS	3026	/* Invalid delete password on override */
#define	FS_SEL_ATRVALUE	3027	/* Bad attribute value */

#define	FS_MGT_BASE	4000	/* File management related diagnostics */
#define	FS_MGT_EXIST	4000	/* Attribute non-existant */
#define	FS_MGT_READ	4001	/* Attribute cannot be read */
#define	FS_MGT_CHANGE	4002	/* Attribute cannot be changed */
#define	FS_MGT_SUPPORT	4003	/* Attribute not supported */
#define	FS_MGT_NAME	4004	/* Bad attribute name */
#define	FS_MGT_VALUE	4005	/* Bad attribute value */

#define	FS_ACC_BASE	5000	/* Access related diagnostics */
#define	FS_ACC_FADU	5000	/* Bad FADU (unspecific) */
#define	FS_ACC_FADUSIZE	5001	/* Bad FADU - size error */
#define	FS_ACC_FADUTYPE	5002	/* Bad FADU - type error */
#define	FS_ACC_FADUSPEC	5003	/* Bad FADU - poorly specified */
#define	FS_ACC_FADULOC	5004	/* Bad FADU - bad location */
#define	FS_ACC_EXIST	5005	/* FADU does not exist */
#define	FS_ACC_AVL	5006	/* FADU not available (unspecific) */
#define	FS_ACC_AVLREAD	5007	/* FADU not available for reading */
#define	FS_ACC_AVLWRITE	5008	/* FADU not available for writing */
#define	FS_ACC_AVLLOC	5009	/* FADU not available for location */
#define	FS_ACC_AVLERASE	5010	/* FADU not available for erasure */
#define	FS_ACC_INSERT	5011	/* FADU cannot be inserted */
#define	FS_ACC_REPLACE	5012	/* FADU cannot be replaced */
#define	FS_ACC_LOCATE	5013	/* FADU cannot be located */
#define	FS_ACC_ELEMENT	5014	/* Bad data element type */
#define	FS_ACC_OPAVAIL	5015	/* Operation not available */
#define	FS_ACC_OPSUPRT	5016	/* Operation not supported */
#define	FS_ACC_OPCONS	5017	/* Operation inconsistent */
#define	FS_ACC_CONAVAIL	5018	/* Concurrency control not available */
#define	FS_ACC_CONSUPRT	5019	/* Concurrency control not supported */
#define	FS_ACC_CONINCON	5020	/* Concurrency control inconsistent */
#define	FS_ACC_MODAVAIL	5021	/* Processing mode not available */
#define	FS_ACC_MODSUPRT	5022	/* Processing mode not supported */
#define	FS_ACC_MODINCON	5023	/* Processing mode inconsistent */
#define	FS_ACC_CTXAVAIL	5024	/* Access context not available */
#define	FS_ACC_CTXSUPRT	5025	/* Access context not supported */
#define	FS_ACC_WRITE	5026	/* Bad write (unspecific) */
#define	FS_ACC_READ	5027	/* Bad read (unspecific) */
#define	FS_ACC_LCL	5028	/* Local failure (unspecific) */
#define	FS_ACC_LCLSPACE	5029	/* Local failure - file space exhausted */
#define	FS_ACC_LCLDATA	5030	/* Local failure - data corrupted */
#define	FS_ACC_LCLDEV	5031	/* Local failure - device failure */
#define	FS_ACC_FFSEXCEED 5032	/* Future filesize exceeded */
#define	FS_ACC_FFSINCRES 5034	/* Future filesize increased */
#define	FS_ACC_FUNIT	5035	/* Functional unit invalid in processing
				   mode */
#define	FS_ACC_TYPINCON	5036	/* Contents type inconsistent */
#define	FS_ACC_TYPSIMPL	5037	/* Contents type simplified */

#define	FS_RVY_BASE	6000	/* Recovery related diagnostics */
#define	FS_RVY_CKP	6000	/* Bad checkpoint (unspecific) */
#define	FS_RVY_UNIQUE	6001	/* Activity not unique */
#define	FS_RVY_WINDOW	6002	/* Checkpoint outside window */
#define	FS_RVY_EXISTS	6003	/* Activity no longer exists */
#define	FS_RVY_RECOG	6004	/* Activity not recognized */
#define	FS_RVY_NODOCKET	6005	/* No docket */
#define	FS_RVY_CODOCKET	6006	/* Corrupt docket */
#define	FS_RVY_WAITING	6007	/* File waiting restart */
#define	FS_RVY_RECOVERY	6008	/* Bad recovery point */
#define	FS_RVY_NOPOINT	6009	/* Non-existent recovery point */
#define	FS_RVY_MODAVAIL	6010	/* Recovery mode not available */
#define	FS_RVY_MODINCON	6011	/* Recovery mode inconsistent */
#define	FS_RVY_MODREDUCE 6012	/* Recovery mode reduced */
#define	FS_RVY_ACCAVAIL	6013	/* Access control not available */
#define	FS_RVY_ACCSUPRT	6014	/* Access control not supported */
#define	FS_RVY_ACCINCON	6015	/* Access control inconsistent */
#define	FS_RVY_TYPINCON	6016	/* Contents type inconsistent */
#define	FS_RVY_TYPSIMPL	6017	/* Contents type simplified */

	int	    ftd_observer;	/* error-observer */
	int	    ftd_source;		/* error-source */
#define	EREF_NONE	0	/*   no-categorizaton-possible */
#define	EREF_IFSU	1	/*   initiating-file-service-user */
#define	EREF_IFPM	2	/*   initiating-file-protocol-machine */
#define	EREF_SERV	3	/*   service-supporting-the-file-protocol-machine */
#define	EREF_RFPM	4	/*   responding-file-protocol-machine */
#define	EREF_RFSU	5	/*   responding-file-service-user */

	int	    ftd_delay;		/* suggested-delay */
#define	DIAG_NODELAY	(-1)

	/* further-details */
#define	FTD_SIZE	512	/* should be unlimited... */
	int	    ftd_cc;		/*   length */
	char    ftd_data[FTD_SIZE];	/*   data */
};

/*  */

struct FTAMcharging {		/* FTAM charging */
	int	    fc_ncharge;		/* number of charges */

#define	NFCHRG	5		/* arbitrary */
	struct fc_charge {
		char   *fc_resource;	/* resource-identifier */
		char   *fc_unit;	/* charging-unit */
		int	fc_value;	/* charging-value */
	}	    fc_charges[NFCHRG];
};

/*  */

struct FTAMpasswords {
	char   *fp_read;		/* read-password */
	int	    fp_readlen;

	char   *fp_insert;		/* insert-password */
	int	    fp_insertlen;

	char   *fp_replace;		/* replace-password */
	int	    fp_replacelen;

	char   *fp_erase;		/* erase-password */
	int	    fp_eraselen;

	char   *fp_extend;		/* extend-password */
	int	    fp_extendlen;

	char   *fp_readattr;	/* read-attribute-password */
	int	    fp_readattrlen;

	char   *fp_chngattr;	/* change-attribute-password */
	int	    fp_chngattrlen;

	char   *fp_delete;		/* delete-password */
	int	    fp_deletelen;
};
#define	FPFREE(fp) \
{ \
    register struct FTAMpasswords *FP = (fp); \
 \
    if (FP -> fp_read) \
	free (FP -> fp_read), FP -> fp_read = NULL; \
    if (FP -> fp_insert) \
	free (FP -> fp_insert), FP -> fp_insert = NULL; \
    if (FP -> fp_replace) \
	free (FP -> fp_replace), FP -> fp_replace = NULL; \
    if (FP -> fp_erase) \
	free (FP -> fp_erase), FP -> fp_erase = NULL; \
    if (FP -> fp_extend) \
	free (FP -> fp_extend), FP -> fp_extend = NULL; \
    if (FP -> fp_readattr) \
	free (FP -> fp_readattr), FP -> fp_readattr = NULL; \
    if (FP -> fp_chngattr) \
	free (FP -> fp_chngattr), FP -> fp_chngattr = NULL; \
    if (FP -> fp_delete) \
	free (FP -> fp_delete), FP -> fp_delete = NULL; \
}

/*  */

struct FTAMacelement {
	int	    fe_actions;		/* action-list */
#define	FA_PERM_READ		0x0001	/* read */
#define	FA_PERM_INSERT		0x0002	/* insert */
#define	FA_PERM_REPLACE		0x0004	/* replace */
#define	FA_PERM_EXTEND		0x0008	/* extend */
#define	FA_PERM_ERASE		0x0010	/* erase */
#define	FA_PERM_READATTR	0x0020	/* read-attribute */
#define	FA_PERM_CHNGATTR	0x0040	/* change-attribute */
#define	FA_PERM_DELETE		0x0080	/* delete-file */

	char   *fe_identity;	/* user-identity */

	struct FTAMpasswords fe_passwords;

	char   *fe_aet;		/* actually, could be EXTERNAL,
				   but we support only GraphicString for now */

	struct FTAMacelement *fe_next;
};
#define	FEFREE(fe) \
{ \
    register struct FTAMacelement  *FE, \
				   *FN; \
 \
    for (FE = (fe); FE; FE = FN) { \
	FN = FE -> fe_next; \
 \
	if (FE -> fe_identity) \
	    free (FE -> fe_identity), FE -> fe_identity = NULL; \
 \
	FPFREE (&FE -> fe_passwords); \
 \
	if (FE -> fe_aet) \
	    free (FE -> fe_aet), FE -> fe_aet = NULL; \
 \
	FE -> fe_next = NULL; \
 \
	free ((char *) FE); \
    } \
}


struct FTAMaclist {
	struct FTAMacelement *fl_insert;
	struct FTAMacelement *fl_delete;
};
#define	FLFREE(fl) \
{ \
    FEFREE ((fl) -> fl_insert) \
    (fl) -> fl_insert = NULL; \
 \
    FEFREE ((fl) -> fl_delete) \
    (fl) -> fl_delete = NULL; \
}

/*  */

struct FTAMattributes {
	long    fa_present;		/* values present */
	long    fa_novalue;		/* no value available */
#define	FA_NULL		0x00000
#define	FA_FILENAME	0x00001	/* filename */
#define	FA_CONTENTS	0x00002	/* contents-type */
#define	FA_ACCOUNT	0x00004	/* account */
#define	FA_DATE_CREATE	0x00008	/* date-and-time-of-creation */
#define	FA_DATE_MODIFY	0x00010	/* date-and-time-of-last-modification */
#define	FA_DATE_READ	0x00020	/* date-and-time-of-last-read-access */
#define	FA_DATE_ATTR	0x00040	/* date-and-time-of-last-attribute-modification */
#define	FA_ID_CREATE	0x00080	/* identity-of-creator */
#define	FA_ID_MODIFY	0x00100	/* identity-of-last-modifier */
#define	FA_ID_READ	0x00200	/* identity-of-last-reader */
#define	FA_ID_ATTR	0x00400	/* identity-of-last-attribute-modifier */
#define	FA_AVAILABILITY	0x00800	/* file-availability */
#define	FA_ACTIONS	0x01000	/* permitted-actions */
#define	FA_FILESIZE	0x02000	/* filesize */
#define	FA_FUTURESIZE	0x04000	/* future-filesize */
#define	FA_CONTROL	0x08000	/* access-control */
#define	FA_ENCRYPT	0x10000	/* encryption-name */
#define	FA_LEGAL	0x20000	/* legal-qualifications */
#define	FA_PRIVATE	0x40000	/* private-use */

#define	FA_STORAGE	(FA_ACCOUNT | FA_DATE_CREATE | FA_DATE_MODIFY \
			    | FA_DATE_READ | FA_DATE_ATTR | FA_ID_CREATE \
 			    | FA_ID_MODIFY | FA_ID_READ | FA_ID_ATTR \
			    | FA_AVAILABILITY | FA_ACTIONS | FA_FILESIZE \
			    | FA_FUTURESIZE)
#define	FA_SECURITY	(FA_CONTROL | FA_ENCRYPT | FA_LEGAL)

#define	NFFILE	5		/* arbitrary */
	int	    fa_nfile;		/* filename */
	char   *fa_files[NFFILE];	/*   .. */

	OID	    fa_contents;	/* contents-type */

	char   *fa_account;		/* account */

	/* date-and-time-of- ... */
	struct UTCtime fa_date_create;
	struct UTCtime fa_date_modify;
	struct UTCtime fa_date_read;
	struct UTCtime fa_date_attribute;

	/* identity-of- ... */
	char   *fa_id_create;
	char   *fa_id_modify;
	char   *fa_id_read;
	char   *fa_id_attribute;

	int	    fa_availability;	/* file-availability */
#define	FA_AVAIL_IMMED	0	/*   immediate */
#define	FA_AVAIL_DEFER	1	/*   deferred */

	int	    fa_permitted;	/* permitted-actions,
				   same as fe_actions, plus: */
#define	FA_PERM_TRAV		0x0100	/* traversal */
#define	FA_PERM_RVTRAV		0x0200	/* reverse-traversal */
#define	FA_PERM_RANDOM		0x0400	/* random-order */
#define	FA_PERM_TRAVERSAL	(FA_PERM_TRAV | FA_PERM_RVTRAV \
					| FA_PERM_RANDOM)

	int	    fa_filesize;	/* filesize */
	int	    fa_futuresize;	/* future-filesize */

	struct FTAMaclist fa_control;/* access-control */
	char   *fa_encrypt;		/* encryption-name */
	char   *fa_legal;		/* legal-qualification */
};

void	FAFREE ();

/*  */

struct FTAMconcurrency {
#define	FLOCK_SHARED	00	/* shared */
#define	FLOCK_EXCLUSIVE	01	/* exclusive */
#define	FLOCK_NOTREQD	02	/* not-required */
#define	FLOCK_NOACCESS	03	/* no-access */
#define	FLOCK_MASK	03
#define	FLOCK_PRESENT	FLOCK_EXCLUSIVE
#define	FLOCK_RESTRICT	01

	char    fc_readlock;
	char    fc_insertlock;
	char    fc_replacelock;
	char    fc_eraselock;
	char    fc_extendlock;
	char    fc_readattrlock;
	char    fc_chngattrlock;
	char    fc_deletelock;
};
#define	FCINIT(fc) \
{ \
    (fc) -> fc_readlock = FLOCK_NOTREQD; \
    (fc) -> fc_insertlock = FLOCK_NOTREQD; \
    (fc) -> fc_replacelock = FLOCK_NOTREQD; \
    (fc) -> fc_eraselock = FLOCK_NOTREQD; \
    (fc) -> fc_extendlock = FLOCK_NOTREQD; \
    (fc) -> fc_readattrlock = FLOCK_NOTREQD; \
    (fc) -> fc_chngattrlock = FLOCK_NOTREQD; \
    (fc) -> fc_deletelock = FLOCK_NOTREQD; \
}

/*  */

struct FADUidentity {
	int	    fa_type;
#define	FA_FIRSTLAST	0	/* first-last */
#define	FA_RELATIVE	1	/* relative */
#define	FA_BEGINEND	2	/* begin-end */
#define	FA_SINGLE	3	/* single-name */
#define	FA_NAMELIST	4	/* name-list */
#define	FA_FADUNUMBER	5	/* fadu-number */

	union {
		int	fa_un_firstlast;
#define	FA_FIRST	0
#define	FA_LAST		1

		int	fa_un_relative;
#define	FA_PREVIOUS	0
#define	FA_CURRENT	1
#define	FA_NEXT		2

		int	fa_un_beginend;
#define	FA_BEGIN	0
#define	FA_END		1

		char   *fa_un_singlename;

#define	NANAME	5		/* arbitrary */
		struct {
			char   *fa_un_names[NANAME];
			int	    fa_un_nname;
		}	fa_un_list;

		int	fa_un_fadunumber;
	}	fa_un;
#define	fa_firstlast	fa_un.fa_un_firstlast
#define	fa_relative	fa_un.fa_un_relative
#define	fa_beginend	fa_un.fa_un_beginend
#define	fa_singlename	fa_un.fa_un_singlename
#define	fa_names	fa_un.fa_un_list.fa_un_names
#define	fa_nname	fa_un.fa_un_list.fa_un_nname
#define	fa_fadunumber	fa_un.fa_un_fadunumber

	int	    fa_levelnumber;
};
#define	FUFREE(fu) \
{ \
    register int    FUI; \
 \
    if ((fu) -> fa_singlename) \
	free ((fu) -> fa_singlename), (fu) -> fa_singlename = NULL; \
 \
    for (FUI = (fu) -> fa_nname - 1; FUI >= 0; FUI--) \
	if ((fu) -> fa_names[FUI]) \
	    free ((fu) -> fa_names[FUI]), (fu) -> fa_names[FUI] = NULL; \
}

/*  */

struct FTAMstart {		/* F-INITIALIZE.INDICATION */
	int	    fts_sd;		/* FTAM descriptor */

	AEInfo fts_callingtitle;	/* info on calling application-entity */
	AEInfo fts_calledtitle;	/* info called application-entity */

	struct PSAPaddr fts_calledaddr;/* called presentation address */
	struct PSAPaddr fts_callingaddr;/* calling presentation address */

	int	    fts_manage;		/* presentation context management */

	int	    fts_level;		/* service level */
	int	    fts_class;		/* service class */

	int	    fts_units;		/* functional units */
	int	    fts_attrs;		/* attribute groups */

	int	    fts_rollback;	/* rollback availability */

	struct FTAMcontentlist fts_contents;/* contents type list */

	char   *fts_initiator;	/* initiator */
	char   *fts_account;	/* account */
	char   *fts_password;	/* password */
	int	    fts_passlen;	/*   .. */

	int	    fts_ssdusize;	/* largest atomic SSDU */

	struct QOStype fts_qos;	/* quality of service */
};
#define	FTSFREE(fts) \
{ \
    register int FSI; \
 \
    AEIFREE (&(fts) -> fts_callingtitle); \
    AEIFREE (&(fts) -> fts_calledtitle); \
 \
    for (FSI = (fts) -> fts_contents.fc_ncontent - 1; FSI >= 0; FSI--) \
	oid_free ((fts) -> fts_contents.fc_contents[FSI].fc_dtn), \
		(fts) -> fts_contents.fc_contents[FSI].fc_dtn = NULLOID; \
    (fts) -> fts_contents.fc_ncontent = 0; \
 \
    if ((fts) -> fts_initiator) \
	free ((fts) -> fts_initiator), (fts) -> fts_initiator = NULL; \
    if ((fts) -> fts_account) \
	free ((fts) -> fts_account), (fts) -> fts_account = NULL; \
    if ((fts) -> fts_password) \
	free ((fts) -> fts_password), (fts) -> fts_password = NULL; \
}


struct FTAMconnect {		/* F-INITIALIZE.CONFIRMATION */
	int	    ftc_sd;		/* FTAM descriptor */

	int	    ftc_state;		/* state result */
	int	    ftc_action;		/* action result */

	AEInfo ftc_respondtitle;	/* responding application-entity title */

	struct PSAPaddr ftc_respondaddr;/* responding presentation address */

	int	    ftc_manage;		/* presentation context management */

	int	    ftc_units;		/* functional units */
	int	    ftc_attrs;		/* attribute groups */

	int	    ftc_rollback;	/* rollback availability */

	struct FTAMcontentlist ftc_contents;/* contents type list */

#define	NFDIAG	5		/* diagnostics */
	int	    ftc_ndiag;
	struct FTAMdiagnostic  ftc_diags[NFDIAG];

	int	    ftc_ssdusize;	/* largest atomic SSDU */

	struct QOStype ftc_qos;	/* quality of service */
};
#define	FTCFREE(ftc) \
{ \
    register int FCI; \
 \
    AEIFREE (&(ftc) -> ftc_respondtitle); \
 \
    for (FCI = (ftc) -> ftc_contents.fc_ncontent - 1; FCI >= 0; FCI--) \
	oid_free ((ftc) -> ftc_contents.fc_contents[FCI].fc_dtn), \
		(ftc) -> ftc_contents.fc_contents[FCI].fc_dtn = NULLOID; \
    (ftc) -> ftc_contents.fc_ncontent = 0; \
}


struct FTAMfinish {		/* F-TERMINATE.INDICATION */
	int	    ftf_dummy;
};


struct FTAMrelease {		/* F-TERMINATE.CONFIRMATION */
	/* charging */
	struct FTAMcharging ftr_charges;
};
#define	FTRFREE(ftr) \
{ \
    register int    FRI; \
    register struct fc_charge  *FC; \
 \
    for (FC = (ftr) -> ftr_charges.fc_charges, FRI = (ftr) -> ftr_charges.fc_ncharge - 1; \
	    FRI >= 0; \
	    FC++, FRI--) { \
	if (FC -> fc_resource) \
	    free (FC -> fc_resource), FC -> fc_resource = NULL; \
	if (FC -> fc_unit) \
	    free (FC -> fc_unit), FC -> fc_unit = NULL; \
    } \
    (ftr) -> ftr_charges.fc_ncharge = 0; \
}


struct FTAMabort {		/* F-{U,P}-ABORT.INDICATION */
	int	    fta_peer;		/* T   = F-U-ABORT.INDICATION
				   NIL = F-P-ABORT.INDICATION */

	int	    fta_action;		/* action result */

	int	    fta_ndiag;		/* diagnostics */
	struct FTAMdiagnostic fta_diags[NFDIAG];
};


struct FTAMselect {		/* F-SELECT.* */
	/* RESPONSE only */
	int     ftse_state;		/* state result */
	int     ftse_action;	/* action result */

	/* attributes (FILENAME only) */
	struct FTAMattributes ftse_attrs;

	/* REQUEST only */
	int	    ftse_access;	/* requested access */
#define	FA_REQ_MASK	(FA_PERM_READ | FA_PERM_INSERT | FA_PERM_REPLACE \
				| FA_PERM_EXTEND | FA_PERM_ERASE \
				| FA_PERM_READATTR | FA_PERM_CHNGATTR \
				| FA_PERM_DELETE)
	/* access-passwords */
	struct FTAMpasswords ftse_pwds;
	/* concurrency-control */
	struct FTAMconcurrency ftse_conctl;
	char   *ftse_account;	/* account */

	/* RESPONSE only */
	int	    ftse_ndiag;		/* diagnostics */
	struct FTAMdiagnostic ftse_diags[NFDIAG];
};
#define	FTSEFREE(ftse) \
{ \
    FAFREE(&((ftse) -> ftse_attrs)); \
    FPFREE(&((ftse) -> ftse_pwds)); \
    if ((ftse) -> ftse_account) \
	free ((ftse) -> ftse_account), (ftse) -> ftse_account = NULLCP; \
}


struct FTAMdeselect {		/* F-DESELECT.* */
	/* these are all RESPONSE only */
	int     ftde_action;	/* action result */

	/* charging */
	struct FTAMcharging ftde_charges;

	int     ftde_ndiag;		/* diagnostics */
	struct FTAMdiagnostic   ftde_diags[NFDIAG];
};
#define	FTDEFREE(ftde) \
{ \
    register int    FDEI; \
    register struct fc_charge  *FC; \
 \
    for (FC = (ftde) -> ftde_charges.fc_charges, FDEI = (ftde) -> ftde_charges.fc_ncharge - 1; \
	    FDEI >= 0; \
	    FC++, FDEI--) { \
	if (FC -> fc_resource) \
	    free (FC -> fc_resource), FC -> fc_resource = NULL; \
	if (FC -> fc_unit) \
	    free (FC -> fc_unit), FC -> fc_unit = NULL; \
    } \
    (ftde) -> ftde_charges.fc_ncharge = 0; \
}


struct FTAMcreate {		/* F-CREATE.* */
	/* RESPONSE only */
	int     ftce_state;		/* state result */
	int     ftce_action;	/* action result */

	/* REQUEST only */
	int	    ftce_override;	/* override */
#define	FOVER_FAIL	0	/* fail, if already exists */
#define	FOVER_SELECT	1	/* select, if it already exists */
#define	FOVER_WRITE	2	/* zero-truncate, if it already exists */
#define	FOVER_DELETE	3	/* delete, if it already exists */
	char   *ftce_create;	/* create password */
	int	    ftce_crelen;	/*   .. */
	char   *ftce_delete;	/* delete password */
	int	    ftce_dellen;	/*   .. */

	/* initial attributes */
	struct FTAMattributes ftce_attrs;

	/* REQUEST only */
	int	    ftce_access;	/* requested access */
	/* access-passwords */
	struct FTAMpasswords ftce_pwds;
	/* concurrency-control */
	struct FTAMconcurrency ftce_conctl;
	char   *ftce_account;	/* account */

	/* RESPONSE only */
	int	    ftce_ndiag;		/* diagnostics */
	struct FTAMdiagnostic ftce_diags[NFDIAG];
};
#define	FTCEFREE(ftce) \
{ \
    if ((ftce) -> ftce_create) \
	free ((ftce) -> ftce_create), (ftce) -> ftce_create = NULLCP; \
    if ((ftce) -> ftce_delete) \
	free ((ftce) -> ftce_delete), (ftce) -> ftce_delete = NULLCP; \
    FAFREE(&((ftce) -> ftce_attrs)); \
    FPFREE(&((ftce) -> ftce_pwds)); \
    if ((ftce) -> ftce_account) \
	free ((ftce) -> ftce_account), (ftce) -> ftce_account = NULLCP; \
}


struct FTAMdelete {		/* F-DELETE.* */
	/* RESPONSE only */
	int	    ftxe_action;	/* action result */

	/* REQUEST only */
	char   *ftxe_delete;	/* delete password */
	int	    ftxe_dellen;

	/* RESPONSE only */
	/* charging */
	struct FTAMcharging ftxe_charges;
	int     ftxe_ndiag;		/* diagnostics */
	struct FTAMdiagnostic   ftxe_diags[NFDIAG];
};
#define	FTXEFREE(ftxe) \
{ \
    register int    FXEI; \
    register struct fc_charge  *FC; \
 \
    if ((ftxe) -> ftxe_delete) \
	free ((ftxe) -> ftxe_delete), (ftxe) -> ftxe_delete = NULLCP; \
 \
    for (FC = (ftxe) -> ftxe_charges.fc_charges, FXEI = (ftxe) -> ftxe_charges.fc_ncharge - 1; \
	    FXEI >= 0; \
	    FC++, FXEI--) { \
	if (FC -> fc_resource) \
	    free (FC -> fc_resource), FC -> fc_resource = NULL; \
	if (FC -> fc_unit) \
	    free (FC -> fc_unit), FC -> fc_unit = NULL; \
    } \
    (ftxe) -> ftxe_charges.fc_ncharge = 0; \
}


struct FTAMreadattr {		/* F-READ-ATTRIB.* */
	/* RESPONSE only */
	int	    ftra_action;	/* action result */

	/* REQUEST only */
	int	    ftra_attrnames;	/* attribute names (from fa_present) */

	/* RESPONSE only */
	struct FTAMattributes ftra_attrs;
	int	    ftra_ndiag;		/* diagnostics */
	struct FTAMdiagnostic ftra_diags[NFDIAG];
};
#define	FTRAFREE(ftra) \
{ \
    FAFREE(&((ftra) -> ftra_attrs)); \
}


struct FTAMchngattr {		/* F-CHANGE-ATTRIB.* */
	/* RESPONSE only */
	int ftca_action;		/* action result */

	struct FTAMattributes ftca_attrs;

	/* RESPONSE only */
	int	    ftca_ndiag;		/* diagnostics */
	struct FTAMdiagnostic ftca_diags[NFDIAG];
};
#define	FTCAFREE(ftca) \
{ \
    FAFREE(&((ftca) -> ftca_attrs)); \
}


struct FTAMopen {		/* F-OPEN.* */
	/* RESPONSE only */
	int	    ftop_state;		/* state result */
	int	    ftop_action;	/* action result */

	/* REQUEST only */
	int	    ftop_mode;		/* processing mode (read..extend) */
#define	FA_MODE_MASK	(FA_PERM_READ | FA_PERM_INSERT | FA_PERM_REPLACE \
				| FA_PERM_EXTEND | FA_PERM_ERASE)

	OID	    ftop_contents;	/* contents type */
	/* concurrency-control */
	struct FTAMconcurrency ftop_conctl;

	/* RESPONSE only */
	int	    ftop_ndiag;		/* diagnostics */
	struct FTAMdiagnostic ftop_diags[NFDIAG];
};
#define	FTOPFREE(ftop) \
{ \
    if ((ftop) -> ftop_contents) \
	oid_free ((ftop) -> ftop_contents), \
	    (ftop) -> ftop_contents = NULLOID; \
}


struct FTAMclose {		/* F-CLOSE.* */
	/* these are all RESPONSE only */
	int	    ftcl_action;	/* action result */

	int     ftcl_ndiag;		/* diagnostics */
	struct FTAMdiagnostic   ftcl_diags[NFDIAG];
};


struct FTAMgroup {
	int     ftg_threshold;	/* threshold */

	int     ftg_flags;
#define	FTG_NULL	0x0000
#define	FTG_BEGIN	0x0001	/* have begin */
#define	FTG_SELECT	0x0002	/*   .. select */
#define	FTG_CREATE	0x0004	/*   .. create */
#define	FTG_RDATTR	0x0008	/*   .. read attribute */
#define	FTG_CHATTR	0x0010	/*   .. change attribute */
#define	FTG_OPEN	0x0020	/*   .. open */
#define	FTG_CLOSE	0x0040	/*   .. close */
#define	FTG_DESELECT	0x0080	/*   .. deselect */
#define	FTG_DELETE	0x0100	/*   .. delete */
#define	FTG_END		0x0200	/*   .. end */

	union {
		struct FTAMselect   ftg_un1_select;
		struct FTAMcreate   ftg_un1_create;
		struct FTAMclose    ftg_un1_close;
	}                   ftg_un1;
#define	ftg_select	ftg_un1.ftg_un1_select
#define	ftg_create	ftg_un1.ftg_un1_create
#define	ftg_close	ftg_un1.ftg_un1_close

	struct FTAMreadattr ftg_readattr;

	struct FTAMchngattr ftg_chngattr;

	union {
		struct FTAMdeselect ftg_un2_deselect;
		struct FTAMdelete   ftg_un2_delete;
		struct FTAMopen	    ftg_un2_open;
	} ftg_un2;
#define	ftg_deselect	ftg_un2.ftg_un2_deselect
#define	ftg_delete	ftg_un2.ftg_un2_delete
#define	ftg_open	ftg_un2.ftg_un2_open
};
#define	FTGFREE(ftg) \
{ \
    if ((ftg) -> ftg_flags & FTG_SELECT) { \
	FTSEFREE (&((ftg) -> ftg_select)); \
    } \
    else \
	if ((ftg) -> ftg_flags & FTG_CREATE) \
	    FTCEFREE (&((ftg) -> ftg_create)); \
 \
    if ((ftg) ->ftg_flags & FTG_RDATTR) \
	FTRAFREE (&((ftg) -> ftg_readattr)); \
 \
    if ((ftg) ->ftg_flags & FTG_CHATTR) \
	FTCAFREE (&((ftg) -> ftg_chngattr)); \
 \
    if ((ftg) -> ftg_flags & FTG_DESELECT) { \
	FTDEFREE (&((ftg) -> ftg_deselect)); \
    } \
    else \
	if ((ftg) -> ftg_flags & FTG_DELETE) { \
	    FTXEFREE (&((ftg) -> ftg_delete)); \
	} \
	else \
	    if ((ftg) -> ftg_flags & FTG_OPEN) \
		FTOPFREE (&((ftg) -> ftg_open)); \
}


struct FTAMaccess {		/* F-{LOCATE,ERASE}.{INDICATION,CONFIRMATION} */
	int	    ftac_operation;
#define	FA_OPS_LOCATE	0	/* locate */
#define	FA_OPS_ERASE	1	/* erase */

	/* RESPONSE only */
	int	    ftac_action;	/* action result */

	/* REQUEST ONLY */
	/* fadu-identity */
	struct FADUidentity ftac_identity;

	/* RESPONSE only */
	int     ftac_ndiag;		/* diagnostics */
	struct FTAMdiagnostic   ftac_diags[NFDIAG];
};
#define	FTACFREE(ftac) \
{ \
    FUFREE (&((ftac) -> ftac_identity)); \
}


struct FTAMreadwrite {		/* F-{READ,WRITE}.INDICATION */
	int	    ftrw_operation;	/* fadu-operation */
#define	FA_OPS_READ	(-1)	/*   read (pseudo) */
#define	FA_OPS_INSERT	0	/*   insert */
#define	FA_OPS_REPLACE	1	/*   replace */
#define	FA_OPS_EXTEND	2	/*   extend */

	/* fadu-identity */
	struct FADUidentity	ftrw_identity;

	int	    ftrw_context;	/* access-context */
#define	FA_ACC_HA	0	/*   hierarchical-all-data-units */
#define	FA_ACC_HN	1	/*   hierarchical-no-data-units */
#define	FA_ACC_FA	2	/*   flat-all-data-units */
#define	FA_ACC_FL	3	/*   flat-one-level-data-units */
#define	FA_ACC_FS	4	/*   flat-single-data-unit */
#define	FA_ACC_UA	5	/*   unstructured-all-data-units */
#define	FA_ACC_US	6	/*   unstructured-single-data-unit */
};
#define	FTRWFREE(ftrw) \
{ \
    FUFREE (&((ftrw) -> ftrw_identity)); \
}


struct FTAMdataend {		/* F-DATA-END.INDICATION */
	int	    ftda_action;	/* action-result */

	int     ftda_ndiag;		/* diagnostics */
	struct FTAMdiagnostic   ftda_diags[NFDIAG];
};


struct FTAMcancel {		/* F-CANCEL.{INDICATION,CONFIRMATION} */
	int	    ftcn_action;	/* action-result */

	int     ftcn_ndiag;		/* diagnostics */
	struct FTAMdiagnostic   ftcn_diags[NFDIAG];
};


struct FTAMtransend {		/* F-TRANSFER-END.{INDICATION,CONFIRMATION} */
	/* all these are RESPONSE only */
	int	    ftre_action;	/* action-result */

	int     ftre_ndiag;		/* diagnostics */
	struct FTAMdiagnostic   ftre_diags[NFDIAG];
};


struct FTAMindication {
	int     fti_type;		/* the union element present */
#define	FTI_FINISH	0x00
#define	FTI_ABORT	0x01
#define	FTI_MANAGEMENT	0x02
#define	FTI_BULKBEGIN	0x03
#define	FTI_BULKEND	0x04
#define	FTI_ACCESS	0x05
#define	FTI_READWRITE	0x06
#define	FTI_DATA	0x07
#define	FTI_DATAEND	0x08
#define	FTI_CANCEL	0x09
#define	FTI_TRANSEND	0x10

	union {
		struct FTAMfinish   fti_un_finish;
		struct FTAMabort    fti_un_abort;
		struct FTAMgroup    fti_un_group;
		struct FTAMaccess   fti_un_access;
		struct FTAMreadwrite fti_un_readwrite;
		struct PSAPdata	    fti_un_data;
		struct FTAMdataend  fti_un_dataend;
		struct FTAMcancel   fti_un_cancel;
		struct FTAMtransend fti_un_transend;
	}	fti_un;
#define	fti_finish	fti_un.fti_un_finish
#define	fti_abort	fti_un.fti_un_abort
#define	fti_group	fti_un.fti_un_group
#define	fti_access	fti_un.fti_un_access
#define	fti_readwrite	fti_un.fti_un_readwrite
#define	fti_data	fti_un.fti_un_data
#define	fti_dataend	fti_un.fti_un_dataend
#define	fti_cancel	fti_un.fti_un_cancel
#define	fti_transend	fti_un.fti_un_transend
};

/* when FTAMindication has PSAPdata, the pe_context indicates whether
   each data is from the FTAM PCI or is a data element.

	FTAM PCI	- PE_DFLT_CTX

	data element	- anything else

   three different types of data in the FTAM PCI are handled by the user:

	Node-Descriptor-Data-Element ::=	[APPLICATION 21] ...
	Enter-Subtree-Data-Element ::=		[APPLICATION 22] ...
	Exit-Subtree-Data-Element ::=		[APPLICATION 23] ...
 */

#define	FADU_NODESCR	21	/* Node-Descriptor-Data-Element */
#define	FADU_ENTERTREE	22	/* Enter-Subtree-Data-Element */
#define	FADU_EXITREE	23	/* Exit-Subtree-Data-Element */

/*  */

extern char *ftamversion;

extern char *ftamfile;


int	FInit ();		/* F-INITIALIZE.INDICATION */
int	FInitializeResponse ();	/* F-INITIALIZE.RESPONSE */
int	FInitializeRequest ();	/* F-INITIALIZE.REQUEST */
int	FTerminateRequest ();	/* F-TERMINATE.REQUEST */
int	FTerminateResponse ();	/* F-TERMINATE.RESPONSE */
int	FUAbortRequest ();	/* F-U-ABORT.REQUEST */

int	FWaitRequest ();	/* F-WAIT.REQUEST (pseudo) */

int	FManageRequest ();	/* F-MANAGE.REQUEST (group) */
int	FManageResponse ();	/* F-MANAGE.RESPONSE (group) */
int	FBulkBeginRequest ();	/* F-BULK-BEGIN.REQUEST (group) */
int	FBulkBeginResponse ();	/* F-BULK-BEGIN.RESPONSE (group) */
int	FBulkEndRequest ();	/* F-BULK-END.REQUEST (group) */
int	FBulkEndResponse ();	/* F-BULK-END.RESPONSE (group) */

int	FAccessRequest ();	/* F-{LOCATE,ERASE}.REQUEST */
int	FAccessResponse ();	/* F-{LOCATE,ERASE}.RESPONSE */

int	FReadWriteRequest ();	/* F-{READ,WRITE}.REQUEST */
int	FDataRequest ();	/* F-DATA.REQUEST */
int	FDataEndRequest ();	/* F-DATA-END.REQUEST */
int	FCancelRequest ();	/* F-CANCEL.REQUEST */
int	FCancelResponse ();	/* F-CANCEL.RESPONSE */
int	FTransEndRequest ();	/* F-TRANSFER-END.REQUEST */
int	FTransEndResponse ();	/* F-TRANSFER-END.RESPONSE */

int	FSetIndications ();	/* define vector for INDICATION events */
int	FSelectMask ();		/* map ftam descriptors for select() */

int	FHookRequest ();	/* set tracing */
int	FTraceHook ();		/* user-defined tracing */

char   *FErrString ();		/* return FTAM error code in string form */

/*  */

struct isodocument {
	char   *id_entry;

	OID	    id_type;

	OID	    id_constraint;
	OID	    id_abstract;
	OID	    id_transfer;
};

int	setisodocument (), endisodocument ();

struct isodocument *getisodocument ();

struct isodocument *getisodocumentbyentry ();
struct isodocument *getisodocumentbytype ();

#endif
