#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);

    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    if (list_empty(l)) {
        free(l);
        return;
    }

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *element = (element_t *) malloc(sizeof(element_t));
    if (!element)
        return false;

    char *tmp = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!tmp) {
        free(element);
        return false;
    }

    strncpy(tmp, s, strlen(s) + 1);
    element->value = tmp;

    list_add(&element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;

    element_t *element = (element_t *) malloc(sizeof(element_t));
    if (!element)
        return false;

    char *tmp = (char *) malloc(sizeof(char) * (strlen(s) + 1));
    if (!tmp) {
        free(element);
        return false;
    }

    strncpy(tmp, s, strlen(s) + 1);
    element->value = tmp;

    list_add_tail(&element->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_first_entry(head, element_t, list);

    list_del(head->next);

    if (sp != NULL) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_last_entry(head, element_t, list);

    list_del(head->prev);

    if (sp != NULL) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
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
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    int mid = q_size(head) / 2;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (mid == 0) {
            list_del(&entry->list);
            q_release_element(entry);
            return true;
        }
        mid = mid - 1;
    }

    return false;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    if (list_empty(head) || list_is_singular(head))
        return true;

    element_t *entry = NULL, *safe = NULL;
    char *tmp = NULL;
    bool delete = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next == head) {
            if (delete) {
                list_del(&entry->list);
                q_release_element(entry);
                return true;
            } else {
                return true;
            }
        }

        tmp = container_of(entry->list.next, element_t, list)->value;

        if (strcmp(entry->value, tmp) == 0) {
            delete = 1;
            list_del(&entry->list);
            q_release_element(entry);
        } else if (delete) {
            delete = 0;
            list_del(&entry->list);
            q_release_element(entry);
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    element_t *entry = NULL, *safe = NULL;
    bool even = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (even) {
            list_move_tail(&entry->list, entry->list.prev);
        }
        even = !even;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *left = head->next;
    struct list_head *right = head->prev;
    struct list_head *tmp;

    while (left != right) {
        if (left->next == right) {
            list_move(left, right);
            break;
        }
        tmp = left->prev;
        list_move(left, right);
        list_move(right, tmp);

        tmp = left->prev;
        left = right->next;
        right = tmp;
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k == 1) {
        return;
    }

    struct list_head *node = NULL, *safe = NULL;
    struct list_head *left = head->next;
    struct list_head *right;
    struct list_head *tmp;
    int count = 0;
    list_for_each_safe (node, safe, head) {
        count = count + 1;
        if (count == k) {
            count = 0;
            right = node;

            while (left != right) {
                if (left->next == right) {
                    list_move(left, right);
                    break;
                }
                tmp = left->prev;
                list_move(left, right);
                list_move(right, tmp);

                tmp = left->prev;
                left = right->next;
                right = tmp;
            }

            left = safe;
        }
    }

    return;
}

// merge sort

struct list_head *mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL, **ptr = &head, **node;

    for (node = NULL; L1 && L2; *node = (*node)->next) {
        node = strcmp(list_entry(L1, element_t, list)->value,
                      list_entry(L2, element_t, list)->value) < 0
                   ? &L1
                   : &L2;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    return head;
}

struct list_head *mergesort_list(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    // find the middle node
    struct list_head *slow = head;
    for (struct list_head *fast = head; fast && fast->next;
         fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow;

    // cut the link of list
    slow->prev->next = NULL;

    struct list_head *left = mergesort_list(head), *right = mergesort_list(mid);

    return mergeTwoLists(left, right);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    head->prev->next = NULL;
    head->next = mergesort_list(head->next);

    // complete the circular list
    struct list_head *p = head, *n = head->next;
    for (; n != NULL; n = n->next) {
        n->prev = p;
        p = n;
    }
    p->next = head;
    head->prev = p;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head)) {
        return 0;
    } else if (list_is_singular(head)) {
        return 1;
    }

    q_reverse(head);

    char *max = list_first_entry(head, element_t, list)->value;

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (strcmp(entry->value, max) < 0) {
            list_del(&entry->list);
            q_release_element(entry);
        } else {
            max = entry->value;
        }
    }

    q_reverse(head);

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }

    queue_contex_t *new_head = list_first_entry(head, queue_contex_t, chain);
    queue_contex_t *entry = NULL;

    list_for_each_entry (entry, head, chain) {
        if (entry == new_head)
            continue;
        list_splice_init(entry->q, new_head->q);
    }

    q_sort(new_head->q);

    return q_size(new_head->q);
}
