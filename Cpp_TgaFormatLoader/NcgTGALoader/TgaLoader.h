#pragma once
#include<vector>

namespace Comfort {

	enum class TGAFormat {
		RGB=0,
		RGBA=1
	};

	class TGAImage {
	public:
		TGAImage() {};
		~TGAImage() {}
		bool ReadTGA(const char*mFileName);
	protected:
		int32_t mImageSize;
		std::vector<int8_t> mImageData;
		TGAFormat mFormat;
		int32_t mWidth;
		int32_t mHeight;
		int32_t mBpp;//Bit Per Pixel
	};

}