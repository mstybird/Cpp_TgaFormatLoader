#pragma once
#include<vector>
#include <d3dx11.h>

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
		bool CreateTextureResource2D(ID3D11Device* aDevice, ID3D11ShaderResourceView *& aOutTexture);
	protected:
		int32_t mImageSize;
		std::vector<int8_t> mImageData;
		TGAFormat mFormat;
		int32_t mWidth;
		int32_t mHeight;
		int32_t mBpp;//Bit Per Pixel
	};

}