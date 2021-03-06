/* Copyright (C) 2007-2013 B.A.T.M.A.N. contributors:
 *
 * Marek Lindner, Simon Wunderlich
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _NET_BATMAN_ADV_ORIGINATOR_H_
#define _NET_BATMAN_ADV_ORIGINATOR_H_

#include "hash.h"

int batadv_originator_init(struct batadv_priv *bat_priv);
void batadv_originator_free(struct batadv_priv *bat_priv);
void batadv_purge_orig_ref(struct batadv_priv *bat_priv);
void batadv_orig_node_free_ref(struct batadv_orig_node *orig_node);
void batadv_orig_node_free_ref_now(struct batadv_orig_node *orig_node);
struct batadv_orig_node *batadv_get_orig_node(struct batadv_priv *bat_priv,
					      const uint8_t *addr);
struct batadv_neigh_node *
batadv_neigh_node_new(struct batadv_hard_iface *hard_iface,
		      const uint8_t *neigh_addr, uint32_t seqno);
void batadv_neigh_node_free_ref(struct batadv_neigh_node *neigh_node);
struct batadv_neigh_node *
batadv_orig_node_get_router(struct batadv_orig_node *orig_node);
int batadv_orig_seq_print_text(struct seq_file *seq, void *offset);
int batadv_orig_hash_add_if(struct batadv_hard_iface *hard_iface,
			    int max_if_num);
int batadv_orig_hash_del_if(struct batadv_hard_iface *hard_iface,
			    int max_if_num);

/* hashfunction to choose an entry in a hash table of given size
 * hash algorithm from http://en.wikipedia.org/wiki/Hash_table
 */
static inline uint32_t batadv_choose_orig(const void *data, uint32_t size)
{
	const unsigned char *key = data;
	uint32_t hash = 0;
	size_t i;

	for (i = 0; i < 6; i++) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash % size;
}

static inline struct batadv_orig_node *
batadv_orig_hash_find(struct batadv_priv *bat_priv, const void *data)
{
	struct batadv_hashtable *hash = bat_priv->orig_hash;
	struct hlist_head *head;
	struct batadv_orig_node *orig_node, *orig_node_tmp = NULL;
	int index;

	if (!hash)
		return NULL;

	index = batadv_choose_orig(data, hash->size);
	head = &hash->table[index];

	rcu_read_lock();
	hlist_for_each_entry_rcu(orig_node, head, hash_entry) {
		if (!batadv_compare_eth(orig_node, data))
			continue;

		if (!atomic_inc_not_zero(&orig_node->refcount))
			continue;

		orig_node_tmp = orig_node;
		break;
	}
	rcu_read_unlock();

	return orig_node_tmp;
}

#endif /* _NET_BATMAN_ADV_ORIGINATOR_H_ */
