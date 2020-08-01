#include "stdafx.h"
#include "EventManager.h"

#include "../CacheManager.h"
#include "ImageLoadJob.h"
#include "ZImage.h"

EventManager::EventManager() : error_image_(new ZImage) {
  InitErrorImage();
}

void EventManager::StartThread() {
  file_read_executor_.StartThread(1);

  const size_t decode_thread_count = GetProcessorNumber() / 2;
  decode_executor_.StartThread(decode_thread_count);
}

void EventManager::StopThread() {
  file_read_executor_.StopThread();
  decode_executor_.StopThread();
}

void EventManager::AddJob(const tstring& filename) {
  LockGuard guard(lock_);

  if (jobs_.count(filename) > 0) {
    return;
  }

  JobInterfacePtr job(new ImageLoadJob(this, error_image_, filename));
  jobs_.insert(make_pair(filename, job));

  file_read_executor_.Add(job);
}

void EventManager::DoDecodeJob(const tstring& filename) {
  LockGuard guard(lock_);
  auto it = jobs_.find(filename);
  if (it == jobs_.end()) {
    return;
  }
  decode_executor_.Add(it->second);
}

void EventManager::OnDecodeCompleted(const tstring& filename) {
  LockGuard guard(lock_);

  auto it = jobs_.find(filename);
  if (it == jobs_.end()) {
    return;
  }

  JobInterfacePtr job = it->second;
  std::shared_ptr<ZImage> image = static_cast<ImageLoadJob*>(job.get())->image();

  CacheManager::GetInstance().OnDecodeCompleted(filename, image);

  jobs_.erase(it);
}

void EventManager::InitErrorImage() {
  const tstring error_image_filename = GetProgramFolder() + TEXT("LoadError.png");
  if (error_image_->LoadFromFile(error_image_filename)) {
  } else {
    MessageBox(HWND_DESKTOP, TEXT("Please check LoadError.png in ZViewer installed folder"), TEXT("ZViewer"), MB_OK);
    const HBITMAP hBitmap = CreateBitmap(100, 100, 1, 1, NULL);
    error_image_->CopyFromBitmap(hBitmap);
  }
}

