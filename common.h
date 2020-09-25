#ifndef COMMON_H
#define COMMON_H

#pragma once
#include <mutex>
#include <iostream>
#include <thread>
#include <time.h>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <typeinfo>
#include <cstdint>

#define MAX_COUNT 100000
#define MAX_VAL 75000
#define ADD_RATIO 75
#define UINT_MAX 0xFFFFFFFF

using namespace std;

typedef unsigned int UINT;
class Node
{
public:	
	UINT data;
	Node* next;
	Node()
	{
		this->data = 0;
		this->next = NULL;
	};
	Node(UINT data)
	{
		this->data = data;
		this->next = NULL;
	}
};

class SyncList {
public:
	SyncList()
	{
	}
	virtual bool add(unsigned int data);
	virtual bool remove(unsigned int data);
	virtual bool contain(unsigned int data);
	virtual void testPrint();
	virtual void clear();

	void test(int thrd_id, int num_thd);
	void test2(int thrd_id, int num_thd);

};

#endif