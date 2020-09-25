#include <iostream>
#include <atomic>
#include <thread>
#include <time.h>
#include <mutex>
#include <chrono>
#include <malloc.h>
#include <cstring>

using namespace std;
using namespace chrono;

#define TOKEN 1000000
int thd[]={18,36,72,144};
int width[]={4,8,16,32};


class Balancer
{
	atomic<bool> toggle; 

	public:
	Balancer()
	{
		toggle=true;
	}
	int traverse()
	{
		//old = exchange하기 전의 toggle 값
		int old=toggle.exchange(!toggle);
		if(old) 
			return 0;
		else
			return 1;
	}
};

class Merger
{
	public:
		Merger *half;
		Balancer *layer;
		int width;

	public:
		Merger(int m_width)
		{
			width = m_width;
			layer = new Balancer[width / 2];
			if (width > 2)
				half = new Merger[2]{Merger(width / 2), Merger(width / 2)};
		}
		int traverse(int input){
			if(width==2){
				return layer[0].traverse();
			}
			int output=0;
			if(input<width/2){
				output=half[input%2].traverse(input/2);
			}
			else{
				output=half[1-(input%2)].traverse(input/2);
			}
			return (2*output)+layer[output].traverse();
		}
};

class Bitonic{
	Bitonic *half;
	Merger *merger;
	int width;
	public:
	Bitonic(int m_width){
		width=m_width;
		merger=new Merger(width);
		if(width>2)
			half=new Bitonic[2]{Bitonic(width/2),Bitonic(width/2)};
	}
	int traverse(int input){
		int output=0;
		if(width>2){
			output=half[input/(width/2)].traverse(input%(width/2));
		}
		
		return merger->traverse((input>=(width/2)?(width/2):0)+output);		
		
	}

};

void test(Bitonic *b,int thread_id,int num_of_input,int w_idx)
{
	/*
	   각 쓰레드당 num of input만큼의 토큰을 수행하는데
	   예를 들면 thread0는 0~10만큼 돌고
	   thread1은 11~20만큼 돌고 
	   등등 이렇게 수행되도록 만듬
	   */
	for(int i=thread_id*num_of_input;i<(thread_id+1)*num_of_input;i++){
		/*
		//print잘 나오는지 lock걸면서 봤음
		lock_obj.lock();
		cout<<b->traverse(i%width[w_idx])<<endl;
		lock_obj.unlock();
		*/
		//바이토닉의 width를 넘지 않도록 하기 위해서 %width[w_idx]를 추가함
		b->traverse(i%width[w_idx]);
	}

}
//마지막 thread는 test()대신 run()을 동작한다.
void run(Bitonic *b,int thread_id,int start,int w_idx)
{
	for(int i=start;i<TOKEN;i++)
	{
		b->traverse(i%width[w_idx]);
	}
}




int main()
{
	for (int tn = 0; tn < 4; tn++)
	{

		thread *t;
		t = new thread[thd[tn]];
		for (int w = 0; w < 4; w++)
		{
			Bitonic *btn = new Bitonic(width[w]);
			system_clock::time_point chr_start = system_clock::now();
			for (int i = 0; i < thd[tn]-1; i++)
			{
				t[i] = thread(test, btn, i, TOKEN/thd[tn],w);
			}
			t[thd[tn]-1]=thread(run, btn, thd[tn]-1, TOKEN/thd[tn]*(thd[tn]-1),w);

			for (int i = 0; i < thd[tn]; i++)
			{
				t[i].join();
			}
			system_clock::time_point chr_end = system_clock::now();
			duration<double> execTime = duration_cast<microseconds>(chr_end - chr_start);

			cout << "[# of thread: " << thd[tn] <<"& width: "<<width[w]<< "]" << endl;
			cout << "execution time: " << execTime.count() << endl;
			cout<<"-------------------------------------"<<endl;
			delete btn;
		}

	}

}
