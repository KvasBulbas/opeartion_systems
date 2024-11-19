#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <chrono>
#include <ctime>

using namespace std;

mutex mut;
condition_variable cv;

list<int> numberList;

bool ready = false;

void provide(int num)
{
    unique_lock<mutex> ul(mut);

    numberList.push_back(num);
    cout << "thread: " << this_thread::get_id << " provided a number: " << num << " at list\n";

    ready = true;
    cv.notify_one();

    this_thread::sleep_for(chrono::seconds(1));

    ul.unlock();
}

void consume()
{
    unique_lock<mutex> ul(mut);

    cv.wait(ul, [] { return ready; });
    ready = false;

    cout << "thread: " << this_thread::get_id << " consumed a number: " << numberList.back() << " from list\n\n";

    ul.unlock();
}

int main()
{
    srand(time(NULL));
    for (int i = 0; i < 5; i++)
    {
        thread consumer(consume);
        thread provider(provide, rand() & 100);

        provider.join();
        consumer.join();
    }
}