#include "new_timer_impl.h"

TimeInfo* Storage::AnnounceStump(TimerStump* st) {
  int tmp = size.fetch_add(1);
  stumps[tmp] = st;
  return &time_info[tmp];
}

void Storage::Print() {
  for (int i = 0; i < size; i++) {
    std::cout << stumps[i]->timerStump << " " << time_info[i].calls.load()
              << " times for " << time_info[i].nanoseconds.load() / 1000000000
              << " seconds " << time_info[i].nanoseconds.load() % 1000000000
              << " nanoseconds." << std::endl;
  }
}

Storage::~Storage() {
  for (int i = 0; i < size; i++) {
    delete (stumps[i]);
  }
  delete[] stumps;
  delete[] time_info;
}

TimerHolder::TimerHolder(std::string st, Storage* storage) {
  stump = new TimerStump(st);
  time_info = storage->AnnounceStump(stump);
}

TimerHolder::~TimerHolder() {}

Timer::Timer(TimerHolder* holder) {
  info_target = holder->time_info;
  start = std::chrono::steady_clock::now();
}
Timer::~Timer() {
  end = std::chrono::steady_clock::now();
  std::chrono::nanoseconds elapsed_time = end - start;
  long long int nano = elapsed_time.count();
  info_target->calls.fetch_add(1);
  info_target->nanoseconds.fetch_add(nano);
}

Aggregator& Aggregator::GetInstance() {
  static Aggregator instance;
  return instance;
}

Timer* Aggregator::CreateTimer(TimerHolder* st) { return new Timer(st); }

Aggregator::~Aggregator() { storage.Print(); }
