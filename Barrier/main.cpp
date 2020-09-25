// Barrier_console.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "SenseBarrier.h"



void GetElapsedTime(system_clock::time_point chr_start)
{
    system_clock::time_point chr_end = system_clock::now();
    duration<double> execTime = duration_cast<microseconds>(chr_end - chr_start);
    std::cout << "Time : " << execTime.count() << std::endl;
}


int main()
{
    vector<MyBarrier*> listClasses;
    std::vector<std::thread> workerThreads;
#if MY_DEBUG
    UINT threads[TEST_CASE] = { 4,8,16,32,4,4,4,4 };
#else
    UINT threads[TEST_CASE] = { 128,256, 512,1024, 2048, 4096, 8192, 16384 };
#endif

#if SENSE_BARRIER
    for (int i = 0; i < TEST_CASE; i++)
    {
        listClasses.emplace_back(new SenseBarrier(threads[i]));
    }

    for (auto classes : listClasses)
    {
        UINT num_threads = classes->size;
        cout << "Sense" << endl;
        system_clock::time_point chr_start = system_clock::now();
        for (UINT i = 0; i < TRY; i++)
        {
            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads.emplace_back(std::thread(&MyBarrier::await, classes));
            }

            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads[cnt].join();
            }
            workerThreads.clear();
        }
        GetElapsedTime(chr_start);

        delete classes;
    }
    listClasses.clear();
#endif
#if CTREE_BARRIER
    cout << "CTREE" << endl;
    for (int i = 0; i < TEST_CASE; i++)
    {
        listClasses.emplace_back(new CombiningTreeBarrier(threads[i],(UINT)RADIX));
    }
    

    for (auto classes : listClasses)
    {
        UINT num_threads = classes->size;
        system_clock::time_point chr_start = system_clock::now();
        for (UINT i = 0; i < TRY; i++)
        {
            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads.emplace_back(std::thread(&MyBarrier::await_th, classes, cnt));
            }
            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads[cnt].join();
            }
            workerThreads.clear();
        }
        GetElapsedTime(chr_start);

        delete classes;
        cout << endl << endl;
    }
    listClasses.clear();
#endif

#if STATIC_BARRIER

    cout << "STATIC" << endl;
    for (int i = 0; i < TEST_CASE; i++)
    {
        listClasses.emplace_back(new StaticTreeBarrier(threads[i]/RADIX, (UINT)RADIX));
    }


    for (auto classes : listClasses)
    {
        UINT num_threads = classes->size * RADIX - 1;
        system_clock::time_point chr_start = system_clock::now();
        for (UINT i = 0; i < TRY; i++)
        {
            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads.emplace_back(std::thread(&MyBarrier::await_th, classes, cnt));
            }
            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads[cnt].join();
            }
            workerThreads.clear();
        }
        GetElapsedTime(chr_start);

        delete classes;
        cout << endl << endl;
    }
    listClasses.clear();
#endif

#if TOUR_BARRIER

    cout << "TOUR" << endl;
    for (int i = 0; i < TEST_CASE; i++)
    {
        listClasses.emplace_back(new TourBarrier(threads[i]/TOUR));
    }


    for (auto classes : listClasses)
    {
        UINT num_threads = classes->size - 1;
        system_clock::time_point chr_start = system_clock::now();
        for (UINT i = 0; i < TRY; i++)
        {
            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads.emplace_back(std::thread(&MyBarrier::await_th, classes, cnt));
            }

            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads[cnt].join();
            }
            workerThreads.clear();
        }
        GetElapsedTime(chr_start);

        delete classes;
        cout << endl << endl;
    }
    listClasses.clear();
#endif

#if DISSEMINATION_BARRIER
    cout << "DISS" << endl;
    vector<TDBarrier*> listTDClasses;
    for (int i = 0; i < TEST_CASE; i++)
    {
        listTDClasses.emplace_back(new TDBarrier(threads[i]));
    }


    for (auto classes : listTDClasses)
    {
        UINT num_threads = classes->size;
        system_clock::time_point chr_start = system_clock::now();
        for (UINT i = 0; i < TRY; i++)
        {
            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads.emplace_back(std::thread(&TDBarrier::await_TD, classes, 0, true, cnt));
            }

            for (size_t cnt = 0; cnt < num_threads; cnt++)
            {
                workerThreads[cnt].join();
            }
            workerThreads.clear();
        }
        GetElapsedTime(chr_start);

        delete classes;
        cout << endl << endl;
    }
    listTDClasses.clear();
#endif
}

