#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/init.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/init.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: init.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  init.c - Initialize query engine

*****************************************************************************/

#include "types.h"
#include "request.h"
#include "ds_tasks.h"
#include "util.h"
#include "read.h"

#include "quipu/util.h"
#include "quipu/photo.h"
#include "quipu/common.h"
#include "quipu/entry.h"
#include "quipu/dua.h"
#include "tailor.h"

extern int print_parse_errors;          /* Quipu variable to disable
                                           printing of errors */
extern char *oidtable,   /* Name of oidtable to use */
	   *tailfile,   /* Name of dsaptailor file to use*/
	   *myname,     /* Name of dsa to bind to */
	   *dsa_address;

char username[LINESIZE];
char userpassword[LINESIZE];

QBool want_photo_as_bytes = FALSE;

int quipu_pe_cmp();

static void read_args(), read_quipurc();

/*
 * - init_query_engine() -
 *
 */
void
init_query_engine (int argc, char *argv[]) {
	FILE *config_file;
	char linebuf[LINESIZE];

	int test_ufn_paths();

	username[0] = userpassword[0] = '\0';

	print_parse_errors = FALSE;   /* Stop auto printing of errors */
	quipu_syntaxes();

#ifdef USE_PP
	pp_quipu_init ("doog");
#endif

	dsap_init((int *) NULL, (char ***) NULL);

#ifdef USE_PP
	pp_quipu_run ();
#endif

	read_args(argc, argv);


	/*
	 *	Read `dsaptailor' file
	 */

	if((config_file = fopen(isodefile(tailfile,0), "r")) == (FILE *) NULL) {
		fprintf(stderr,
				"Cannot open `dsaptailor' file. Attempting to continue.\n"
			   );
	} else {
		while(fgets(linebuf, sizeof(linebuf), config_file) != NULLCP)
			if ((*linebuf != '#') && (*linebuf != '\n'))
				tai_string (linebuf);

		fclose(config_file);
	}


	/*
	 *	Read users' .quipurc
	 */

	read_quipurc();

	if (dsa_address == NULLCP) dsa_address = myname;

	if (want_photo_as_bytes)
		set_attribute_syntax(str2syntax("photo"),
							 (IFP) pe_cpy,
							 NULLIFP,
							 NULLIFP,
							 photo2xbm,
							 (IFP) pe_cpy,
							 quipu_pe_cmp,
							 pe_free,
							 NULLCP,
							 NULLIFP,
							 TRUE);
} /* init_query_engine */


/*
 * - read_quipurc() -
 *
 *
 */
static void
read_quipurc () {
	extern char *SkipSpace(), *TidyString(), *getenv();
	char quipurc_path[LINESIZE], line[LINESIZE];
	char *quipurc_name = "/.quipurc";
	char *p, *part1, *part2;
	FILE *config_file;


	strcpy(quipurc_path, getenv("HOME"));
	strcat(quipurc_path, quipurc_name);


	if ((config_file = fopen (quipurc_path, "r")) == 0)
		return;

	while (fgets(line, LINESIZE, config_file) != 0) {
		p = SkipSpace(line);


		/* Ignore comments and blanks */

		if (*p == '#' || *p == '\0')
			continue;

		part1 = p;
		if ((part2 = index(p,':')) == NULLCP)
			continue; /* ignore it */

		*part2++ = '\0';
		part2 = TidyString(part2);


		/*
		 *	Read users DN
		 */
		if ((lexequ(part1, "username") == 0) && *username == '\0')
			strcpy (username, part2);

		/*
		 *	Read password
		 */
		else if ((lexequ(part1, "password") == 0) && *userpassword == '\0')
			strcpy (userpassword, part2);

		/*
		 *	Dsap options
		 */
		else if (lexequ (part1, "dsap") == 0)
			tai_string (part2);

		/*
		 *	Isode options
		 */
		else if (lexequ (part1, "isode") == 0) {
			char *split;

			if ((split = index (part2,' ')) != NULLCP) {
				*split++ = 0;
				isodesetvar (part2, split, 0);
			}
		}

		/*
		 *	Service options
		 */
		else if (lexequ(part1, "service") == 0)
			new_service (part2);
	}

	isodexport(NULLCP);
	fclose(config_file);
} /* read_quipurc */

/*
 * - read_args() -
 * Parse command line args.
 *
 */
static void
read_args (int argc, char *argv[]) {
	char *argstr;

	if (argc <= 1) return;

	argv++;
	while (argc != 0 && (argstr = *argv)) {
		if (*argstr == '-') {
			switch (*++argstr) {
			case 'T':

				if (*++argv != NULLCP && *++argstr == '\0')
					load_oid_table(*argv);

				break;

			case 'c':

				if (*++argv != NULLCP  && *++argstr == '\0') myname = *argv;
				dsa_address = NULLCP;

				break;

			case 't':

				if (*++argv != NULLCP && *++argstr == '\0') tailfile = *argv;

				break;

			default:
				argv++;
			}
		}

		argv++;
		argc--;
	}
} /* read_args */

/*
 * - have_iso8859_font() -
 * Set up use of an 1so8859-1 font, to allow display of T.61 strings.
 *
 */
void
have_iso8859_font () {
	extern int ch_set;

	ch_set = 1;
}
