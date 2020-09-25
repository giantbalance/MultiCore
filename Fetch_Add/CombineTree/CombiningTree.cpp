#include"CombiningTree.h"

CombiningTree::CombiningTree(UINT width, UINT thread)
{
	this->width = width;
	this->thread = thread;
	//width = this->width * 2;
	UINT nodeLength = width - 1;
	nodes[0] = new Node();
	
	for (UINT i = 1; i < nodeLength; i++)
	{
		Node* myParent = nodes[(i - 1) / 2];
		nodes[i] = new Node(myParent, i);
	}
	
	UINT leafLength = (width);

	for (UINT i = 0; i < leafLength; i++)
	{
		Node* myParent = nodes[nodeLength - i/2 - 1];
		leaf[i] = new Node(myParent, nodeLength + leafLength - i - 1);
	}
	
}

void CombiningTree::initialize(void)
{
	cout << "Init" << endl;
}

UINT CombiningTree::getAndIncrement(UINT threadId)
{
	stack<Node*>myStack;
	Node* myLeaf = leaf[(threadId / 2) % width];
	Node* node = myLeaf;
	condition_variable* pCv = &cv;
	UINT prior;
	cs.lock();
#if IS_UINT
	if (node->counter < 2)
	{
		node->counter++;
	}
	else
	{
		node->combinedCounter++;
		cs.unlock();
		return 0;
	}

#else
	if (node->counter.load() < 2)
	{
		// combining tree 올라감
		node->counter.fetch_add(1);
	}
	else
	{
		// 값 저장
		node->combinedCounter.fetch_add(1);
		cs.unlock();
#if MY_RESULT
		cout << node->combinedCounter.load() << "save ";
#endif
		return 0;
	}
#endif	

	cs.unlock();

#if MY_DEBUG
	if (node->id == 6)
	{
		cout << "start : threadId " << threadId << " at Node " << node->id << endl;
	}
#endif
	
	// precombine
	while (node->precombine(pCv))
	{
		node = node->parent;
	}
	Node* stop = node;

	//combine
	node = myLeaf;
	UINT combined;
	// 값 전달
#if IS_UINT
	cs.lock();
	combined = node->combinedCounter;
	node->combinedCounter = 1;
	cs.unlock();
#else
	combined = node->combinedCounter.load();
	node->combinedCounter.store(1);
#endif
	while (node != stop)
	{
		combined = node->combine(combined, pCv);
		myStack.push(node);
		node = node->parent;
	}

	// operataion
	prior = stop->operation(combined, pCv);

	// distribute
	while (!myStack.empty())
	{
		node = myStack.top();
		myStack.pop();

		
#if MY_DEBUG
	if (node->id == 6)
	{
	cout << "distribute : threadId " << threadId << " at Node " << node->id << endl;
	}
#endif
		node->distribute(prior, pCv);
	}
#if IS_UINT
	cs.lock();
	node->counter--;
	cs.unlock();
#else
	node->counter.fetch_sub(1);
	
#endif
	
	return prior;
}

void CombiningTree::DoToken(UINT threadId)
{
	UINT res = 0;
	while (MAX_TOKEN > res) 
	{
		res = getAndIncrement(threadId);
#if MY_RESULT
		cout << res << " ";
#endif

	}
}
