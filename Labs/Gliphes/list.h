#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct Node
{
	void* data;
	struct Node* next;
} NODE;

void add_node(NODE** list, NODE* newNode);
void add_node_last(NODE** list, NODE* newNode);
NODE* get_node(NODE** list, int index);
bool delete_node(NODE** list, int index);
void freeList(NODE** list);
void freeListSaveData(NODE** list);
int list_len(NODE** list);
