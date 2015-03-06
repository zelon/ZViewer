#pragma once

#include "../../commonSrc/LockUtil.h"
#include "JobExecutor.h"
#include "JobInterface.h"

class ZImage;

class EventManager final {
public:
  EventManager();

  void StartThread();
  void StopThread();

  void AddJob(const tstring& filename);
  bool IsInJob(const tstring& filename) const;

  void DoDecodeJob(const tstring& filename);
  void OnDecodeCompleted(const tstring& filename);

private:
  void InitErrorImage();

  mutable Lock lock_;
  std::unordered_map<tstring/*filename*/, std::shared_ptr<JobInterface>> jobs_;

  JobExecutor file_read_executor_;
  JobExecutor decode_executor_;

  std::shared_ptr<ZImage> error_image_;
};