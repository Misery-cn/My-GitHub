#include "timer.h"
#include "log.h"

class TimerThread : public Thread
{
public:
	TimerThread(Timer* t) : _timer(t)
	{
	}
	
	void entry()
	{
		_timer->timer_thread();
		return;
	}
	
private:
	Timer* _timer;
};

void Timer::init()
{
	_stopping = false;
	_thread = new TimerThread(this);
	_thread->create();
}

void Timer::shutdown()
{
	if (_thread)
	{
		cancel_all_events();
		_stopping = true;
		_cond.signal();
		_lock.unlock();
		_thread->join();
		_lock.lock();
		
		delete _thread;
		_thread = NULL;
	}
}

void Timer::timer_thread()
{
	_lock.lock();

	DEBUG_LOG("timer thread starting");

	while (!_stopping)
	{
		utime_t now = clock_now();

		while (!_schedule.empty())
		{
			schedule_iter i = _schedule.begin();

			if (i->first > now)
			{
				break;
			}

			Callback* cb = i->second;
			_events.erase(cb);
			_schedule.erase(i);
			
			// ִ�лص�����
			cb->complete(0);
		}

		// ֹͣ
		if (_stopping)
		{
			break;
		}

		// ����
		if (_schedule.empty())
		{
			DEBUG_LOG("timer will wait");
			_cond.wait(_lock);
		}
		else
		{
			// ע����Ҫת�ɺ���
			_cond.timed_wait(_lock, (_schedule.begin()->first - now).to_msec());
		}

		DEBUG_LOG("timer thread awake");
	}
	
	_lock.unlock();
}

void Timer::add_event_after(int64_t seconds, Callback* cb)
{
	utime_t when = clock_now();
	when += seconds;
	add_event_at(when, cb);
}


void Timer::add_event_at(utime_t when, Callback* cb)
{
	std::multimap<utime_t, Callback*>::value_type sval(when, cb);
	schedule_iter i = _schedule.insert(sval);

	std::map<Callback*, schedule_iter>::value_type eval(cb, i);
	std::pair<event_iter, bool> ret = _events.insert(eval);
	if (!ret.second)
	{
		// ERROR
	}

	// ���ֻ��һ�������ִ��,����Ҫ����ִ���߳�
	// ����ж������,ִ���߳�ֻ��time_wait
	if (i == _schedule.begin())
	{
		_cond.signal();
	}
}

bool Timer::cancel_event(Callback* cb)
{
	event_iter i = _events.find(cb);
	if (i == _events.end())
	{
		return false;
	}

	if (i->first)
	{
		delete i->first;
	}

	_schedule.erase(i->second);
	_events.erase(i);
	
	return true;
}

void Timer::cancel_all_events()
{
	while (!_events.empty())
	{
		event_iter i = _events.begin();
		
		if (i->first)
		{
			delete i->first;
		}
		
		_schedule.erase(i->second);
		_events.erase(i);
	}
}
