#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED
#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#define FUNC_LOCATION             \
  [](auto fn, auto ln) {          \
    std::stringstream ss;         \
    ss << fn << ":" << ln << " "; \
    return ss.str();              \
  }(__func__, __LINE__)

#define SetTimer                          \
  static TimerStump stump(FUNC_LOCATION); \
  std::unique_ptr<Timer> tm(Aggregator::GetInstance().CreateTimer(&stump));

std::mutex mutex;

class TimerStump {
 public:
  std::string timerStump;

 public:
  TimerStump(std::string stump) : timerStump(stump) {}
  TimerStump() = delete;
};

class Table {
 public:
  Table() = default;
  ~Table() = default;

 private:
  struct Raw {
    TimerStump* id;
    float workTime;
    int calls;
  };

 public:
  size_t findOrAddRaw(TimerStump* stump) {
    if (!count) {
      tableRaws.push_back({stump, 0., 0});
      functionIdentifier2idx[stump] = count;
      count++;
      return 0;
    } else {
      auto findRes = functionIdentifier2idx.find(stump);
      if (findRes != functionIdentifier2idx.end()) {
        return findRes->second;
      }
      tableRaws.push_back({stump, 0., 0});
      functionIdentifier2idx[stump] = count;
      count++;
      return count - 1;
    }
  }

  void WriteCalls(int idx, float time) {
    tableRaws[idx].calls++;
    tableRaws[idx].workTime += time;
  }

  void PrintTable() {
    for (int i = 0; i < count; i++) {
      std::cout << tableRaws[i].id->timerStump << " " << tableRaws[i].calls
                << " times for " << tableRaws[i].workTime << std::endl;
    }
  }

 private:
  std::vector<Raw> tableRaws;
  std::map<TimerStump*, size_t> functionIdentifier2idx;
  size_t count = 0;
};

class Timer {
 public:
  Timer(TimerStump* st, Table& tableRef) : table(tableRef) {
    mutex.lock();
    idx = table.findOrAddRaw(st);
    start = std::chrono::steady_clock::now();
    mutex.unlock();
  }
  Timer() = delete;

  ~Timer() {
    mutex.lock();
    end = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed_time = end - start;
    table.WriteCalls(idx, elapsed_time.count());
    mutex.unlock();
  }

 private:
  Table& table;
  size_t idx;
  std::chrono::time_point<std::chrono::steady_clock> start;
  std::chrono::time_point<std::chrono::steady_clock> end;
};

static Table* TABLE;

class Aggregator {
 public:
  static Aggregator& GetInstance() {
    static Aggregator instance;
    return instance;
  }

  static Timer* CreateTimer(TimerStump* st) { return new Timer(st, *TABLE); }

 public:
  ~Aggregator() {
    TABLE->PrintTable();
    delete TABLE;
  }

 private:
  Aggregator() { TABLE = new Table; }
  Aggregator(const Aggregator&) = delete;
  Aggregator& operator=(const Aggregator&) = delete;
};

#endif
