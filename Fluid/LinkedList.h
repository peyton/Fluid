//
//  LinkedList.h
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#ifndef Fluid_LinkedList_h
#define Fluid_LinkedList_h

#include <stddef.h>

struct ListNode;

struct ListNode {
    size_t i, j;
    void *data;
    struct ListNode *prev, *next;
};

typedef struct ListNode* List;

List list_prepend(List list, size_t i, size_t j, void *data);
List list_remove_node(struct ListNode *node, List list);
void list_free(List list);

#endif
