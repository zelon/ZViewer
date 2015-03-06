#pragma once

#include <queue>

#include "../../commonSrc/LockUtil.h"
#include "JobInterface.h"

class JobExecutor final {
public:
  JobExecutor();

  void StartThread(const size_t thread_count);
  void StopThread();

  void Add(JobInterfacePtr job);

private:
  void Run();

  mutable Lock lock_;
  mutable ConditionalVariable conditional_variable_;

  volatile bool go_on_ = false;

  std::vector<std::thread*> thread_list_;
  std::queue<JobInterfacePtr> job_queue_;
};