#ifndef __SJA1105P_LISTS_H__
#define __SJA1105P_LISTS_H__


/**
 * Find the address of the structure this list entry is part of.
 * The list hook (next/prev in ListHead) need not be
 * at the top of the structure - thus the 'offsetof'
 * @param entry list entry (pointer to the list hook within a
 *              list element
 * @param elt_type type of the element
 * @param hook name of the list hook within the list element
 *             type
 */
#define LIST_ELT(entry, elt_type, hook) ((elt_type *)((char *)(entry) - offsetof(elt_type, hook)))


/**
 * iterate over all entries in the list. During the loop, the
 * entry must not be removed from the list.
 * @param entry
 * @param list
 */
#define LIST_ITERATE(entry, list) \
    for (entry = (list)->next; entry != (list); entry = entry->next )

/**
 * iterate over all entries in the list. It is safe to remove
 * the entry in the loop
 */
#define LIST_ITERATE_SAFE(entry,tmp, list) \
    for (entry = (list)->next, tmp = (list)->next->next; entry != (list); \
        entry = tmp, tmp = tmp->next)

/*************************************************************************************************
 * Public Types
 ************************************************************************************************/

/**
 * double linked list
 * type of the list hook mentioned above
 */
typedef struct ListHead
{
    struct ListHead *next;
    struct ListHead *prev;
} ListHead;


/**
 * Initialise a list. Resulting in an empty list with both
 * 'next' and 'prev' pointing to itself.
 * @param list
 */
static inline void ListInit(ListHead *list)
{
    list->next = list;
    list->prev = list;
}

/**
 * Add an element to the head of the indicated list
 * @param new_entry pointer to new entry
 * @param list head of the list.
 */
static inline void ListAddHead(ListHead *new_entry, ListHead *list)
{
    ListHead *old_head = list->next;       /* save the old head */

    new_entry->next = old_head;
    new_entry->prev = list;

    list->next = new_entry;      /* The head is after the tail. new_entry is the new head */
    old_head->prev = new_entry;
}


/**
 * Add an element to the tail of the indicated list
 * @param new_entry pointer to new entry
 * @param list head of the list.
 */
static inline void ListAddTail(ListHead *new_entry, ListHead *list)
{
    ListHead *prev_tail = list->prev;   /* save the previous tail */

    new_entry->next = list;
    new_entry->prev = prev_tail;

    list->prev = new_entry;
    /* The tail is before the head. new_entry is the new tail */
    prev_tail->next = new_entry;
}

/**
 * Remove an entry from the the list it is currently inserted in
 * and reinitialising the links of the entry
 * @param entry
 */
static inline void ListRemove(ListHead *entry)
{
    ListHead *list_next;
    ListHead *list_prev;

    /* determine entry's neighbours*/
    list_next = entry->next;
    list_prev = entry->prev;

    /* link the neighbours, leaving out old entry */
    list_next->prev = entry->prev;
    list_prev->next = entry->next;

    /* reinit the entry */
    entry->next = entry;
    entry->prev = entry;
}


/**
 * Remove entry from the list it is in and add it to the
 * tail of the indicated list
 * @param entry 
 * @param list whose tail entry must go to
 */
static inline void ListMoveTail(ListHead *entry,
                                ListHead *list)
{
    ListRemove(entry);
    ListAddTail(entry, list);
}

/**
 * Remove entry from the list it is in and add it to the
 * head of the indicated list
 * @param entry 
 * @param list whose head entry must go to 
 */
static inline void ListMoveHead(ListHead *entry,
                                ListHead *list)
{
    ListRemove(entry);
    ListAddHead(entry, list);
}

/**
 * Append list to a previous one 
 * @param l2 list to be appended 
 * @param l1 previous list  
 */
static inline void ListAppend(ListHead *l2,
                              ListHead *l1)
{
    ListHead * end1, *end2;

    end1 = l1->prev;
    end2 = l2->prev;

    end1->next = l2;
    end2->next = l1;
    l2->prev = end1;
    l1->prev = end2;
}


/**
 * Test if the list is empty
 * @param entry
 */
static inline int ListEmpty(ListHead *list)
{
    return(list->next == list);
}

#endif
