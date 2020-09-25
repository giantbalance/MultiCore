#include "LockFreeList.h"
#include "FineList.h"
#include "CoarseList.h"
#include "LazyList.h"

using namespace std;
using std::thread;

#define MAX_THREAD 32

void GetElapsedTime(double start)
{
    double end = clock();
    std::cout << "Time : " << (end - start) / CLOCKS_PER_SEC << std::endl;
}

int main()
{
    vector<SyncList*> listClasses;
    std::vector<std::thread> workerThreads;

    listClasses.emplace_back(new CoarseList());
    listClasses.emplace_back(new FineList());
    listClasses.emplace_back(new LazyList());
    listClasses.emplace_back(new LockFreeList());

    for (auto classes : listClasses)
    {
        cout << "task1" << endl;
        cout << typeid(*classes).name() << endl;

        for (size_t i = 4; i <= MAX_THREAD; i *= 2)
        {
            for(size_t cnt = 1; cnt <= i; cnt++)
                workerThreads.push_back(std::thread(&SyncList::test, classes, cnt,i));
            double start = clock();
            for (size_t cnt = 0; cnt < i; cnt++)
            {
                workerThreads[cnt].join();
            }
            cout << "Core : " << i << endl;
            GetElapsedTime(start);
            workerThreads.clear();
            classes->testPrint();
            classes->clear();
        }

        cout << endl << endl;
    }

    for (auto classes : listClasses)
    {
        cout << "task2" << endl;
        cout << typeid(*classes).name() << endl;

        for (size_t i = 4; i <= MAX_THREAD; i *= 2)
        {
            for (size_t cnt = 1; cnt <= i; cnt++)
                workerThreads.push_back(std::thread(&SyncList::test2, classes, cnt, i));
            double start = clock();
            for (size_t cnt = 0; cnt < i; cnt++)
            {
                workerThreads[cnt].join();
            }
            cout << "Core : " << i << endl;
            GetElapsedTime(start);
            workerThreads.clear();
            classes->testPrint();
            classes->clear();
        }
        cout << endl << endl;
    }
    listClasses.clear();
}   
