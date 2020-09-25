#pragma once
#include "common.h"

// Atomic Node
class ANode {
public:
	ANode() { data = 0; next = NULL; };
	ANode(UINT data, ANode* next) { this->data = data; this->next = next; };
	UINT data;
	atomic<ANode*> next;
	bool compareAndSet(ANode* expRef, ANode* newRef, int expMark, int newMark);
};

class Window 
{
public:
	Window();
	Window(ANode* prev, ANode* curr);
	ANode *prev, *curr;
};

class LockFreeList :
	public SyncList
{
public:
	LockFreeList();
	bool add(UINT data);
	bool remove(UINT data);
	bool contain(UINT data);

	void testPrint();
	void clear();

	ANode* aHead;
	ANode* aLast;
};



