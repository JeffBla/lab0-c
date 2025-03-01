#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define new_q_element() ((element_t *) test_malloc(sizeof(element_t)))
#define q_entry(node) list_entry(node, element_t, list)
#define ctx_entry(node) list_entry(node, queue_contex_t, chain)
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
    if (new_node->value == NULL) {
        free(new_node);
        return false;
    }
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
    if (new_node->value == NULL) {
        free(new_node);
        return false;
    }
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
    if (sp) {
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
    while (curr != head) {
        if ((cnt & 1) == 1)
            mid = mid->next;
        cnt++;
        curr = curr->next;
    }
    if (mid == head)
        return false;
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
    while (curr != head) {
        bool isDup = false;
        while (curr->next != head &&
               strcmp(q_entry(curr)->value, q_entry(curr->next)->value) == 0) {
            isDup = true;
            curr = curr->next;
        }
        struct list_head *curr_next = curr->next;
        if (isDup) {
            struct list_head *rm_target = prev->next, *rm_next_target;
            prev->next = curr_next;
            curr_next->prev = prev;
            while (rm_target != curr_next) {
                rm_next_target = rm_target->next;
                q_release_element(q_entry(rm_target));
                rm_target = rm_next_target;
            }
        } else {
            prev = curr;
        }
        curr = curr_next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *first, *second, *tmp;

    list_for_each_safe (first, tmp, head) {
        if (first->next == head)
            break;

        second = first->next;

        // Swap nodes
        first->next = second->next;
        second->next->prev = first;

        second->next = first;
        second->prev = first->prev;
        first->prev->next = second;
        first->prev = second;

        // Move to the next pair
        tmp = first->next;
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
    while (curr != head) {
        int cnt = 0;
        while (cnt < k && end != head) {
            end = end->next;
            cnt++;
        }
        if (cnt != k)
            break;
        q_reverse_only_k(curr, k);
        curr = end;
    }
}

void break_circular(struct list_head *head)
{
    head->prev->next = NULL;
}

bool cmp_in_sort(struct list_head *a, struct list_head *b, bool descend)
{
    const element_t *ele_a = q_entry(a);
    const element_t *ele_b = q_entry(b);
    int cmp = strcmp(ele_a->value, ele_b->value);
    return descend ? (cmp > 0) : (cmp < 0);
}

struct list_head *merge(bool descend, struct list_head *b, struct list_head *a)
{
    struct list_head *head = NULL, **tail = &head;

    while (a && b) {
        if (cmp_in_sort(a, b, descend)) {
            *tail = a;
            tail = &a->next;
            a = a->next;
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
        }
    }
    *tail = a ? a : b;

    return head;
}

void merge_final(bool descend,
                 struct list_head *head,
                 struct list_head *b,
                 struct list_head *a)
{
    struct list_head **tail = &head->next, *prev = head;

    while (a && b) {
        if (cmp_in_sort(a, b, descend)) {
            *tail = a;
            a->prev = prev;
            prev = a;
            tail = &a->next;
            a = a->next;

        } else {
            *tail = b;
            b->prev = prev;
            prev = b;
            tail = &b->next;
            b = b->next;
        }
    }
    *tail = a ? a : b;

    // Make rest linked list nodes doubly linked
    while (*tail) {
        (*tail)->prev = prev;
        prev = *tail;
        tail = &(*tail)->next;
    }
    // Make list circular
    head->prev = prev;
    prev->next = head;
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

    break_circular(head);
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

int q_ascend_descend(struct list_head *head,
                     struct list_head *curr,
                     bool descend,
                     char **pivot)
{
    int q_sz = 0;
    if (curr->next != head) {
        q_sz = q_ascend_descend(head, curr->next, descend, pivot);
    }
    element_t *target = q_entry(curr);
    int cmp = strcmp(target->value, *pivot);
    if ((descend && (cmp >= 0)) || (!descend && (cmp <= 0))) {
        *pivot = target->value;
        q_sz++;
    } else {
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        q_release_element(q_entry(curr));
    }

    return q_sz;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    char *min = "~~~~~~~";
    return q_ascend_descend(head, head->next, false, &min);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    char *max = "";
    return q_ascend_descend(head, head->next, true, &max);
}

bool list_is_2(struct list_head *head)
{
    const struct list_head *head_next = head->next;
    return (!list_empty(head) && !list_is_singular(head) &&
            head_next->next == head->prev);
}

void _q_merge(struct list_head *head,
              bool descend,
              bool is_final,
              struct list_head *empty_head)
{
    struct list_head *first, *second;
    queue_contex_t *ctx1, *ctx2;

    first = head->next;
    list_del(first);
    ctx1 = ctx_entry(first);
    break_circular(ctx1->q);

    second = head->next;
    list_del(second);
    ctx2 = ctx_entry(second);
    break_circular(ctx2->q);

    // if (is_final) {
    merge_final(descend, ctx1->q, ctx1->q->next, ctx2->q->next);
    // } else {
    //     ctx1->q->next = merge(descend, ctx1->q->next, ctx2->q->next);
    // }
    ctx1->size += ctx2->size;
    ctx2->q->next = ctx2->q;
    ctx2->q->prev = ctx2->q;
    ctx2->size = 0;
    list_add(&ctx2->chain, empty_head);

    list_add(&ctx1->chain, head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    LIST_HEAD(empty_head);
    while (!list_is_2(head)) {
        _q_merge(head, descend, false, &empty_head);
    }
    _q_merge(head, descend, true, &empty_head);

    list_splice_tail(&empty_head, head);
    const queue_contex_t *merged_ctx = ctx_entry(head->next);
    return merged_ctx->size;
}
