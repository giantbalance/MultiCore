#pragma once
#include "barrier.h"

class SenseBarrier :
	public MyBarrier
{
public:
	SenseBarrier(UINT n);

	atomic<UINT>count;
	atomic<bool>sense;

	void await(void);

	// MyBarrier을(를) 통해 상속됨
	virtual void await_th(UINT threadId) override;
};



class CombiningTreeBarrier :
	public MyBarrier
{
public:
	CombiningTreeBarrier(UINT size, UINT radix);
	// MyBarrier을(를) 통해 상속됨
	virtual void await(void) override;
	virtual void await_th(UINT threadId) override;
	class Node
	{
	public:
		Node(UINT radix) {
			sense = false;
			parent = NULL;
			count.store(radix);
			size = radix;
		};
		Node(UINT radix, Node* parent)
		{
			sense = false;
			parent = NULL;
			count.store(radix);
			size = radix;
			this->parent = parent;
		}
		void Node_await(void);
		atomic<UINT>count;
		Node* parent;
		volatile bool sense;
		UINT size;
	};
	
	void build(CombiningTreeBarrier::Node* parent, UINT depth);

	UINT radix;
	Node* myleaf[MAX_NODE];
	UINT leaves;
};

class StaticTreeBarrier
	:public MyBarrier
{
public:
	StaticTreeBarrier(UINT size, UINT myRadix);
	class Node {
	public:
		Node(Node* myparent, UINT count, StaticTreeBarrier* thisParent);
		void await(void);
		void childDone(void);

		UINT child;
		atomic<UINT> childCount;
		Node* parent;
		StaticTreeBarrier* thisParent;
	};
	void build(StaticTreeBarrier::Node* parent, UINT depth);

	// MyBarrier을(를) 통해 상속됨
	virtual void await(void) override;

	virtual void await_th(UINT threadId) override;

	bool sense;
	UINT radix;
	UINT nodes;
	Node* node[MAX_THREAD];
	bool threadSense;

};

class TourBarrier
	:public MyBarrier
{
public:
	TourBarrier(UINT size);
	class Node
	{
	public:
		Node(void);
		Node(Node* myParent);
		Node(Node* myPartner, bool loser);
		bool flag;
		bool active;
		bool top;
		bool leaf;
		Node* parent;
		Node* partner;
		void await(bool sense);
		TourBarrier* outer;
	};
	Node* node[MAX_THREAD];
	bool sense;
	UINT nodes;

	void build(TourBarrier::Node* myNode, UINT depth);
	void buildLoser(TourBarrier::Node* myNode, UINT depth);
	// MyBarrier을(를) 통해 상속됨
	virtual void await(void) override;
	virtual void await_th(UINT threadId) override;
};

class TDBarrier
	:public MyBarrier
{
public:
	class Node
	{
	public:
		Node(void);
		bool flag[2];
		Node* partner;
		UINT myId;
	};
	TDBarrier(UINT n);
	bool sense;
	bool threadSense;
	UINT nodes;
	UINT size;
	UINT logSize;
	Node* node[MAX_THREAD][MAX_THREAD];
	void await_TD(UINT parity, bool mySense, UINT threadId);
	// MyBarrier을(를) 통해 상속됨
	virtual void await(void) override;
	virtual void await_th(UINT threadId) override;
};





