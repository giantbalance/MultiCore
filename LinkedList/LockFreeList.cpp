#include "LockFreeList.h"

void setMark(ANode* ref, int mark)
{
	intptr_t temp = intptr_t(ref);
	temp |= mark;
	ref = reinterpret_cast<ANode*>(temp);
}
bool getMark(ANode* ref)
{
	intptr_t temp = intptr_t(ref);
	temp &= (0x1);
	return (bool)temp;
}
ANode* getRef(ANode* ref)
{
	intptr_t temp = intptr_t(ref);
	temp &= ~(0x1);
	return reinterpret_cast<ANode*>(temp);
}


Window* find(ANode* head, UINT data)
{
	ANode* prev, * curr, * succ;
	bool marked, snip;
retry: while (true)
	{
		prev = getRef(head);
		curr = getRef(prev->next);
		while (true)
		{
			succ = getRef(curr->next);
			marked = getMark(curr->next);
			while (marked)
			{
				snip = prev->compareAndSet(curr, succ, 0, 1);
				if (!snip) goto retry;
				curr = succ;
				succ = getRef(curr->next);
				marked = getMark(curr->next);
			}
			if (curr->data >= data)
				return new Window(prev, curr);
			prev = getRef(curr);
			curr = getRef(succ);
		}
	}
}

LockFreeList::LockFreeList()
{
	aHead = new ANode();
	aLast = new ANode(0xFFFFFFFF, NULL);
	aHead->next.store(aLast);

}	

bool LockFreeList::add(UINT data)
{
	bool ret = false;
	while (true)
	{
		Window* window = find(aHead, data);
		ANode* prev = window->prev;
		ANode* curr = window->curr;
		if (data == curr->data)
		{
			return false;
		}
		else
		{
			ANode* node = new ANode(data, curr);
			if (getRef(prev)->compareAndSet(curr, node, 0, 0))
			{
				return true;
			}
		}
	}
	return ret;
}

bool LockFreeList::remove(UINT data)
{
	bool ret = false;
	bool snip;
	while (true)
	{
		Window* window = find(aHead, data);
		ANode* prev = window->prev;
		ANode* curr = window->curr;
		if (curr->data != data)
		{
			return false;
		}
		else
		{
			ANode* succ = getRef(curr->next);
			snip = getRef(curr)->compareAndSet(succ, succ, 0, 1);
			if (!snip)
				continue;
			getRef(prev)->compareAndSet(curr, succ, 0, 0);
			return true;
		}
	}
	return ret;
}

bool LockFreeList::contain(UINT data)
{
	bool ret = false;
	bool marked = false;
	ANode* curr = aHead;
	while (curr->data < data)
	{
		curr = getRef(curr->next);
		marked = getMark(curr->next);
	}
	return (curr->data == data && !marked);
}

void LockFreeList::testPrint()
{
	cout << "Test Print" << endl;
	ANode* node = aHead;
	UINT data;
	for (int i = 0; i < 10; i++)
	{
		data = node->data;
		cout << data <<"  ";
		node = node->next;
	}
	cout << endl;
}

void LockFreeList::clear()
{
	ANode* prev = aHead;
	ANode* curr = aHead->next;
	while (curr)
	{
		delete(prev);
		prev = curr;
		curr = curr->next;
	}
	this->aHead = new ANode();
	this->aHead->next = aLast;
}

Window::Window()
{
}

Window::Window(ANode* prev, ANode* curr)
{
	this->prev = prev;
	this->curr = curr;
}


bool ANode::compareAndSet(ANode* expRef, ANode* newRef, int expMark, int newMark)
{
	ANode* expTemp = reinterpret_cast<ANode*>((intptr_t(expRef) | expMark));
	ANode* newTemp = reinterpret_cast<ANode*>((intptr_t(newRef) | newMark));
	return this->next.compare_exchange_strong(expTemp, newTemp);
}

