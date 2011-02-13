#ifndef OPENBSC_ABIS_OM2K_H
#define OPENBSC_ABIS_OM2K_H
/* Ericsson RBS 2xxx GSM O&M (OM2000) messages on the A-bis interface
 * implemented based on protocol trace analysis, no formal documentation */

/* (C) 2010-2011 by Harald Welte <laforge@gnumonks.org>
 *
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

struct abis_om2k_mo {
	uint8_t class;
	uint8_t bts;
	uint8_t assoc_so;
	uint8_t inst;
} __attribute__ ((packed));

struct om2k_is_conn_grp {
	uint16_t icp1;
	uint16_t icp2;
	uint8_t cont_idx;
} __attribute__ ((packed));

extern const struct value_string om2k_mo_class_short_vals[];

int abis_om2k_rcvmsg(struct msgb *msg);

extern const struct abis_om2k_mo om2k_mo_cf;

int abis_om2k_tx_reset_cmd(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_start_req(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_status_req(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_connect_cmd(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_disconnect_cmd(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_enable_req(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_disable_req(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_test_req(struct gsm_bts *bts, const struct abis_om2k_mo *mo);
int abis_om2k_tx_op_info(struct gsm_bts *bts, const struct abis_om2k_mo *mo,
			 uint8_t operational);
int abis_om2k_tx_is_conf_req(struct gsm_bts *bts, struct om2k_is_conn_grp *cg,
			     unsigned int num_cg);

int abis_om2k_vty_init(void);

#endif /* OPENBCS_ABIS_OM2K_H */