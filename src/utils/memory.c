/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2019 Sartura Ltd.
 *
 * Author: Juraj Vijtiuk <juraj.vijtiuk@sartura.hr>
 *
 * https://www.sartura.hr/
 */

#include <stdlib.h>
#include <string.h>

#include "memory.h"

void *xmalloc(size_t size)
{
	void *res;

	if (size == 0) {
		abort();
	}

	res = malloc(size);

	if (res == NULL) {
		abort();
	}

	return res;
}

void *xrealloc(void *ptr, size_t size)
{
	void *res;

	if (size == 0) {
		abort();
	}

	res = realloc(ptr, size);

	if (res == NULL) {
		abort();
	}

	return res;
}

void *xcalloc(size_t nmemb, size_t size)
{
	void *res;

	if (size == 0) {
		abort();
	}

	res = calloc(nmemb, size);

	if (res == NULL) {
		abort();
	}

	return res;
}

char *xstrdup(const char *s)
{
	char *res;

	res = strdup(s);

	if (res == NULL) {
		abort();
	}

	return res;
}

char *xstrndup(const char *s, size_t size)
{
	char *res;

	res = strndup(s, size);

	if (res == NULL) {
		abort();
	}

	return res;
}
