#pragma once
#include "common.h"

class LockNode
{
public:
	LockNode();
	LockNode(UINT data);
	UINT data;
	LockNode* next;
	mutex myMutex;
};
class FineList :
	public SyncList
{
public:
	FineList();
	bool add(UINT data);
	bool remove(UINT data);
	bool contain(UINT data);
	void testPrint();
	void clear();

	LockNode* lockHead;
	LockNode* lockLast;
};

