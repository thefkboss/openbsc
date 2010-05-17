#ifndef _GPRS_SGSN_H
#define _GPRS_SGSN_H

#include <stdint.h>
#include <netinet/in.h>

#include <osmocore/gsm48.h>

#define GSM_IMSI_LENGTH 17
#define GSM_IMEI_LENGTH 17
#define GSM_EXTENSION_LENGTH 15

/* TS 04.08 4.1.3.3 GMM mobility management states on the network side */
enum gprs_mm_state {
	GMM_DEREGISTERED,		/* 4.1.3.3.1.1 */
	GMM_COMMON_PROC_INIT,		/* 4.1.3.3.1.2 */
	GMM_REGISTERED_NORMAL,		/* 4.1.3.3.2.1 */
	GMM_REGISTERED_SUSPENDED,	/* 4.1.3.3.2.2 */
	GMM_DEREGISTERED_INIT,		/* 4.1.3.3.1.4 */
};

enum gprs_ciph_algo {
	GPRS_ALGO_GEA0,
	GPRS_ALGO_GEA1,
	GPRS_ALGO_GEA2,
};

#define MS_RADIO_ACCESS_CAPA

/* According to TS 03.60, Table 5: SGSN MM and PDP Contexts */
/* Extended by 3GPP TS 23.060, Table 6: SGSN MM and PDP Contexts */
struct sgsn_mm_ctx {
	struct llist_head	list;

	char 			imsi[GSM_IMSI_LENGTH];
	enum gprs_mm_state	mm_state;
	uint32_t 		p_tmsi;
	uint32_t 		p_tmsi_sig;
	char 			imei[GSM_IMEI_LENGTH];
	/* Opt: Software Version Numbber / TS 23.195 */
	char 			msisdn[GSM_EXTENSION_LENGTH];
	struct gprs_ra_id	ra;
	uint16_t		cell_id;
	uint32_t		cell_id_age;
	uint16_t		sac;	/* Iu: Service Area Code */
	uint32_t		sac_age;/* Iu: Service Area Code age */
	/* VLR number */
	uint32_t		new_sgsn_addr;
	/* Authentication Triplets */
	/* Kc */
	/* Iu: CK, IK, KSI */
	/* CKSN */
	enum gprs_ciph_algo	ciph_algo;
	struct {
		uint8_t	buf[14];	/* 10.5.5.12a */
		uint8_t	len;
	} ms_radio_access_capa;
	struct {
		uint8_t	buf[4];		/* 10.5.5.12 */
		uint8_t	len;
	} ms_network_capa;
	uint16_t		drx_parms;
	int			mnrg;	/* MS reported to HLR? */
	int			ngaf;	/* MS reported to MSC/VLR? */
	int			ppf;	/* paging for GPRS + non-GPRS? */
	/* SMS Parameters */
	int			recovery;
	uint8_t			radio_prio_sms;

	struct llist_head	pdp_list;

	/* Additional bits not present in the GSM TS */
	uint32_t		tlli;
	struct timer_list	timer;
	unsigned int		T;
};

/* look-up a SGSN MM context based on TLLI + RAI */
struct sgsn_mm_ctx *sgsn_mm_ctx_by_tlli(uint32_t tlli,
					const struct gprs_ra_id *raid);
struct sgsn_mm_ctx *sgsn_mm_ctx_by_ptmsi(uint32_t tmsi);
struct sgsn_mm_ctx *sgsn_mm_ctx_by_imsi(const char *imsi);

/* Allocate a new SGSN MM context */
struct sgsn_mm_ctx *sgsn_mm_ctx_alloc(uint32_t tlli,
					const struct gprs_ra_id *raid);


enum pdp_ctx_state {
	PDP_STATE_NONE,
};

enum pdp_type {
	PDP_TYPE_NONE,
};

struct sgsn_pdp_ctx {
	struct llist_head	list;	/* list_head for mmctx->pdp_list */
	struct llist_head	g_list;	/* list_head for global list */
	struct sgsn_mm_ctx	*mm;	/* back pointer to MM CTX */
	struct sgsn_ggsn_ctx	*ggsn;	/* which GGSN serves this PDP */

	//unsigned int		id;
	struct pdp_t		*lib;	/* pointer to libgtp PDP ctx */
	enum pdp_ctx_state	state;
	enum pdp_type		type;
	uint32_t		address;
	char 			*apn_subscribed;
	//char 			*apn_used;
	uint16_t		nsapi;
	uint8_t			ti;	/* transaction identifier */
	int			vplmn_allowed;
	uint32_t		qos_profile_subscr;
	//uint32_t		qos_profile_req;
	//uint32_t		qos_profile_neg;
	uint8_t			radio_prio;
	uint32_t		tx_npdu_nr;
	uint32_t		rx_npdu_nr;
	uint32_t		tx_gtp_snd;
	uint32_t		rx_gtp_snu;
	//uint32_t		charging_id;
	int			reordering_reqd;
};


struct sgsn_pdp_ctx *sgsn_pdp_ctx_by_nsapi(const struct sgsn_mm_ctx *mm,
					   uint8_t nsapi);
struct sgsn_pdp_ctx *sgsn_pdp_ctx_alloc(struct sgsn_mm_ctx *mm,
					uint8_t nsapi);
void sgsn_pdp_ctx_free(struct sgsn_pdp_ctx *pdp);


struct ggsn_ctx {
	struct llist_head list;
	uint32_t id;
	unsigned int gtp_version;
	struct in_addr remote_addr;
	struct gsn_t *gsn;
};
struct ggsn_ctx *ggsn_ctx_alloc(uint32_t id);
struct ggsn_ctx *ggsn_ctx_by_id(uint32_t id);
struct ggsn_ctx *ggsn_ctx_find_alloc(uint32_t id);

struct apn_ctx {
	struct llist_head list;
	struct ggsn_ctx *ggsn;
	char *name;
	char *description;
};

extern struct llist_head sgsn_mm_ctxts;
extern struct llist_head sgsn_ggsn_ctxts;
extern struct llist_head sgsn_apn_ctxts;
extern struct llist_head sgsn_pdp_ctxts;

#endif /* _GPRS_SGSN_H */