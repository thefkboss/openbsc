/* Generic signalling/notification infrastructure */
/* (C) 2009 by Holger Hans Peter Freyther <zecke@selfish.org>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef OPENBSC_SIGNAL_H
#define OPENBSC_SIGNAL_H

#include <openbsc/gsm_data.h>
#include <openbsc/gsm_subscriber.h>


/*
 * Signalling areas
 */
#define S_PAGING	0x0001
#define S_SMS		0x0002


struct signal_data {
	int area;
};


struct paging_signal_data {
	struct signal_data data;

	struct gsm_subscriber *subscr;
	struct gsm_bts *bts;

	/* NULL in case the paging didn't work */
	struct gsm_lchan *lchan;
};

struct sms_signal_data {
	struct signal_data data;

	struct sms_submit *sms;
};


/* Management */
void register_signal_handler(int areas, int (*sig)(struct signal_data *, void *data), void *data);
void remove_signal_handler(int areas, int (*sig)(struct signal_data *, void *data), void *data);

/* Dispatch */
void dispatch_signal(struct signal_data *data);


#endif
