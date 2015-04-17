/*	$Id$ */
/*
 * Copyright (c) 2015 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "config.h"

#include <sys/stat.h>
#include <sys/mman.h>

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "extern.h"

int verbose = 1;

static void
ical_printcomp(const struct icalcomp *c)
{
	size_t	 i;

	assert(NULL != c);
	assert(ICALTYPE__MAX != c->type);

	printf("[%s] Parsed...\n", icaltypes[c->type]);
	if (NULL != c->uid)
		printf("[%s] UID = %s\n", 
			icaltypes[c->type], c->uid);
	if (NULL != c->tzid)
		printf("[%s] TZID = %s\n", 
			icaltypes[c->type], c->tzid);
	if (0 != c->created)
		printf("[%s] CREATED = %s", 
			icaltypes[c->type], ctime(&c->created));
	if (0 != c->lastmod)
		printf("[%s] LASTMODIFIED = %s", 
			icaltypes[c->type], ctime(&c->lastmod));
	if (0 != c->dtstamp)
		printf("[%s] DTSTAMP = %s", 
			icaltypes[c->type], ctime(&c->dtstamp));
	for (i = 0; i < c->tzsz; i++) {
		if (NULL != c->tzs[i].tzid)
			printf("[%s:%s] TZID = %s\n", 
				icaltypes[c->type], 
				icaltztypes[c->tzs[i].type], 
				c->tzs[i].tzid);
		if (0 != c->tzs[i].dtstart)
			printf("[%s:%s] DTSTART = %s", 
				icaltypes[c->type], 
				icaltztypes[c->tzs[i].type], 
				ctime(&c->tzs[i].dtstart));
		if (0 != c->tzs[i].tzto)
			printf("[%s:%s] TZOFFSETTO = %d\n", 
				icaltypes[c->type], 
				icaltztypes[c->tzs[i].type], 
				c->tzs[i].tzto);
		if (0 != c->tzs[i].tzfrom)
			printf("[%s:%s] TZOFFSETFROM = %d\n", 
				icaltypes[c->type], 
				icaltztypes[c->tzs[i].type], 
				c->tzs[i].tzfrom);
		if (NULL != c->tzs[i].rrule)
			printf("[%s:%s] RRULE = %s\n", 
				icaltypes[c->type], 
				icaltztypes[c->tzs[i].type], 
				c->tzs[i].rrule);
	}

	if (NULL != c->next)
		ical_printcomp(c->next);
}

int
main(int argc, char *argv[])
{
	int		 fd, c;
	struct stat	 st;
	size_t		 sz;
	char		*map;
	struct ical	*p = NULL;

	if (-1 != (c = getopt(argc, argv, "")))
		return(EXIT_FAILURE);

	argc -= optind;
	argv += optind;

	if (0 == argc)
		return(EXIT_FAILURE);

	if (-1 == (fd = open(argv[0], O_RDONLY, 0))) {
		perror(argv[0]);
		return(EXIT_FAILURE);
	} else if (-1 == fstat(fd, &st)) {
		perror(argv[0]);
		close(fd);
		return(EXIT_FAILURE);
	} 

	sz = st.st_size;
	map = mmap(NULL, sz, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);

	if (MAP_FAILED == map) {
		perror(argv[0]);
		return(EXIT_FAILURE);
	} else if (NULL != (p = ical_parse(argv[0], map, sz))) {
		if (ICAL_VCALENDAR & p->bits)
			ical_printcomp(p->comps[ICALTYPE_VCALENDAR]);
		if (ICAL_VEVENT & p->bits)
			ical_printcomp(p->comps[ICALTYPE_VEVENT]);
		if (ICAL_VTIMEZONE & p->bits)
			ical_printcomp(p->comps[ICALTYPE_VTIMEZONE]);
#if 0
		if (ICAL_VTODO & p->bits)
			printf(" VTODO");
		if (ICAL_VJOURNAL & p->bits)
			printf(" VJOURNAL");
		if (ICAL_VFREEBUSY & p->bits)
			printf(" VFREEBUSY");
		if (ICAL_VTIMEZONE & p->bits)
			printf(" VTIMEZONE");
		if (ICAL_VALARM & p->bits)
			printf(" VALARM");
		printf("\n");
#endif
		fflush(stdout);
		ical_printfile(STDOUT_FILENO, p);
	}

	munmap(map, sz);
	ical_free(p);
	return(NULL == p ? EXIT_FAILURE : EXIT_SUCCESS);
}
