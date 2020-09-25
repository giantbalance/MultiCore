#pragma once

#include "common.h"

class CoarseList : public SyncList
{
public:
	CoarseList() {
		this->head = new Node();
		this->last = new Node(0xFFFFFFFF);
		head->next = last;
	};
	bool add(unsigned int data) {
		Node *prev, *curr;
		bool ret = true;

		myMutex.lock();
		prev = head;
		curr = head->next;
		while (data > curr->data)
		{
			prev = curr;
			curr = curr->next;
		}
		if (data == curr->data)
		{
			ret = false;
		}
		else
		{
			Node* node = new Node;
			node->data = data;
			node->next = curr;
			prev->next = node;
		}
		myMutex.unlock();

		return ret;
	};

	bool remove(unsigned int data)
	{
		Node *prev, *curr;
		bool ret = true;

		myMutex.lock();
		prev = head;
		curr = prev->next;

		while (curr->data < data)
		{
			prev = curr;
			curr = curr->next;
		}
		if (data == curr->data)
		{
			prev->next = curr->next;
			delete curr;
		}
		else
		{
			ret = false;
		}
		myMutex.unlock();
		return ret;
	};

	bool contain(unsigned int data)
	{
		Node *prev, *curr;
		bool ret = false;

		myMutex.lock();
		prev = head;
		curr = prev->next;

		while (curr->data < data)
		{
			prev = curr;
			curr = curr->next;
		}
		if (data == curr->data)
		{
			ret = true;
		}
		
		myMutex.unlock();
		return ret;
	};
	void testPrint(void)
	{
		cout << "Test Print" << endl;
		Node* node = this->head;
		UINT data;
		for (int i = 0; i < 10; i++)
		{
			data = node->data;
			cout << data << "  ";
			node = node->next;
		}
		cout << endl;
	};
	void clear()
	{
		Node* prev = head;
		Node* curr = head->next;
		while (curr)
		{
			delete(prev);
			prev = curr;
			curr = curr->next;
		}
		this->head = new Node();
		this->head->next = last;
	};

	std::mutex myMutex;
	Node* head;
	Node* last;
};

