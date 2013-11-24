#include "list.h"
#include "alloc.h"

DEFINE_ALLOCATOR(list_node, struct list_node, ALLOC_DEFAULT_BLOB_SIZE);

static inline void list_link(struct list_node *prev, struct list_node *next)
{
    prev->next = next;
    next->prev = prev;
}

void destroy_list(struct list *list)
{
    struct list_node *node, *tmp;

    list_for_each_node_safe(node, tmp, list)
        free_list_node(node);
}

struct list_node *list_insert(void *ptr, struct list_node *next)
{
    struct list_node *node = alloc_list_node();

    node->elem = ptr;
    list_link(next->prev, node);
    list_link(node, next);
    return node;
}

struct list_node *list_remove(struct list_node *node)
{
    struct list_node *next = node->next;

    list_link(node->prev, next);
    free_list_node(node);
    return next;
}

void list_remove_elem(void *ptr, struct list *list)
{
    struct list_node *node;

    list_for_each_node(node, list) {
        if (node->elem == ptr) {
            list_remove(node);
            break;
        }
    }
}

void list_move(struct list_node *node, struct list_node *next)
{
    list_link(node->prev, node->next);
    list_link(next->prev, node);
    list_link(node, next);
}

struct list_node *list_find(void *ptr, struct list *list)
{
    struct list_node *node;

    list_for_each_node(node, list) {
        if (node->elem == ptr)
            break;
    }

    return node;
}
