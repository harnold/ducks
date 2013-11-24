#ifndef ELIST_H
#define ELIST_H

#include <stdbool.h>

struct elist_node {
    struct elist_node *next;
    struct elist_node *prev;
};

struct elist {
    struct elist_node end;
};

void init_elist(struct elist *list);
struct elist_node *elist_begin(struct elist *list);
struct elist_node *elist_end(struct elist *list);
bool elist_empty(struct elist *list);
void elist_insert(struct elist_node *node, struct elist_node *next);
void elist_remove(struct elist_node *node);
void elist_move(struct elist_node *node, struct elist_node *next);
void elist_splice(struct elist_node *begin, struct elist_node *end,
                  struct elist_node *next);

#define elist_get(node, type, member) \
    (type *) (((char *) (node)) - offsetof((type), (member)))

#define elist_for_each_node(node, list) \
    for ((node) = elist_begin(list); \
         (node) != elist_end(list); \
         (node) = (node)->next)

#define elist_for_each_node_reverse(node, list) \
    for ((node) = elist_end(list)->prev; \
         (node) != elist_end(list); \
         (node) = (node)->prev)

#define elist_for_each_node_safe(node, tmp, list) \
    for ((node) = elist_begin(list), (tmp) = (node)->next; \
         (node) != elist_end(list); \
         (node) = (tmp), (tmp) = (node)->next)

#define elist_for_each_elem(elem, list, type, member) \
    for ((elem) = elist_get(elist_begin(list), (type), (member)); \
         &(elem)->(member) != elist_end(list); \
         (elem) = elist_get((elem)->(member).next, (type), (member)))

#define elist_for_each_elem_reverse(elem, list, type, member) \
    for ((elem) = elist_get(elist_end(list)->prev, (type), (member)); \
         &(elem)->(member) != elist_end(list); \
         (elem) = elist_get((elem)->(member).prev, (type), (member)))

static inline void elist_link(struct elist_node *prev, struct elist_node *next)
{
    prev->next = next;
    next->prev = prev;
}

static inline void init_elist(struct elist *list)
{
    list->end.next = &list->end;
    list->end.prev = &list->end;
}

static inline struct elist_node *elist_begin(struct elist *list)
{
    return list->end.next;
}

static inline struct elist_node *elist_end(struct elist *list)
{
    return &list->end;
}

static inline bool elist_empty(struct elist *list)
{
    return elist_begin(list) == elist_end(list);
}

static inline void elist_insert(struct elist_node *node, struct elist_node *next)
{
    elist_link(next->prev, node);
    elist_link(node, next);
}

static inline void elist_remove(struct elist_node *node)
{
    elist_link(node->prev, node->next);
    node->next = node;
    node->prev = node;
}

static inline void elist_move(struct elist_node *node, struct elist_node *next)
{
    elist_link(node->prev, node->next);
    elist_insert(node, next);
}

void elist_splice(struct elist_node *begin, struct elist_node *end,
                  struct elist_node *next)
{
    elist_link(end->prev, next);
    elist_link(begin->prev, end);
    elist_link(next->prev, begin);
}

#endif
