#pragma once
#include "common.h"

class MyBarrier
{
public:
	virtual void await(void) = 0;
	virtual void await_th(UINT threadId) = 0;
	UINT size;
};

