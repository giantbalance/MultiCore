#include "Node.h"

mutex myMutex;
mutex uniqueMutex;

Node::Node()
{
	cstatus = CStatus::ROOT;
	locked = false;
	firstValue = 0;
	secondValue = 0;
	result = 0;
	parent = NULL;
	id = 0;
#if IS_UINT
	counter = 0;
#else
	counter.store(0);
	combinedCounter.store(0);
#endif
	
}

Node::Node(Node* myParent, UINT id)
{
	parent = myParent;
	cstatus = CStatus::IDLE;
	locked = false;
	firstValue = 0;
	secondValue = 0;
	result = 0;
	this->id = id;
#if IS_UINT
	counter = 0;
#else
	counter.store(0);
	combinedCounter.store(0);
#endif
}

bool Node::precombine(condition_variable* cv)
{
	std::unique_lock<std::mutex> lk(uniqueMutex);
	//wait();
	cv->wait(lk, [&] {return !locked; });

	switch (cstatus)
	{
		case CStatus::IDLE:
			cstatus = CStatus::FIRST;
			return true;
		case CStatus::FIRST:
#if MY_DEBUG
			cout << "Precombine Lock(first->second) : " <<this->id <<endl;
#endif
			locked = true;
			cstatus = CStatus::SECOND;
			return false;
		case CStatus::ROOT:
			return false;
		default:
			cout << "Precombine Default : " << (int)cstatus << endl;
	}
	return false;
}

UINT Node::combine(UINT combined, condition_variable* cv)
{
	std::unique_lock<std::mutex> lk(uniqueMutex);
	//wait	
	cv->wait(lk, [&] {return !locked; });
	locked = true;
#if MY_DEBUG
	cout << "Combine Lock : " << this->id << endl;
#endif
	firstValue = combined;
	switch (cstatus)
	{
	case CStatus::FIRST:
		return firstValue;
	case CStatus::SECOND:
		return firstValue + secondValue;
	default:
		cout << "COMBINE Default!" << endl;
	}
	return 0;
}

UINT Node::operation(UINT combined, condition_variable* cv)
{
	std::unique_lock<std::mutex> lk(uniqueMutex);
	int prior = 0;
	switch (cstatus)
	{
	case CStatus::ROOT:
		prior = result;
		result += combined;
		return prior;
	case CStatus::SECOND:
		secondValue = combined;
#if MY_DEBUG
		cout << "operation1 : unlock node : " << this->id << endl;
#endif
		locked = false;
		//notifyAll();
		cv->notify_all();
		// wait();
		cv->wait(lk, [&] {return cstatus == CStatus::RESULT; });
#if MY_DEBUG
		cout << "operation2 : unlock node : " << this->id << endl;
#endif
		locked = false;
		//notifyAll();
		cv->notify_all();
		cstatus = CStatus::IDLE;
		return result;
	default:
		cout << "Operation Default" << endl;
	}
	return 0;
}

void Node::distribute(UINT prior, condition_variable* cv)
{
	std::unique_lock<std::mutex> lk(uniqueMutex);
	switch (cstatus)
	{
	case CStatus::FIRST:
		cstatus = CStatus::IDLE;
#if MY_DEBUG
		cout << "distribute : unlock node : " << this->id << endl;
#endif
		locked = false;
		break;
	case CStatus::SECOND:
		result = prior + firstValue;
		cstatus = CStatus::RESULT;
		break;
	default:
		cout << "Distibute Default" << endl;
	}
	// notifyAll();
	cv->notify_all();
}
