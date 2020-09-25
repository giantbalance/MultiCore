#include "CombiningTree.h"
#include "common.h"

void GetElapsedTime(system_clock::time_point chr_start)
{
    system_clock::time_point chr_end = system_clock::now();
    duration<double> execTime = duration_cast<microseconds>(chr_end - chr_start);
    std::cout << "Time : " << execTime.count() << std::endl;
}


int main()
{
    vector<CombiningTree*> listClasses;
    std::vector<std::thread> workerThreads;
    UINT threads[4] = { 18,36,72,144 };
    UINT widths[4] = { 4,8,16,32 };
    for (int i = 0; i <4 ; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            listClasses.emplace_back(new CombiningTree(widths[i],threads[j]));
        }
        
    }

    for (auto classes : listClasses)
    {
        UINT numThread = classes->thread;
        for (size_t cnt = 0; cnt < numThread; cnt++)
            workerThreads.emplace_back(std::thread(&CombiningTree::DoToken, classes, cnt));
        system_clock::time_point chr_start = system_clock::now();
        for (size_t cnt = 0; cnt < numThread; cnt++)
        {
            workerThreads[cnt].join();
        }
        GetElapsedTime(chr_start);
        cout << "Thread : " << numThread << "Width : "<< classes->width << endl;
        workerThreads.clear();
        delete classes;
        cout << endl << endl;
    }
    listClasses.clear();
}

