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
#define add_to_first_queue(opera)                                        \
    for (struct list_head *ptr = first_q_iterator;; ptr = ptr->next) {   \
        if (strcmp(list_entry(ptr, element_t, list)->value,              \
                   list_entry(queue, element_t, list)->value) opera 0 || \
            !ptr) {                                                      \
            queue_next = queue->next;                                    \
            ptr = ptr->prev;                                             \
            list_add(queue, ptr);                                        \
            first_q_iterator = ptr->next;                                \
            *size_ptr = (*size_ptr) + 1;                                 \
            break;                                                       \
        }                                                                \
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
    struct list_head *pos, *tmp;

    list_for_each_safe (pos, tmp, head) {
        free(list_entry(pos, element_t, list)->value);
        free(list_entry(pos, element_t, list));
    }

    free(head);
}

/* Insert an element at head of queue */
/* cppcheck-suppress nullPointer */
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
    if (!head) {
        return 0;
    }
    int len = 0;
    struct list_head *li;

    list_for_each (li, head) {
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

    for (pos = head->next; pos->next != head; pos = pos->next) {
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
    for (first = head->next, second = head->next->next;
         first != head && second != head;
         first = first->next->next, second = second->next->next) {
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
    for (struct list_head *group_first = head; group_first != head;) {
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

        group_first->prev->next = tmp_next;
        tmp_next->prev = group_first->prev;
        struct list_head *tmp = group_first->prev;
        group_first->prev = tmp_prev;

        group_first = tmp;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
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
    int count = 0;
    struct list_head *pos, *pos_next;
    for (pos = head->next, pos_next = pos->next; pos_next != head;) {
        if (strcmp(list_entry(pos, element_t, list)->value,
                   list_entry(pos_next, element_t, list)->value) > 0) {
            list_del(pos_next);
            free(list_entry(pos_next, element_t, list)->value);
            free(list_entry(pos_next, element_t, list));
            pos_next = pos->next;
            continue;
        }
        pos = pos_next;
        pos_next = pos_next->next;
        count++;
    }
    return count + 1;
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
    int count = 0;
    struct list_head *pos, *pos_next;
    for (pos = head->next, pos_next = pos->next; pos_next != head;) {
        if (strcmp(list_entry(pos, element_t, list)->value,
                   list_entry(pos_next, element_t, list)->value) < 0) {
            list_del(pos_next);
            free(list_entry(pos_next, element_t, list)->value);
            free(list_entry(pos_next, element_t, list));
            pos_next = pos->next;
            continue;
        }
        pos = pos_next;
        pos_next = pos_next->next;
        count++;
    }
    return count + 1;
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
        list_entry(head->next, queue_contex_t, chain)->q->next;

    int *size_ptr = &list_entry(head->next, queue_contex_t, chain)->size;
    head = head->next->next;
    struct list_head *queue_next, *first_q_iterator = first_queue;
    while (head) {
        struct list_head *queue = list_entry(head, queue_contex_t, chain)->q;

        queue->prev->next = NULL;
        queue = queue->next;
        queue_next = queue->next;
        queue->prev->next = NULL;

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
        first_q_iterator = first_queue;
        head = head->next;
    }
    return *size_ptr;
}
