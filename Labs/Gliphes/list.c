
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>




void add_node(NODE** list, NODE* newNode)
{
	if (*list == NULL)
	{
		*list = (NODE*)malloc(sizeof(NODE));
		(*list)->data = newNode->data;
		(*list)->next = NULL;
	}
	else
	{
		newNode->next = *list;
		*list = newNode;
	}
}

void add_node_last(NODE** list, NODE* newNode)
{
	if (*list == NULL)
	{
		*list = (NODE*)malloc(sizeof(NODE));
		(*list)->data = newNode->data;
		(*list)->next = NULL;
	}
	else
	{
		NODE* node = *list;

		while (node->next != NULL)
			node = node->next;

		node->next = newNode;
		newNode->next = NULL;
	}
}



NODE* get_node(NODE** list, int index)
{
	int i = 0;
	NODE* node = *list;
	while (node != NULL)
	{
		if (i == index)
			return node;
		i++;
		node = node->next;
	}

	return NULL;
}

bool delete_node(NODE** list, int index)
{
	if (index < 0) return false;

	int i = 0;
	NODE* curNode = *list;
	NODE* prevNode = NULL;
	while (curNode != NULL)
	{
		if (i == index)
		{
			if (prevNode == NULL)
			{
				prevNode = *list;
				*list = (*list)->next;
				free(prevNode->data);
				prevNode->next = NULL;
			}

			prevNode->next = curNode->next;
			free(curNode->data);
			return true;
		}

		i++;
		prevNode = curNode;
		curNode = curNode->next;
	}

	return false;
}

int list_len(NODE** list)
{
	int len = 0;

	if (*list != NULL)
	{
		len++;
		NODE* node = *list;
		while (node->next != NULL)
		{
			len++;
			node = node->next;
		}
	}

	return len;
}

void freeList(NODE** list)
{
	NODE* node = *list;
	NODE* nextNode = NULL;
	while (node != NULL)
	{
		nextNode = node->next;

		free(node->data);
		node->next = NULL;
		free(node);

		node = nextNode;
	}
}

void freeListSaveData(NODE** list)
{
	NODE* node = *list;
	NODE* nextNode = NULL;
	while (node != NULL)
	{
		nextNode = node->next;

		node->next = NULL;
		free(node);

		node = nextNode;
	}
}
