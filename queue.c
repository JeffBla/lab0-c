#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define new_q_element() ((element_t *) test_malloc(sizeof(element_t)))
#define q_entry(node) list_entry(node, element_t, list)
#define swap(x, y, tmp) \
    {                   \
        tmp = x;        \
        x = y;          \
        y = tmp;        \
    }

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
    free(head);
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
        if ((cnt & 1) == 1)
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
    if (!head || list_empty(head))
        return;
    struct list_head *curr = head, *nex, *nex_nex;
    while (curr->next != NULL && curr->next->next != NULL) {
        // Handle next ptr
        nex = curr->next;
        nex_nex = nex->next;
        curr->next = nex_nex;
        nex->next = nex_nex->next;
        nex_nex->next = nex;
        // Handle prev ptr
        nex->prev = nex_nex;
        nex_nex->prev = curr;
        nex_nex->next->prev = nex;

        curr = nex;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    // Reverse c-str ptr address
    char *tmp_for_swap = NULL;
    element_t *forward_e = NULL, *backward_e = NULL;
    for (forward_e = q_entry(head->next), backward_e = q_entry(head->prev);
         forward_e != backward_e; forward_e = q_entry(forward_e->list.next),
        backward_e = q_entry(backward_e->list.prev)) {
        swap(backward_e->value, forward_e->value, tmp_for_swap);
        if (forward_e->list.next == &backward_e->list)
            break;
    }
}

void q_reverse_only_k(struct list_head *front, int k)
{
    // Reverse c-str ptr address
    // Init back
    struct list_head *back = front;
    for (int i = 1; i < k; i++) {
        back = back->next;
    }
    // Reverse
    char *tmp_for_swap = NULL;
    while (front != back) {
        swap(q_entry(front)->value, q_entry(back)->value, tmp_for_swap);
        if (front->next == back)
            break;

        front = front->next;
        back = back->prev;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    struct list_head *curr = head->next, *end = head->next;
    while (curr) {
        int cnt = 0;
        while (cnt < k && end) {
            end = end->next;
            cnt++;
        }
        if (cnt != k)
            break;
        q_reverse_only_k(curr, k);
        curr = end;
    }
}

bool cmp_in_sort(struct list_head *a, struct list_head *b, bool descend)
{
    const element_t *ele_a = q_entry(a);
    const element_t *ele_b = q_entry(b);
    if (descend) {
        return strcmp(ele_a->value, ele_b->value) >= 0 ? 0 : 1;
    } else {
        return strcmp(ele_a->value, ele_b->value) >= 0 ? 1 : 0;
    }
}

struct list_head *merge(bool descend, struct list_head *b, struct list_head *a)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        if (cmp_in_sort(a, b, descend)) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

void merge_final(bool descend,
                 struct list_head *head,
                 struct list_head *b,
                 struct list_head *a)
{
    struct list_head **tail = &head, *prev = NULL;

    for (;;) {
        if (cmp_in_sort(a, b, descend)) {
            *tail = a;
            a->prev = prev;
            prev = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            b->prev = prev;
            prev = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    // Make linked list Circular
    head->prev = *tail;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    // Ref from
    // list_sort.c(https://github.com/torvalds/linux/blob/master/lib/list_sort.c)
    // with the help of 你所不知道的 C 語言: linked list 和非連續記憶體
    // (https://hackmd.io/@sysprog/c-linked-list#Linux-%E6%A0%B8%E5%BF%83%E7%9A%84-list_sort-%E5%AF%A6%E4%BD%9C)
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;
    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(descend, b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        // Remove from list's head. Insert from pending's tail.
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);
    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(descend, pending, list);
        pending = next;
    }
    // Maintain the prev ptr
    merge_final(descend, head, pending, list);
}

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
