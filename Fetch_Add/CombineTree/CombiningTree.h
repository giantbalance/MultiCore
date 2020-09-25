#pragma once

#include "common.h"
#include "Node.h"



class CombiningTree
{
public:
	CombiningTree() { cout << "Combining tree constructor" << endl; };
	CombiningTree(UINT width, UINT thread);
	void initialize(void);
	UINT getAndIncrement(UINT threadId);
	void DoToken(UINT threadId);

	Node* nodes[MAX_WIDTH];
	Node* leaf[MAX_WIDTH];
	UINT width;
	UINT thread;
	condition_variable cv;
	mutex cs;
};
