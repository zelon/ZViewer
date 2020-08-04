#pragma once

#include "JobInterface.h"

class ZImage;
class EventManager;

class ImageLoadJob final : public JobInterface {
public:
	ImageLoadJob(EventManager* event_manager, std::shared_ptr<ZImage> error_image, const tstring& filename);

	ImageLoadJob(ImageLoadJob const&) = delete;
	ImageLoadJob& operator=(ImageLoadJob const&) = delete;

	void DoJob() override;

	void ReadFile();
	void Decode();

	std::shared_ptr<ZImage> image() { return image_; }
	const tstring& filename() const { return filename_; }

private:
	enum class State {
		kReadyToFileRead,
		kReadyToDecode,
		kReadyToComplete,
	};

	EventManager* event_manager_ = nullptr;
	State state_;
	std::shared_ptr<ZImage> error_image_;
	std::shared_ptr<ZImage> image_;
	const tstring filename_;
	const tstring only_filename_;


	std::vector<BYTE> file_read_buffer_;
	bool need_auto_rotate_ = false;
};

typedef std::shared_ptr<ImageLoadJob> Job;
