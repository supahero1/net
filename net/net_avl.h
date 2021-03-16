/*
   Copyright 2021 shädam

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef lu_eoVE_9ZX_B__QSYe_uZ2d_kgw_b_f
#define lu_eoVE_9ZX_B__QSYe_uZ2d_kgw_b_f 1

#ifdef __cplusplus
extern "C" {
#endif

#include "../def.h"
#include "net_base.h"

#include <stdint.h>

struct net_avl_node {
  struct net_avl_node* restrict parent;
  struct net_avl_node* restrict left;
  struct net_avl_node* restrict right;
  struct NETSocket socket;
  int balance;
};

struct net_avl_tree {
  struct net_avl_node** parts;
  struct net_avl_node* head;
  struct net_avl_node* last;
  uint32_t count;
  uint32_t amount;
  uint32_t max_items_per_part;
};

__const
extern struct net_avl_tree net_avl_tree(const uint32_t);

__nonnull((1))
extern int net_avl_init(struct net_avl_tree* const);

__nonnull((1))
extern void net_avl_free(struct net_avl_tree* const);

__nonnull((1))
extern int net_avl_insert(struct net_avl_tree* const, const struct NETSocket);

__nonnull((1))
extern struct NETSocket* net_avl_search(struct net_avl_tree* const, const int);

__nonnull((1))
extern void net_avl_delete(struct net_avl_tree* const, const int);

#ifdef __cplusplus
}
#endif

#endif // lu_eoVE_9ZX_B__QSYe_uZ2d_kgw_b_f