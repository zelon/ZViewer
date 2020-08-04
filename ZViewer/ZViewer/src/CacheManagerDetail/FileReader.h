#pragma once

class FileReader final {
public:
	FileReader(const tstring& filename);

	~FileReader();

	bool IsOpened() const;

	bool Read(void* read_buffer, const DWORD read_buffer_size, DWORD* number_of_bytes_read);

private:
	HANDLE file_handle_;
};
