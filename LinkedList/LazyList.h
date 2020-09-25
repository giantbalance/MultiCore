#pragma once
#include "common.h"

class LazyNode
{
public:
	LazyNode();
	LazyNode(UINT data);
	UINT data;
	LazyNode* next;
	bool mark;
	mutex myMutex;
};
class LazyList :
	public SyncList
{
public:
	LazyList() { head = new LazyNode(); last = new LazyNode(UINT_MAX); head->next = last; };
	bool add(UINT data);
	bool remove(UINT data);
	bool contain(UINT data);
	void testPrint();
	void clear();
private:
	bool validate(LazyNode* prev, LazyNode* curr);

	LazyNode* head;
	LazyNode* last;
};

