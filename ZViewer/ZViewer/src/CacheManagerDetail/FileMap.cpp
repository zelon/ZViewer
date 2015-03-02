#include "stdafx.h"
#include "FileMap.h"

void FileMap::Clear() {
  index_to_filename_.clear();
  filename_to_index_.clear();
}

size_t FileMap::size() const {
  assert(index_to_filename_.size() == filename_to_index_.size());
  return index_to_filename_.size();
}

void FileMap::Set(const std::vector<FileData>& filedata_list) {
  Clear();

  for ( size_t i=0; i<filedata_list.size(); ++i) {
    index_to_filename_.insert(std::make_pair((int)i, filedata_list[i].m_strFileName));
    filename_to_index_.insert(std::make_pair(filedata_list[i].m_strFileName, (int)i));
  }

  assert(index_to_filename_.size() == filename_to_index_.size());
  assert(index_to_filename_.size() == filedata_list.size());
}

tstring FileMap::FindFilenameByIndex(const int index) const {
  auto it = index_to_filename_.find(index);
  if (it == index_to_filename_.end()) {
    return null_string_;
  }
  return it->second;
}

int FileMap::FindIndexByFilename(const tstring& filename) const {
  auto it = filename_to_index_.find(filename);
  if (it == filename_to_index_.end()) {
    return -1;
  }
  return it->second;
}

