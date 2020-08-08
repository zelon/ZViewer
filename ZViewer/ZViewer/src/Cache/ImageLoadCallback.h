#pragma once

class ZImage;

using ImageLoadCallback = std::function<void(const tstring& filename, const std::shared_ptr<ZImage>& image)>;
