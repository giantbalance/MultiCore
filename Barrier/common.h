#pragma once

#ifndef COMMON_H
#define COMMON_H


#include <iostream>
#include <thread>
#include <time.h>
#include <vector>
#include <queue>
#include <atomic>
#include <mutex>
#include <stack>
#include <condition_variable>
#include <deque>
#include <math.h>

#define MY_DEBUG false
#define TEST_CASE 8
#define MAX_THREAD 20000
#define RADIX 2
#define TOUR 2
#define MAX_NODE (MAX_THREAD/RADIX)
#define TRY 10

#define SENSE_BARRIER true
#define CTREE_BARRIER true
#define STATIC_BARRIER true
#define TOUR_BARRIER true
#define DISSEMINATION_BARRIER true

using namespace std;
using namespace chrono;

typedef unsigned int UINT;

#endif

