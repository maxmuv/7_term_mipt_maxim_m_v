#include <iostream>
#include <thread>
#include "timer.h"

void BOO() {
  SetTimer;
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

void FOO() {
  SetTimer;
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

void tenFoo() {
  for (int i = 0; i < 10; i++) {
    FOO();
  }
}

int main(int argc, char *argv[]) {
  std::thread foo1(tenFoo);
  std::thread foo2(tenFoo);
  std::thread foo3(tenFoo);
  SetTimer;
  foo1.join();
  foo2.join();
  foo3.join();
  return 0;
}
