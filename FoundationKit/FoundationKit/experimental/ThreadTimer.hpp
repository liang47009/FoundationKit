


#include <functional>
#include <thread>
#include <atomic>
#include <stdexcept>

class ThreadTimer
{
public:
	ThreadTimer()
	{

	}

	virtual ~ThreadTimer()
	{

	}

	ThreadTimer& SetTimeInterval(int interval)
	{
		if (interval < 0)
			throw std::invalid_argument("setInterval value must be > 0");
		TimeInterval = interval;
		return (*this);
	}


	ThreadTimer& SetEnable(bool bEnable)
	{

	}


	void Start()
	{
		if (LoopThread.native_handle() == nullptr)
		{
			LoopThread = std::thread([]() 
			{

			});
		}
	}

	void Stop()
	{

	}


private:
	std::atomic_int  FrameCount;
	std::atomic_int  TimeInterval; // expressed in milliseconds
	std::atomic_bool Enable;
	std::thread      LoopThread;
};

