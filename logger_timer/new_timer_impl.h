#ifndef NEW_TIMER_IMPL_H_INCLUDED
#define NEW_TIMER_IMPL_H_INCLUDED
#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <thread>
#include <vector>

#define FUNC_LOCATION             \
  [](auto fn, auto ln) {          \
    std::stringstream ss;         \
    ss << fn << ":" << ln << " "; \
    return ss.str();              \
  }(__func__, __LINE__)

#ifdef USE
#define SetTimer                                                 \
  static TimerHolder holder(FUNC_LOCATION,                       \
                            &Aggregator::GetInstance().storage); \
  std::unique_ptr<Timer> tm(Aggregator::GetInstance().CreateTimer(&holder));
#endif
#ifndef USE
#define SetTimer
#endif

class TimerStump {
 public:
  std::string timerStump;

 public:
  TimerStump(std::string stump) : timerStump(stump) {}
  TimerStump() = delete;
};

struct TimeInfo {
  std::atomic<int> calls;
  std::atomic<int> milliseconds;
};

const int STORAGE_SIZE = 100000;

class Storage {
 public:
  Storage() {
    time_info = new TimeInfo[STORAGE_SIZE];
    stumps = new TimerStump*[STORAGE_SIZE];
    size = 0;
  }

  TimeInfo* AnnounceStump(TimerStump* st);
  void Print();
  ~Storage();

 private:
  TimeInfo* time_info;
  TimerStump** stumps;
  std::atomic<int> size;
};

class TimerHolder {
 public:
  TimerHolder() = delete;
  TimerHolder(std::string st, Storage* storage);
  ~TimerHolder();

 public:
  TimerStump* stump;
  TimeInfo* time_info;
};

class Timer {
 public:
  Timer() = delete;
  Timer(TimerHolder* holder);
  ~Timer();

 private:
  TimeInfo* info_target;
  std::chrono::time_point<std::chrono::steady_clock> start;
  std::chrono::time_point<std::chrono::steady_clock> end;
};

class Aggregator {
 public:
  static Aggregator& GetInstance();
  static Timer* CreateTimer(TimerHolder* st);
  ~Aggregator();

 private:
  Aggregator() = default;
  Aggregator(const Aggregator&) = delete;
  Aggregator& operator=(const Aggregator&) = delete;

 public:
  Storage storage;
};
#endif
