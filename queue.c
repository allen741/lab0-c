#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*Be used in function q_merge*/
#define add_to_first_queue(opera)                            \
    struct list_head *ptr_prev = first_q_iterator->prev;     \
    queue_value = list_entry(queue, element_t, list)->value; \
    for (struct list_head *ptr = first_q_iterator;;          \
         ptr_prev = ptr, ptr = ptr->next) {                  \
        if (ptr == first_queue) {                            \
            queue_next = queue->next;                        \
            list_add(queue, ptr_prev);                       \
            first_q_iterator = ptr_prev->next;               \
            size = size + 1;                                 \
            break;                                           \
        }                                                    \
        ptr_value = list_entry(ptr, element_t, list)->value; \
        if (ptr_value && queue_value &&                      \
            strcmp(ptr_value, queue_value) opera 0) {        \
            queue_next = queue->next;                        \
            list_add(queue, ptr_prev);                       \
            first_q_iterator = ptr_prev->next;               \
            size = size + 1;                                 \
            break;                                           \
        }                                                    \
    }

#define merge_two_list(opera)                                                  \
    struct list_head **merge_head_ptr = &merge_head;                           \
    while (true) {                                                             \
        if (strcmp(list_entry(merge_last, element_t, list)->value,             \
                   list_entry(merge_first, element_t, list)->value) opera 0) { \
            *merge_target = merge_last;                                        \
            *merge_head_ptr = merge_last;                                      \
            merge_target = &merge_last->next;                                  \
            merge_head_ptr = &merge_last->next;                                \
            merge_last = merge_last->next;                                     \
            if (!merge_last) {                                                 \
                *merge_target = merge_first;                                   \
                break;                                                         \
            }                                                                  \
        } else {                                                               \
            *merge_target = merge_first;                                       \
            *merge_head_ptr = merge_first;                                     \
            merge_target = &merge_first->next;                                 \
            merge_head_ptr = &merge_first->next;                               \
            merge_first = merge_first->next;                                   \
            if (!merge_first) {                                                \
                *merge_target = merge_last;                                    \
                break;                                                         \
            }                                                                  \
        }                                                                      \
    }

