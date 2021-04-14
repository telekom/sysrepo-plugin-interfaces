/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2019 Sartura Ltd.
 *
 * Author: Juraj Vijtiuk <juraj.vijtiuk@sartura.hr>
 *
 * https://www.sartura.hr/
 */

#ifndef MEMORY_H_ONCE
#define MEMORY_H_ONCE

#include <stdlib.h>

#define FREE_SAFE(x) \
	do {             \
		free(x);     \
		(x) = NULL;  \
	} while (0)

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nmemb, size_t size);
char *xstrdup(const char *s);
char *xstrndup(const char *s, size_t size);

#endif /* MEMORY_H_ONCE */
