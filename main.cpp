#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>

class PingPong
{
public:
	static constexpr std::size_t MAX = 3;

	void ping()
	{
    	std::unique_lock<std::mutex> lock(m_);
    	while (count_.load() < MAX)
    	{
    	    
        	std::cout << "Ping" << std::endl;
        	count_++;
        	cv_.notify_all();
        	cv_.wait(lock,[this]{return count_.load() % 2 == 0;});
        	
    	}

        count_++;
        cv_.notify_all();
 	}

	void pong()
	{
    	std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock,[this]{return count_.load() % 2 == 1;});
    	while (count_.load() < MAX)
    	{
        	std::cout << "Pong" << std::endl;
        	count_++;
        	cv_.notify_all();
        	cv_.wait(lock,[this]{return count_.load() % 2 == 1;});
    	}
    	
    	count_++;
        cv_.notify_all();
	}

private:
	std::atomic<std::size_t> count_ = 0;
	std::mutex m_;
	std::condition_variable cv_;
};

int main()
{
	PingPong p;
	std::thread pingThread(&PingPong::ping, &p);
	std::thread pongThread(&PingPong::pong, &p);

	pingThread.join();
	pongThread.join();
}
