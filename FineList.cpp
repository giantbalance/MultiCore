#include "FineList.h"

FineList::FineList()
{
	this->lockHead = new LockNode();
	this->lockLast = new LockNode(UINT_MAX);
	lockHead->next = lockLast;
}

bool FineList::add(UINT data)
{
	bool ret = false;
	this->lockHead->myMutex.lock();
	LockNode* prev = this->lockHead;
	LockNode* curr = prev->next;
	curr->myMutex.lock();

	while (curr->data < data)
	{
		prev->myMutex.unlock();
		prev = curr;
		curr = curr->next;
		curr->myMutex.lock();
	}
	if (curr->data != data)
	{
		LockNode* newNode = new LockNode(data);
		newNode->next = curr;
		prev->next = newNode;
		ret = true;
	}
	
	curr->myMutex.unlock();
	prev->myMutex.unlock();

	return ret;
}

bool FineList::remove(UINT data)
{
	bool ret = false;
	LockNode* prev = NULL, * curr = NULL;
	lockHead->myMutex.lock();

	prev = lockHead;
	curr = prev->next;
	curr->myMutex.lock();

	while (curr->data < data) {
		prev->myMutex.unlock();
		prev = curr;
		curr = curr->next;
		curr->myMutex.lock();
	}
	if (curr->data == data)
	{
		prev->next = curr->next;
		ret = true;
	}

	curr->myMutex.unlock();
	prev->myMutex.unlock();

	return ret;
}

bool FineList::contain(UINT data)
{
	bool ret = false;
	LockNode* prev = NULL, * curr = NULL;
	lockHead->myMutex.lock();

	prev = lockHead;
	curr = prev->next;
	curr->myMutex.lock();

	while (curr->data < data) {
		prev->myMutex.unlock();
		prev = curr;
		curr = curr->next;
		curr->myMutex.lock();
	}
	if (curr->data == data)
	{
		ret = true;
	}

	curr->myMutex.unlock();
	prev->myMutex.unlock();

	return ret;
}

void FineList::testPrint()
{
	cout << "Test Print" << endl;
	LockNode* node = this->lockHead;
	UINT data;
	for (int i = 0; i < 10; i++)
	{
		data = node->data;
		cout << data << "  ";
		node = node->next;
	}
	cout << endl;
}

void FineList::clear()
{
	LockNode* prev = lockHead;
	LockNode* curr = lockHead->next;
	while (curr)
	{
		delete(prev);
		prev = curr;
		curr = curr->next;
	}
	this->lockHead = new LockNode();
	this->lockHead->next = lockLast;
}

LockNode::LockNode()
{
	this->data = 0;
	this->next = NULL;
}

LockNode::LockNode(UINT data)
{
	this->data = data;
	this->next = NULL;
}
