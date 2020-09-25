#include "LazyList.h"

LazyNode::LazyNode()
{
	data = 0;
	next = NULL;
	mark = false;
}

LazyNode::LazyNode(UINT data)
{
	this->data = data;
	next = NULL;
	mark = false;
}

bool LazyList::add(UINT data)
{
	bool ret = false;
	while (true)
	{
		LazyNode* prev = head;
		LazyNode* curr = head->next;
		while (curr->data < data)
		{
			prev = curr;
			curr = curr->next;
		}
		prev->myMutex.lock();
		curr->myMutex.lock();
		if (validate(prev, curr))
		{
			if (curr->data == data)
			{
				ret = false; // For make it sure
				goto UNLOCK;
			}
			else
			{
				LazyNode* node = new LazyNode(data);
				node->next = curr;
				prev->next = node;
				ret = true;
				goto UNLOCK;
			}
		}
		UNLOCK:
		curr->myMutex.unlock();
		prev->myMutex.unlock();
		break;
	}
	return ret;
}

bool LazyList::remove(UINT data)
{
	bool ret = false;
	while (true)
	{
		LazyNode* prev = head;
		LazyNode* curr = head->next;
		while (curr->data < data) {
			prev = curr;
			curr = curr->next;
		}
		prev->myMutex.lock();
		curr->myMutex.lock();
		if (validate(prev, curr))
		{
			if (curr->data != data)
			{
				ret = false; // For make it sure
				goto UNLOCK;
			}
			else
			{
				curr->mark = true;
				prev->next = curr->next;
				ret = true;
				goto UNLOCK;
			}
		}
	UNLOCK:
		curr->myMutex.unlock();
		prev->myMutex.unlock();
		break;
	}
	return ret;
}

bool LazyList::contain(UINT data)
{
	bool ret = false;
	LazyNode* curr = head;
	while (curr->data < data)
	{
		curr = curr->next;
	}
	ret = (curr->data == data && !curr->mark);
	return ret;
}

void LazyList::testPrint()
{
	cout << "Test Print" << endl;
	LazyNode* node = this->head;
	UINT data;
	for (int i = 0; i < 10; i++)
	{
		data = node->data;
		cout << data << "  ";
		node = node->next;
	}
	cout << endl;
}

void LazyList::clear()
{
	LazyNode* prev = head;
	LazyNode* curr = head->next;
	while (curr)
	{
		delete(prev);
		prev = curr;
		curr = curr->next;
	}
	this->head = new LazyNode();
	this->head->next = last;
}

bool LazyList::validate(LazyNode* prev, LazyNode* curr)
{
	return (!prev->mark && !curr->mark && prev->next == curr);
}
