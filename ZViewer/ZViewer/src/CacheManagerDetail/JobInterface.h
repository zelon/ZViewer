#pragma once

class JobInterface {
public:
  virtual ~JobInterface() {}

  virtual void DoJob() = 0;
};

typedef std::shared_ptr<JobInterface> JobInterfacePtr;

