#include "common.h"

bool SyncList::add(unsigned int data)
{
	return false;
}

bool SyncList::remove(unsigned int data)
{
	return false;
}

bool SyncList::contain(unsigned int data)
{
	return false;
}

void SyncList::testPrint()
{
}

void SyncList::clear()
{
}


void SyncList::test(int thrd_id, int num_thd)
{
	int count = 0;
	int data = 0;
	int op;
	int loop_per_thread;

	loop_per_thread = MAX_COUNT / num_thd;

	srand(thrd_id);

	while (count < loop_per_thread)
	{
		op = rand() % 100;
		if (op < ADD_RATIO) {
			add(rand() % MAX_VAL);
		}
		else if (op < 95) {
			contain(rand() % MAX_VAL); //Task1�� ��ġ�� Task2 �� ���� Line 21�� Line 23�� switch�Ͽ� �����Ѵ�.
		}
		else {
			remove(rand() % MAX_VAL);
		}
		count++;
	}
}

void SyncList::test2(int thrd_id, int num_thd)
{
	int count = 0;
	int data = 0;
	int op;
	int loop_per_thread;

	loop_per_thread = MAX_COUNT / num_thd;

	srand(thrd_id);

	while (count < loop_per_thread)
	{
		op = rand() % 100;
		if (op < ADD_RATIO) {
			add(rand() % MAX_VAL);
		}
		else if (op < 95) {
			remove(rand() % MAX_VAL);
		}
		else {
			contain(rand() % MAX_VAL); //Task1�� ��ġ�� Task2 �� ���� Line 21�� Line 23�� switch�Ͽ� �����Ѵ�.
		}
		count++;
	}
}
