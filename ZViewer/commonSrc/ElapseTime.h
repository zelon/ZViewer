#pragma once

#include <chrono>

class ElapseTime final {
public:
  ElapseTime() {
    start_time_ = std::chrono::system_clock::now();
  }

  long long End() {
    using namespace std::chrono;
    const milliseconds ms = duration_cast<milliseconds>(system_clock::now() - start_time_);
    return ms.count();
  }

private:
  std::chrono::time_point<std::chrono::system_clock> start_time_;
};
