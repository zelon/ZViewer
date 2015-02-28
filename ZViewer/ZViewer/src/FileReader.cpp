#include "stdafx.h"
#include "FileReader.h"

FileReader::FileReader(const tstring& filename) {
  file_handle_ = CreateFile(filename.c_str(),
    GENERIC_READ,
    FILE_SHARE_READ,
    NULL,
    OPEN_EXISTING,
    FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_POSIX_SEMANTICS,
    NULL);
}

FileReader::~FileReader() {
  if (IsOpened()) {
    CloseHandle(file_handle_);
  }
}

bool FileReader::IsOpened() const {
  return file_handle_ != INVALID_HANDLE_VALUE;
}

bool FileReader::Read(void* read_buffer, const DWORD read_buffer_size, DWORD* number_of_bytes_read) {
  return ::ReadFile(file_handle_, read_buffer, read_buffer_size, number_of_bytes_read, nullptr) == TRUE;
}
