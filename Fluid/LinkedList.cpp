//
//  LinkedList.c
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#include "LinkedList.h"

#include <stdlib.h>
#include <stdio.h>

List list_prepend(List list, size_t i, size_t j, void *data)
{
    struct ListNode *new_node = (struct ListNode *)malloc(sizeof(struct ListNode));
    new_node->i = i;
    new_node->j = j;
    new_node->data = data;
    new_node->next = list;
    new_node->prev = NULL;
    if (list)
        list->prev = new_node;
    
    return new_node;
}

List list_remove_node(struct ListNode *node, List list)
{
    List next = node->next;
    if (node->prev)
        node->prev->next = next;
    if (next)
        next->prev = node->prev;
        
    free(node);
    
    if (node == list)
        return next;
    else
        return list;
}

void list_free(List list)
{
    if (!list)
        return;
    
    list_free(list->next);
    free(list);
}