static inline element_t *e_new(char *s)
{
    if (!s) {
        return NULL;
    }

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element) {
        return NULL;
    }
    INIT_LIST_HEAD(&new_element->list);

    size_t len = strlen(s) + 1;
    new_element->value = malloc(len);
    if (!new_element->value) {
        free(new_element);
        return NULL;
    }
    memcpy(new_element->value, s, len);

    return new_element;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *queue = malloc(sizeof(struct list_head));

    if (!queue) {
        return NULL;
    }

    INIT_LIST_HEAD(queue);

    return queue;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }
    if (list_empty(head)) {
        free(head);
        return;
    }
    struct list_head *pos, *pos_next;

    for (pos = head->next, pos_next = pos->next; pos != head && pos != NULL;
         pos = pos_next, pos_next = pos_next->next) {
        free(list_entry(pos, element_t, list)->value);
        free(list_entry(pos, element_t, list));
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *new_element = e_new(s);

    if (!new_element) {
        return false;
    }

    list_add(&(new_element->list), head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *new_element = e_new(s);

    if (!new_element) {
        return false;
    }

    list_add_tail(&(new_element->list), head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    element_t *entry = list_entry(head->next, element_t, list);
    list_del(head->next);

    if (sp) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *entry = list_entry(head->prev, element_t, list);
    list_del(head->prev);

    if (sp) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    int len = 0;
    struct list_head *li;

    for (li = head->next; li != head && li != NULL; li = li->next) {
        len++;
    }
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }
    if (head->next == head->prev) {
        free(list_entry(head->next, element_t, list)->value);
        free(list_entry(head->next, element_t, list));
        head->next = head;
        head->prev = head;
        return true;
    }
    struct list_head *fast, *slow;
    for (fast = head->next, slow = head->next;
         fast != head && fast->next != head;
         fast = fast->next->next, slow = slow->next) {
    }

    list_del(slow);
    free(list_entry(slow, element_t, list)->value);
    free(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head) {
        return false;
    }
    if (head->next == head->prev) {
        return true;
    }
    struct list_head *pos, *pos_next;
    for (pos = head->next; pos != head; pos = pos->next) {
        for (pos_next = pos->next; pos_next != head;) {
            if (strcmp(list_entry(pos, element_t, list)->value,
                       list_entry(pos_next, element_t, list)->value) == 0) {
                struct list_head *to_delete = pos_next;
                pos_next = pos_next->next;
                list_del(to_delete);
                free(list_entry(to_delete, element_t, list)->value);
                free(list_entry(to_delete, element_t, list));
                continue;
            }
            pos_next = pos_next->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head) {
        return;
    }
    struct list_head *first, *second;
    for (first = head->next, second = first->next;
         first != head && second != head;
         first = first->next, second = first->next) {
        list_move(first, second);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head->prev) {
        return;
    }
    struct list_head *tmp;
    for (struct list_head *next_ptr = head->next->next; next_ptr != head;
         next_ptr = tmp) {
        tmp = next_ptr->next;
        list_move(next_ptr, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || k == 1) {
        return;
    }
    for (struct list_head *group_first = head;;) {
        struct list_head *group_last = group_first;
        int check = 0;
        for (int i = 0; i < k; i++) {
            group_last = group_last->next;
            if (group_last == head) {
                check = 1;
                break;
            }
        }
        if (check == 1) {
            break;
        }
        struct list_head *tmp_next = group_last->next;
        struct list_head *tmp_prev = group_first->prev;
        group_last->next = group_first;
        group_first->prev = group_last;

        q_reverse(group_first);

        tmp_next->prev = group_first->prev;
        group_first->prev->next = tmp_next;
        struct list_head *tmp = group_first->prev;
        group_first->prev = tmp_prev;
        group_first = tmp;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next == head->prev) {
        return;
    }
    int count = 0, count_compare = 1;
    struct list_head *pending = NULL, *list = head->next, *merge_first,
                     *merge_last, *merge_last_prev, *merge_head;
    struct list_head **merge_target;
    head->prev->next = NULL;
    while (list) {
        count++;
        if (count == count_compare) {
            count_compare = count_compare * 2;
        } else {
            size_t bits;
            merge_target = &pending;
            for (bits = count - 1; bits & 1; bits = bits >> 1) {
                merge_target = &(*merge_target)->prev;
            }
            merge_first = *merge_target;
            merge_last = merge_first->prev;
            merge_last_prev = merge_last->prev;
            if (descend == false) {
                merge_two_list(<=);
            } else {
                merge_two_list(>=);
            }
            list->prev = pending;
            merge_head->prev = merge_last_prev;
        }
        pending = list;
        list = list->next;
        pending->next = NULL;
    }
    merge_target = &pending;
    merge_first = pending;
    merge_last = merge_first->prev;
    while (merge_last != head) {
        merge_last_prev = merge_last->prev;
        if (descend == false) {
            merge_two_list(<=);
        } else {
            merge_two_list(>=);
        }
        pending->prev = merge_last_prev;
        merge_first = pending;
        merge_last = merge_first->prev;
        merge_target = &pending;
    }
    head->next = pending;
    struct list_head *rebuild = head;
    while (pending != NULL) {
        pending->prev = rebuild;
        rebuild = pending;
        pending = pending->next;
    }
    rebuild->next = head;
    head->prev = rebuild;
    return;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    if (head->next == head->prev) {
        return 1;
    }
    struct list_head *pos, *pos_next;
    for (pos = head->next; pos != head; pos = pos->next) {
        const char *pos_val = list_entry(pos, element_t, list)->value;
        for (pos_next = pos->next; pos_next != head;
             pos_next = pos_next->next) {
            if (strcmp(pos_val, list_entry(pos_next, element_t, list)->value) >
                0) {
                struct list_head *tmp = pos;
                pos = pos->prev;
                list_del(tmp);
                free(list_entry(tmp, element_t, list)->value);
                free(list_entry(tmp, element_t, list));
                break;
            }
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    if (head->next == head->prev) {
        return 1;
    }
    struct list_head *pos, *pos_next;
    for (pos = head->next; pos != head; pos = pos->next) {
        const char *pos_val = list_entry(pos, element_t, list)->value;
        for (pos_next = pos->next; pos_next != head;
             pos_next = pos_next->next) {
            if (strcmp(pos_val, list_entry(pos_next, element_t, list)->value) <
                0) {
                struct list_head *tmp = pos;
                pos = pos->prev;
                list_del(tmp);
                free(list_entry(tmp, element_t, list)->value);
                free(list_entry(tmp, element_t, list));
                break;
            }
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    if (head->next == head->prev) {
        return list_entry(head->next, queue_contex_t, chain)->size;
    }

    head->prev->next = NULL;
    struct list_head *first_queue =
        list_entry(head->next, queue_contex_t, chain)->q;

    int size = list_entry(head->next, queue_contex_t, chain)->size;
    head = head->next->next;
    struct list_head *queue_next, *first_q_iterator = first_queue->next;
    const char *ptr_value, *queue_value;
    while (head) {
        struct list_head *queue = list_entry(head, queue_contex_t, chain)->q;
        if (list_empty(queue)) {
            head = head->next;
            continue;
        }
        queue->prev->next = NULL;
        queue = queue->next;
        queue_next = queue->next;
        while (queue) {
            if (descend == false) {
                add_to_first_queue(>=);
            } else {
                add_to_first_queue(<=);
            }
            queue = queue_next;
            if (queue_next) {
                queue_next = queue_next->next;
            }
        }
        struct list_head *tmp = list_entry(head, queue_contex_t, chain)->q;
        tmp->next = tmp;
        tmp->prev = tmp;
        first_q_iterator = first_queue->next;
        head = head->next;
    }
    list_entry(first_queue, queue_contex_t, chain)->size = size;
    return size;
}