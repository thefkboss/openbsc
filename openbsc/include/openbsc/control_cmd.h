#ifndef _CONTROL_CMD_H
#define _CONTROL_CMD_H

#include <osmocom/core/msgb.h>
#include <osmocom/core/talloc.h>
#include <osmocom/core/write_queue.h>

#include <osmocom/vty/vector.h>

#include <openbsc/vty.h>

#define CTRL_CMD_ERROR		-1
#define CTRL_CMD_HANDLED	0
#define CTRL_CMD_REPLY		1

struct ctrl_handle;

enum ctrl_node_type {
	CTRL_NODE_ROOT,	/* Root elements */
	CTRL_NODE_BTS,	/* BTS specific (net.btsN.) */
	CTRL_NODE_TRX,	/* TRX specific (net.btsN.trxM.) */
	CTRL_NODE_TS,	/* TS specific (net.btsN.trxM.tsI.) */
	_LAST_CTRL_NODE
};

enum ctrl_type {
	CTRL_TYPE_UNKNOWN,
	CTRL_TYPE_GET,
	CTRL_TYPE_SET,
	CTRL_TYPE_GET_REPLY,
	CTRL_TYPE_SET_REPLY,
	CTRL_TYPE_TRAP,
	CTRL_TYPE_ERROR
};

struct ctrl_connection {
	struct llist_head list_entry;

	/* The queue for sending data back */
	struct osmo_wqueue write_queue;

	/* Callback if the connection was closed */
	void (*closed_cb)(struct ctrl_connection *conn);

	/* Pending commands for this connection */
	struct llist_head cmds;
};

struct ctrl_cmd {
	struct ctrl_connection *ccon;
	enum ctrl_type type;
	char *id;
	void *node;
	char *variable;
	char *value;
	char *reply;
};

struct ctrl_cmd_struct {
	int nr_commands;
	char **command;
};

struct ctrl_cmd_element {
	const char *name;
	const char *param;
	struct ctrl_cmd_struct strcmd;
	int (*set)(struct ctrl_cmd *cmd, void *data);
	int (*get)(struct ctrl_cmd *cmd, void *data);
	int (*verify)(struct ctrl_cmd *cmd, const char *value, void *data);
};

struct ctrl_cmd_map {
	char *cmd;
	enum ctrl_type type;
};

int ctrl_cmd_exec(vector vline, struct ctrl_cmd *command, vector node, void *data);
int ctrl_cmd_install(enum ctrl_node_type node, struct ctrl_cmd_element *cmd);
int ctrl_cmd_handle(struct ctrl_cmd *cmd, void *data);
int ctrl_cmd_send(struct osmo_wqueue *queue, struct ctrl_cmd *cmd);
int ctrl_cmd_send_to_all(struct ctrl_handle *ctrl, struct ctrl_cmd *cmd);
struct ctrl_cmd *ctrl_cmd_parse(void *ctx, struct msgb *msg);
struct msgb *ctrl_cmd_make(struct ctrl_cmd *cmd);
struct ctrl_cmd *ctrl_cmd_cpy(void *ctx, struct ctrl_cmd *cmd);
struct ctrl_cmd *ctrl_cmd_create(void *ctx, enum ctrl_type);
struct ctrl_cmd *ctrl_cmd_trap(struct ctrl_cmd *cmd);


#define CTRL_HELPER_GET_INT(cmdname, dtype, element) \
static int get_##cmdname(struct ctrl_cmd *cmd, void *_data) \
{ \
	dtype *node = cmd->node; \
	cmd->reply = talloc_asprintf(cmd, "%i", node->element); \
	if (!cmd->reply) { \
		cmd->reply = "OOM"; \
		return CTRL_CMD_ERROR; \
	} \
	return CTRL_CMD_REPLY; \
}
#define CTRL_HELPER_SET_INT(cmdname, dtype, element) \
static int set_##cmdname(struct ctrl_cmd *cmd, void *_data) \
{ \
	dtype *node = cmd->node; \
	int tmp = atoi(cmd->value); \
	node->element = tmp; \
	return get_##cmdname(cmd, _data); \
}
#define CTRL_HELPER_VERIFY_RANGE(cmdname, min, max) \
static int verify_##cmdname(struct ctrl_cmd *cmd, const char *value, void *_data) \
{ \
	int tmp = atoi(value); \
	if ((tmp >= min)&&(tmp <= max)) { \
		return 0; \
	} \
	cmd->reply = "Input not within the range"; \
	return -1; \
}

#define CTRL_CMD_DEFINE_RANGE(cmdname, cmdstr, dtype, element, min, max) \
	CTRL_HELPER_GET_INT(cmdname, dtype, element) \
	CTRL_HELPER_SET_INT(cmdname, dtype, element) \
	CTRL_HELPER_VERIFY_RANGE(cmdname, min, max) \
static struct ctrl_cmd_element cmd_##cmdname = { \
	.name = cmdstr, \
	.param = NULL, \
	.get = &get_##cmdname, \
	.set = &set_##cmdname, \
	.verify = &verify_##cmdname, \
}

#define CTRL_HELPER_GET_STRING(cmdname, dtype, element) \
static int get_##cmdname(struct ctrl_cmd *cmd, void *_data) \
{ \
	dtype *data = cmd->node; \
	cmd->reply = talloc_asprintf(cmd, "%s", data->element); \
	if (!cmd->reply) { \
		cmd->reply = "OOM"; \
		return CTRL_CMD_ERROR; \
	} \
	return CTRL_CMD_REPLY; \
}
#define CTRL_HELPER_SET_STRING(cmdname, dtype, element) \
static int set_##cmdname(struct ctrl_cmd *cmd, void *_data) \
{ \
	dtype *data = cmd->node; \
	bsc_replace_string(cmd->node, &data->element, cmd->value); \
	return get_##cmdname(cmd, _data); \
}
#define CTRL_CMD_DEFINE_STRING(cmdname, cmdstr, dtype, element) \
	CTRL_HELPER_GET_STRING(cmdname, dtype, element) \
	CTRL_HELPER_SET_STRING(cmdname, dtype, element) \
static struct ctrl_cmd_element cmd_##cmdname = { \
	.name = cmdstr, \
	.param = NULL, \
	.get = &get_##cmdname, \
	.set = &set_##cmdname, \
	.verify = NULL, \
}

#define CTRL_CMD_RAW(cmdname, cmdstr, cmdget, cmdset, cmdverify) \
static struct ctrl_cmd_element cmd_##cmdname = { \
	.name = cmdstr, \
	.param = NULL, \
	.get = cmdget, \
	.set = cmdset, \
	.verify = cmdverify, \
}

#define CTRL_CMD_DEFINE(cmdname, cmdstr) \
static int get_##cmdname(struct ctrl_cmd *cmd, void *data); \
static int set_##cmdname(struct ctrl_cmd *cmd, void *data); \
static int verify_##cmdname(struct ctrl_cmd *cmd, const char *value, void *data); \
	CTRL_CMD_RAW(cmdname, cmdstr, get_##cmdname, set_##cmdname, verify_##cmdname)
struct gsm_network;

#endif /* _CONTROL_CMD_H */
