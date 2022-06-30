/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
#ifndef _NLIST_C_
#define	_NLIST_C_

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)nlist.c	8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */
// #include <sys/cdefs.h>
// 
// #include <sys/param.h>
// #include <sys/mman.h>
// #include <sys/stat.h>
#include <sys/file.h>
#include <stddef.h>
// #include <arpa/inet.h>
// 
// #include <errno.h>
// #include <fcntl.h>
// #include <a.out.h>
// #include <stdio.h>
// #include <string.h>
// #include <unistd.h>

#define SIZE_T_MAX 0xffffffffU

int nlist(const char *name, struct nlist *list)
{
	int fd, n;

	fd = open(name, O_RDONLY, 0);
	if (fd < 0)
		return (-1);
	n = __fdnlist(fd, list);
	(void)close(fd);
	return (n);
}

static struct nlist_handlers {
	int	(*fn)(int fd, struct nlist *list);
} nlist_fn[] = {
#ifdef _NLIST_DO_AOUT
	{ __aout_fdnlist },
#endif
#ifdef _NLIST_DO_ELF
	{ __elf_fdnlist },
#endif
};

int __fdnlist(int fd, struct nlist *list)
{
	size_t i;
	int n = -1;

	for (i = 0; i < sizeof(nlist_fn) / sizeof(nlist_fn[0]); i++) {
		n = (nlist_fn[i].fn)(fd, list);
		if (n != -1)
			break;
	}
	return (n);
}

#endif /* !_NLIST_C_ */
