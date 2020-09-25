#pragma once

#include "common.h"

enum class CStatus {
	IDLE,
	FIRST,
	SECOND,
	RESULT,
	ROOT
};



class Node
{
public:
	Node();
	Node(Node* myParent, UINT id);
	Node(const Node& a) { cout << "복사생성자" << endl; };

	mutex uniqueMutex;
	bool locked;
	CStatus cstatus;
	UINT firstValue, secondValue;
	UINT result;
	UINT id;
	Node* parent;
#if IS_UINT
	UINT counter;
	UINT combinedCounter;
#else
	atomic<UINT>counter;
	atomic<UINT>combinedCounter;
#endif
	

	bool precombine(condition_variable* cv);
	UINT combine(UINT combined, condition_variable* cv);
	UINT operation(UINT combined, condition_variable* cv);
	void distribute(UINT prior, condition_variable* cv);
};




