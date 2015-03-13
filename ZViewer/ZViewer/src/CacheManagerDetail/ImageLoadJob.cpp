#include "stdafx.h"
#include "ImageLoadJob.h"

#include "../../commonSrc/CommonFunc.h"
#include "../../commonSrc/ElapseTime.h"
#include "EventManager.h"
#include "../CacheManagerDetail/FileReader.h"
#include "ZImage.h"
#include "ZOption.h"

ImageLoadJob::ImageLoadJob(EventManager* event_manager, std::shared_ptr<ZImage> error_image,
    const tstring& filename)
  : event_manager_(event_manager), filename_(filename), state_(State::kReadyToFileRead)
  , only_filename_(GetFileNameFromFullFileName(filename))
  , error_image_(error_image)
  , image_(new ZImage) {
  assert(error_image_ != nullptr);
}

void ImageLoadJob::DoJob() {
  State old_state = state_;
  switch (state_) {
  case State::kReadyToFileRead:
    ReadFile();
    break;
  case State::kReadyToDecode:
    Decode();
    break;
  default:
    assert(false);
  }

  State new_state = state_;
  if (new_state == old_state) {
    assert(false);
  }
  switch (new_state) {
  case State::kReadyToDecode:
    event_manager_->DoDecodeJob(filename_);
    break;
  case State::kReadyToComplete:
    event_manager_->OnDecodeCompleted(filename_);
    break;
  default:
    assert(false);
  }
}

void ImageLoadJob::ReadFile() {
  FileReader file(filename_);

  bool bLoadOK = false;

  if (file.IsOpened() == false) {
    assert(false);
    bLoadOK = false;
  } else {
    static const size_t kReadBufferSize = 1024*10;
    BYTE readBuffer[kReadBufferSize];
    BOOL bReadOK = TRUE;
    ElapseTime file_io_time;

    while (bReadOK) {
      DWORD dwReadBytes = 0;
      bReadOK = file.Read(readBuffer, kReadBufferSize, &dwReadBytes);
      if (FALSE == bReadOK) {
        assert(false);
        bLoadOK = false;
        break;
      } else {
        if (dwReadBytes <= 0) {///< 파일의 끝까지 읽었다.
          break;
        } else {
          file_read_buffer_.resize(file_read_buffer_.size() + dwReadBytes);
          memcpy((&(file_read_buffer_[0])) + file_read_buffer_.size() - dwReadBytes, readBuffer, dwReadBytes);
        }
      }
    }

    DebugPrintf(TEXT("fileio(%s) time:%d"), only_filename_.c_str(), file_io_time.End());

    static const size_t kMinimumFileSize = 5;
    if (file_read_buffer_.size() < kMinimumFileSize) {
      /// Too small image file
      bLoadOK = false;
    }
    else {
      bLoadOK = true;
    }
  }

  if (bLoadOK == true) {
    state_ = State::kReadyToDecode;
  } else {
    assert(!"Can't load image");
    image_ = error_image_;
    state_ = State::kReadyToComplete;
  }
}

void ImageLoadJob::Decode() {
  assert(image_ != nullptr);

  fipMemoryIO mem(&file_read_buffer_[0], (DWORD)file_read_buffer_.size());

  ElapseTime decode_elapse;
  bool bLoadOK = image_->LoadFromMemory(mem);
  DebugPrintf(TEXT("decode(%s) time:%d"), only_filename_.c_str(), decode_elapse.End());

  if (bLoadOK == true) {
    /// 옵션에 따라 자동 회전을 시킨다.
    if (ZOption::GetInstance().IsUseAutoRotation()) {
      ElapseTime rotate_elapse;
      image_->AutoRotate();
      DebugPrintf(TEXT("rotate(%s) time:%d"), only_filename_.c_str(), rotate_elapse.End());
    }
  }
  else {
    image_ = error_image_;
  }
  state_ = State::kReadyToComplete;
}

