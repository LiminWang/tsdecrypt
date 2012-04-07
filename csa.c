/*
 * CSA functions
 * Copyright (C) 2011-2012 Unix Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/time.h>

#include <dvbcsa/dvbcsa.h>

#include "libfuncs/libfuncs.h"

/* The following routine is taken from benchbitslice in libdvbcsa */
void csa_benchmark(void) {
	struct timeval t0, t1;
	struct dvbcsa_bs_key_s *key = dvbcsa_bs_key_alloc();
	unsigned int n, i, npackets = 0;
	unsigned int batch_size = dvbcsa_bs_batch_size();
	uint8_t data[batch_size + 1][188];
	struct dvbcsa_bs_batch_s pcks[batch_size + 1];
	uint8_t cw[8] = { 0x12, 0x34, 0x56, 0x78, 0x89, 0xab, 0xcd, 0xef, };

	srand(time(0));
	puts("Single threaded CSA decoding benchmark");

	dvbcsa_bs_key_set (cw, key);

	printf("Batch size %d packets.\n\n", batch_size);
	for (i = 0; i < batch_size; i++) {
		pcks[i].data = data[i];
		pcks[i].len = 184;
		memset(data[i], rand(), pcks[i].len);
	}
	pcks[i].data = NULL;

	gettimeofday(&t0, NULL);
	for (n = (1 << 12) / batch_size; n < (1 << 19) / batch_size; n *= 2) {
		printf(" Decrypting %6u mpegts packets\r", n * batch_size);
		fflush(stdout);
		for (i = 0; i < n; i++) {
			dvbcsa_bs_decrypt(key, pcks, 184);
		}
		npackets += n * batch_size;
	}
	gettimeofday(&t1, NULL);

	unsigned long long usec = timeval_diff_usec(&t0, &t1);
	printf("DONE: %u packets (%u bytes) decrypted in %llu ms = %.1f Mbits/s\n\n",
		npackets,
		npackets * 188,
		usec / 1000,
		(double)(npackets * 188 * 8) / (double)usec
	);

	dvbcsa_bs_key_free(key);
}
