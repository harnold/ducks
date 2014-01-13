#ifndef LIST_H
#define LIST_H

struct list_node_base {
    struct list_node *next;
    struct list_node *prev;
};

struct list_node {
    struct list_node *next;
    struct list_node *prev;
    void *elem;
};

struct list {
    struct list_node_base end;
};

static inline void init_list(struct list *list);
void destroy_list(struct list *list);
static inline struct list_node *list_begin(struct list *list);
static inline struct list_node *list_end(struct list *list);
static inline void *list_front(struct list *list);
static inline void *list_back(struct list *list);
struct list_node *list_insert(void *ptr, struct list_node *next);
static inline struct list_node *list_insert_front(void *ptr, struct list *list);
static inline struct list_node *list_insert_back(void *ptr, struct list *list);
struct list_node *list_remove(struct list_node *node);
static inline void list_remove_front(struct list *list);
static inline void list_remove_back(struct list *list);
void list_remove_elem(void *ptr, struct list *list);
void list_move(struct list_node *node, struct list_node *next);
static inline void list_move_front(struct list_node *node, struct list *list);
static inline void list_move_back(struct list_node *node, struct list *list);
struct list_node *list_find(void *ptr, struct list *list);

#define list_for_each_node(__node, __list) \
    for ((__node) = list_begin(__list); \
         (__node) != list_end(__list); \
         (__node) = (__node)->next)

#define list_for_each_node_safe(__node, tmp, __list) \
    for ((__node) = list_begin(__list), (tmp) = (__node)->next; \
         (__node) != list_end(__list); \
         (__node) = (tmp), (tmp) = (__node)->next)

#define list_for_each_node_reverse(__node, __list) \
    for ((__node) = list_end(__list)->prev; \
         (__node) != list_end(__list); \
         (__node) = (__node)->prev)

static inline void init_list(struct list *list)
{
    list->end.next = (struct list_node *) &list->end;
    list->end.prev = (struct list_node *) &list->end;
}

static inline struct list_node *list_begin(struct list *list)
{
    return list->end.next;
}

static inline struct list_node *list_end(struct list *list)
{
    return (struct list_node *) &list->end;
}

static inline void *list_front(struct list *list)
{
    return list_begin(list)->elem;
}

static inline void *list_back(struct list *list)
{
    return list_end(list)->prev->elem;
}

static inline struct list_node *list_insert_front(void *ptr, struct list *list)
{
    return list_insert(ptr, list_begin(list));
}

static inline struct list_node *list_insert_back(void *ptr, struct list *list)
{
    return list_insert(ptr, list_end(list));
}

static inline void list_remove_front(struct list *list)
{
    list_remove(list_begin(list));
}

static inline void list_remove_back(struct list *list)
{
    list_remove(list_end(list)->prev);
}

static inline void list_move_front(struct list_node *node, struct list *list)
{
    list_move(node, list_begin(list));
}

static inline void list_move_back(struct list_node *node, struct list *list)
{
    list_move(node, list_end(list));
}

#endif
