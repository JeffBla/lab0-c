#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define new_q_element() ((element_t *) test_malloc(sizeof(element_t)))
#define q_entry(node) list_entry(node, element_t, list)


/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) test_malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry = NULL, *tmp;
    list_for_each_entry_safe (entry, tmp, head, list) {
        list_del(&entry->list);
        q_release_element(entry);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_node = new_q_element();
    if (!new_node)
        return false;
    new_node->value = test_strdup(s);
    if (new_node->value == NULL)
        return false;
    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new_node = new_q_element();
    if (!new_node)
        return false;
    new_node->value = test_strdup(s);
    if (new_node->value == NULL)
        return false;
    list_add_tail(&new_node->list, head);
    return true;
}

element_t *q_remove_mid(struct list_head *head,
                        struct list_head *target,
                        char *sp,
                        size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    list_del(target);
    if (!sp) {
        strncpy(sp, q_entry(target)->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return q_entry(target);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove_mid(head, head->next, sp, bufsize);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return q_remove_mid(head, head->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *mid = head->next;
    struct list_head *curr = head->next;
    int cnt = 0;
    while (curr) {
        if (cnt % 2 == 1)
            mid = mid->next;
        cnt++;
        curr = curr->next;
    }
    list_del(mid);
    q_release_element(q_entry(mid));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *prev = head, *curr = head->next;
    while (curr) {
        bool isDup = false;
        while (curr->next &&
               strcmp(q_entry(curr)->value, q_entry(curr->next)->value) == 0) {
            isDup = true;
            curr = curr->next;
        }
        if (isDup) {
            struct list_head *rm_target = prev->next;
            prev->next = curr->next;
            curr->next->prev = prev;
            while (rm_target != prev->next) {
                q_release_element(q_entry(rm_target));
            }
        } else {
            prev = prev->next;
        }
        curr = curr->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
