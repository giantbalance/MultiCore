#include "SenseBarrier.h"

thread_local bool mySense;

SenseBarrier::SenseBarrier(UINT n)
{
	count.store(n);
	size = n;
	sense = false;
	mySense = !sense;
}

void SenseBarrier::await(void)
{
	UINT position = count.fetch_sub(1);
	if (1 == position)
	{
		count.store(this->size);
		sense.store(mySense);
		mySense = !sense.load();
	}
	else
	{
		while (sense != mySense) {}
	}
}

void SenseBarrier::await_th(UINT threadId)
{
}

CombiningTreeBarrier::CombiningTreeBarrier(UINT size, UINT radix)
{
	this->radix = radix;
	this->size = size;
	leaves = 0;
	UINT depth = 0;
	mySense = true;
	while (size > 1)
	{
		depth++;
		size = size / radix;
	}
	Node* root = new Node(this->radix);
	build(root, depth - 1);
}


void CombiningTreeBarrier::build(CombiningTreeBarrier::Node *parent, UINT depth)
{
	if (depth == 0)
	{
		myleaf[leaves++] = parent;
	}
	else
	{
		for (UINT i = 0; i < radix; i++)
		{
			Node* child = new Node(this->radix, parent);
			build(child, depth - 1);
		}
	}
}

void CombiningTreeBarrier::await(void)
{
}

void CombiningTreeBarrier::await_th(UINT threadId)
{
	myleaf[threadId / radix]->Node_await();
}

void CombiningTreeBarrier::Node::Node_await(void)
{
	int position = count.fetch_sub(1);
	if (position == 1)
	{
		if (parent != NULL)
		{
			parent->Node_await();
		}
		count.store(size);
		sense = mySense;
		mySense = !sense;
	}
	else
	{
		while (sense != mySense) {};
	}
}

StaticTreeBarrier::Node::Node(Node* myparent, UINT count, StaticTreeBarrier* thisParent)
{
	childCount = count;
	this->child = count;
	parent = myparent;
	this->thisParent = thisParent;
}

void StaticTreeBarrier::Node::await(void)
{
	bool mySense = thisParent->threadSense;
	while (childCount.load() > 0) {};
	childCount.store(child);
	if (parent != NULL)
	{
		parent->childDone();
		while (thisParent->sense != mySense) {};
	}
	else
	{
		thisParent->sense = mySense;
	}
	thisParent->threadSense =!mySense;
}

void StaticTreeBarrier::Node::childDone(void)
{
	childCount.fetch_sub(1);
}

StaticTreeBarrier::StaticTreeBarrier(UINT size, UINT myRadix)
{
	this->radix = myRadix;
	this->size = size;
	this->sense = false;
	this->threadSense = !this->sense;
	this->nodes = 0;
	UINT depth = 0;
	while (size > 1)
	{
		depth++;
		size = size / radix;
	}
	
	build(NULL, depth);
}

void StaticTreeBarrier::build(StaticTreeBarrier::Node* parent, UINT depth)
{
	if (depth == 0)
	{
		node[nodes++] = new Node(parent, 0, this);
	}
	else
	{
		Node* myNode = new Node(parent, this->radix, this);
		node[nodes++] = myNode;
		for (UINT i = 0; i < radix; i++)
		{
			build(myNode, depth - 1);
		}
	}
}

void StaticTreeBarrier::await(void)
{
}

void StaticTreeBarrier::await_th(UINT threadId)
{
	node[threadId]->await();
}

TourBarrier::TourBarrier(UINT size)
{
	this->size = size;
	nodes = 0;
	UINT depth = 0;
	sense = true;
	while (size > 1)
	{
		depth++;
		size = size / TOUR;
	}
	Node* root = new Node();
	root->top = true;
	root->outer = this;
	build(root, depth - 1);
}

void TourBarrier::build(TourBarrier::Node* myNode, UINT depth)
{
	if (depth == 0)
	{
		myNode->leaf = true;
		node[nodes++] = myNode;
	}
	else
	{
		Node* child = new Node(myNode);
		Node* loser = new Node(child, false);
		child->partner = loser;
		node[nodes++] = myNode;
		build(child, depth - 1);
		build(loser, depth - 1);
	}
}

void TourBarrier::buildLoser(TourBarrier::Node* myNode, UINT depth)
{
}

void TourBarrier::await(void)
{
}

void TourBarrier::await_th(UINT threadId)
{
	bool sense = this->sense;
	TourBarrier::Node *temp = node[threadId];
	if (temp->leaf == true)
	{
		node[threadId]->await(sense);
	}

}

TourBarrier::Node::Node(void)
{
	flag = false;
	active = false;
	partner = NULL;
	parent = NULL;
	top = false;
	leaf = false;
}

TourBarrier::Node::Node(Node* myParent)
{
	flag = false;
	active = false;
	partner = NULL;
	parent = NULL;
	top = false;
	leaf = false;
	parent = myParent;
	active = true;
}

TourBarrier::Node::Node(Node* myPartner, bool loser)
{
	flag = false;
	active = false;
	partner = NULL;
	parent = NULL;
	top = false;
	leaf = false;
	partner = myPartner;
}

void TourBarrier::Node::await(bool sense)
{
	if (top)
	{
		this->outer->sense = !sense;
	}
	else
	{
		if (parent != NULL)
		{
			// winner
			while (flag != sense) {};
			parent->await(sense);
			partner->flag = sense;
		}
		else
		{
			// loser
			partner->flag = sense;
			while (flag != sense) {};
		}
	}

}


TDBarrier::TDBarrier(UINT count)
{
	this->size = count;
	UINT d = 1;
	this->logSize = log(count);
	for (UINT i = 0; i < count; i++)
	{
		for (UINT j = 0; j < logSize; j++)
		{
			node[i][j] = new Node();
		}
	}
	
	for (UINT j = 0; j < logSize; j++)
	{
		for (UINT i = 0; i < count; i++)
		{
			node[i][j]->partner = node[(i + d) % count][j];
			node[i][j]->myId = (i + d) % count;
		}
		d = 2 * d;
	}
	sense = false;
	threadSense = true;
}

TDBarrier::Node::Node(void)
{
	flag[0] = false;
	flag[1] = false;
	partner = NULL;
}

void TDBarrier::await_TD(UINT parity, bool mySense, UINT threadId)
{
	for (UINT r = 0; r < logSize; r++)
	{
		node[threadId][r]->partner->flag[parity] = mySense;
		while (node[threadId][r]->flag[parity] != mySense) {}
	}
	
}

void TDBarrier::await(void)
{
}

void TDBarrier::await_th(UINT threadId)
{

}
