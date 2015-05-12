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
#ifndef EXTERN_H
#define EXTERN_H

#include "libkcaldav.h"

/*
 * Algorithm for HTTP digest.
 */
enum	httpalg {
	HTTPALG_MD5 = 0,
	HTTPALG_MD5_SESS,
	HTTPALG__MAX
};

/*
 * Quality of protection (QOP) for HTTP digest.
 */
enum	httpqop {
	HTTPQOP_NONE = 0,
	HTTPQOP_AUTH,
	HTTPQOP_AUTH_INT,
	HTTPQOP__MAX
};

/*
 * Parsed HTTP ``Authorization'' header (RFC 2617).
 * These are just copied over from kcgi's values.
 */
#define	KREALM		"kcaldav"
struct	httpauth {
	enum httpalg	 alg;
	enum httpqop	 qop;
	const char	*user;
	const char	*uri;
	const char	*realm;
	const char	*nonce;
	const char	*cnonce;
	const char	*response;
	size_t		 count;
	const char	*opaque;
	const char	*req;
	size_t		 reqsz;
	const char	*method;
};

struct	res {
	char		*data;
	struct ical	*ical;
	int64_t		 etag;
	char		*url;
	int64_t		 collection;
	int64_t		 id;
};

struct	coln {
	char		*url; /* name of collection */
	char		*displayname; /* displayname of collection */
	char		*colour; /* colour (RGBA) */
	char		*description; /* free-form description */
	int64_t		 ctag; /* collection tag */
	int64_t		 id; /* unique identifier */
};

/*
 * A principal is a user of the system.
 * The HTTP authorised user (struct httpauth) is matched against a list
 * of principals when the password file is read.
 */
struct	prncpl {
	char		*name; /* username */
	char		*hash; /* MD5 of name, realm, and password */
	uint64_t	 quota_used; /* quota (VFS) */
	uint64_t	 quota_avail; /* quota (VFS) */
	char		*email; /* email address */
	struct coln	*cols; /* owned collections */
	size_t		 colsz; /* number of owned collections */
	int64_t		 id; /* unique identifier */
};

/*
 * Return codes for nonce operations.
 */
enum	nonceerr {
	NONCE_ERR, /* generic error */
	NONCE_NOTFOUND, /* nonce entry not found */
	NONCE_REPLAY, /* replay attack detected! */
	NONCE_OK /* nonce checks out */
};

__BEGIN_DECLS

int		  db_collection_load(struct coln **, const char *, int64_t);
int		  db_collection_new(const char *, int64_t);
int		  db_collection_remove(int64_t);
int		  db_collection_resources(void (*)(const struct res *, void *), int64_t, void *);
int		  db_collection_update(const struct coln *);
int		  db_init(const char *, int);
int		  db_nonce_new(char **);
enum nonceerr	  db_nonce_update(const char *, size_t);
enum nonceerr	  db_nonce_validate(const char *, size_t);
int		  db_owner_check_or_set(int64_t);
int		  db_prncpl_load(struct prncpl **, const char *);
int		  db_prncpl_new(const char *, const char *, const char *, const char *);
int		  db_prncpl_update(const struct prncpl *);
int		  db_resource_delete(const char *, int64_t, int64_t);
int		  db_resource_remove(const char *, int64_t);
int		  db_resource_load(struct res **, const char *, int64_t);
int		  db_resource_new(const char *, const char *, int64_t);
int		  db_resource_update(const char *, const char *, int64_t, int64_t);

void		  prncpl_free(struct prncpl *);

void		  res_free(struct res *);

__END_DECLS

#endif
