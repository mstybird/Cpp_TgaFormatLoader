#include "TgaLoader.h"
#include<iostream>
#include<array>
#pragma warning(disable:4996)
using std::cout;
using std::endl;
using std::array;
namespace Comfort {
	//TGAファイルの読み込み
	bool TGAImage::ReadTGA(const char * mFileName)
	{
		auto lStream = fopen(mFileName, "rb");
		array<uint8_t, 18>lHeader{};
		uint8_t lBytePerPixel{};
		uint32_t lTemp;

		if (lStream==nullptr) {
			cout << "Not Found \"" << mFileName << "\"" << endl;
			return false;
		}

		//ヘッダ読み込み
		fread(lHeader.data(), 1, lHeader.size(), lStream);

		//幅と高さを読み込み
		mWidth = lHeader[13] * 256 + lHeader[12];
		mHeight = lHeader[15] * 256 + lHeader[14];

		//ビット深度
		mBpp = lHeader[16];

		//24 bit
		if (mBpp == 24)
		{
			mFormat = TGAFormat::RGB;
		}
		//32 bit
		else if (mBpp = 32)
		{
			mFormat = TGAFormat::RGBA;
		}

		//1ピクセルあたりのバイト数
		lBytePerPixel = mBpp / 8;
		//データサイズの確定
		mImageSize = mWidth*mHeight*lBytePerPixel;
		//サイズ確保
		mImageData.resize(mImageSize);
		fread(mImageData.data(), 1, mImageSize, lStream);

		//　BGR(A)をRGB(A)にコンバート
		for (auto i = 0; i<mImageSize; i += lBytePerPixel)
		{
			lTemp = mImageData[i];
			mImageData[i + 0] = mImageData[i + 2];
			mImageData[i + 2] = lTemp;
		}
		//　ファイルを閉じる
		fclose(lStream);
		return true;
	}

}

