#include <haproxy/guid.h>

#include <import/ebistree.h>
#include <haproxy/listener-t.h>
#include <haproxy/obj_type.h>
#include <haproxy/proxy.h>
#include <haproxy/server-t.h>
#include <haproxy/tools.h>

/* GUID global tree */
struct eb_root guid_tree = EB_ROOT_UNIQUE;

/* Initialize <guid> members. */
void guid_init(struct guid_node *guid)
{
	guid->node.key = NULL;
	guid->node.node.leaf_p = NULL;
}

/* Insert <objt> into GUID global tree with key <uid>. Must only be called on
 * thread isolation. On failure, <errmsg> will be allocated with an error
 * description. Caller is responsible to free it.
 *
 * Returns 0 on success else non-zero.
 */
int guid_insert(enum obj_type *objt, const char *uid, char **errmsg)
{
	struct guid_node *guid = NULL;
	struct guid_node *dup;
	struct ebpt_node *node;
	char *key = NULL;
	char *dup_name = NULL;
	const char *c;

	if (strlen(uid) > GUID_MAX_LEN) {
		memprintf(errmsg, "UID too big");
		goto err;
	}

	c = invalid_char(uid);
	if (c) {
		memprintf(errmsg, "invalid character '%c'", c[0]);
		goto err;
	}

	switch (obj_type(objt)) {
	case OBJ_TYPE_PROXY:
		guid = &__objt_proxy(objt)->guid;
		break;

	case OBJ_TYPE_LISTENER:
		guid = &__objt_listener(objt)->guid;
		break;

	case OBJ_TYPE_SERVER:
		guid = &__objt_server(objt)->guid;
		break;

	default:
		/* No guid support for this objtype. */
		ABORT_NOW();
		return 0;
	}

	key = strdup(uid);
	if (!key) {
		memprintf(errmsg, "key alloc failure");
		goto err;
	}

	guid->node.key = key;
	node = ebis_insert(&guid_tree, &guid->node);
	if (node != &guid->node) {
		dup = ebpt_entry(node, struct guid_node, node);
		dup_name = guid_name(dup);
		memprintf(errmsg, "duplicate entry with %s", dup_name);
		goto err;
	}

	guid->obj_type = objt;
	return 0;

 err:
	ha_free(&key);
	ha_free(&dup_name);
	return 1;
}

/* Remove <guid> node from GUID global tree. Must only be called on thread
 * isolation. Safe to call even if node is not currently stored.
 */
void guid_remove(struct guid_node *guid)
{
	ebpt_delete(&guid->node);
	ha_free(&guid->node.key);
}

/* Retrieve an instance from GUID global tree with key <uid>.
 *
 * Returns the GUID instance or NULL if key not found.
 */
struct guid_node *guid_lookup(const char *uid)
{
	struct ebpt_node *node = NULL;
	struct guid_node *guid = NULL;

	node = ebis_lookup(&guid_tree, uid);
	if (node)
		guid = ebpt_entry(node, struct guid_node, node);

	return guid;
}

/* Generate a user-friendly description for the instance attached via <guid>
 * node. The string is dynamically allocated and the caller is responsible to
 * free it.
 *
 * Returns a pointer to the dynamically allocated message.
 */
char *guid_name(const struct guid_node *guid)
{
	char *msg = NULL;
	struct proxy *px;
	struct listener *l;
	struct server *srv;

	switch (obj_type(guid->obj_type)) {
	case OBJ_TYPE_PROXY:
		px = __objt_proxy(guid->obj_type);
		return memprintf(&msg, "%s %s", proxy_cap_str(px->cap), px->id);

	case OBJ_TYPE_LISTENER:
		l = __objt_listener(guid->obj_type);
		return memprintf(&msg, "listener %s (%s:%d)",
		                 l->bind_conf->frontend->id,
		                 l->bind_conf->file, l->bind_conf->line);

	case OBJ_TYPE_SERVER:
		srv = __objt_server(guid->obj_type);
		return memprintf(&msg, "server %s/%s", srv->proxy->id, srv->id);

	default:
		break;
	}

	return NULL;
}
