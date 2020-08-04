#pragma once

class FileMap final {
public:
	void Clear();
	size_t size() const;

	void Set(const std::vector<FileData>& filedata_list);

	tstring FindFilenameByIndex(const int index) const;
	int FindIndexByFilename(const tstring& filename) const;

private:
	std::map < int/*index*/, tstring/*filename*/ > index_to_filename_;
	std::map< /*filename*/tstring, int/*index*/ > filename_to_index_;

	tstring null_string_;
};
