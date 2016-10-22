#include "TgaLoader.h"
#include<iostream>
#include<array>
#pragma warning(disable:4996)
using std::cout;
using std::endl;
using std::array;
namespace Comfort {
	//TGA�t�@�C���̓ǂݍ���
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

		//�w�b�_�ǂݍ���
		fread(lHeader.data(), 1, lHeader.size(), lStream);

		//���ƍ�����ǂݍ���
		mWidth = lHeader[13] * 256 + lHeader[12];
		mHeight = lHeader[15] * 256 + lHeader[14];

		//�r�b�g�[�x
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

		//1�s�N�Z��������̃o�C�g��
		lBytePerPixel = mBpp / 8;
		//�f�[�^�T�C�Y�̊m��
		mImageSize = mWidth*mHeight*lBytePerPixel;
		//�T�C�Y�m��
		mImageData.resize(mImageSize);
		fread(mImageData.data(), 1, mImageSize, lStream);

		//�@BGR(A)��RGB(A)�ɃR���o�[�g
		for (auto i = 0; i<mImageSize; i += lBytePerPixel)
		{
			lTemp = mImageData[i];
			mImageData[i + 0] = mImageData[i + 2];
			mImageData[i + 2] = lTemp;
		}
		//�@�t�@�C�������
		fclose(lStream);
		return true;
	}

	bool TGAImage::CreateTextureResource2D(ID3D11Device* aDevice,ID3D11ShaderResourceView *& aOutTexture)
	{
		D3D11_TEXTURE2D_DESC lDesc;
		lDesc.Width = mWidth;
		lDesc.Height = mHeight;
		lDesc.MipLevels = 1;
		lDesc.ArraySize = 1;
		
		switch (mFormat)
		{
		case Comfort::TGAFormat::RGB://�t�H�[���X���[
		case Comfort::TGAFormat::RGBA:
			lDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		default:
			break;
		}

		lDesc.SampleDesc.Count = 1;
		lDesc.SampleDesc.Quality = 0;
		lDesc.Usage = D3D11_USAGE_DEFAULT;
		lDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		lDesc.CPUAccessFlags = 0;
		lDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA lInitData;
		lInitData.pSysMem = mImageData.data();
		lInitData.SysMemPitch = mWidth*(mBpp / 8);
		lInitData.SysMemSlicePitch = mWidth*mHeight*(mBpp / 8);
		ID3D11Texture2D* lTempTexture;
		auto lHr = aDevice->CreateTexture2D(&lDesc, &lInitData, &lTempTexture);
		if (FAILED(lHr)) {
			if (lTempTexture) {
				lTempTexture->Release();
			}
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC lSRVDesc{};
		lSRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		lSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		lSRVDesc.Texture2D.MipLevels = 1;
		lHr = aDevice->CreateShaderResourceView(lTempTexture, &lSRVDesc, &aOutTexture);
		if (FAILED(lHr)) {
			if (lTempTexture) {
				lTempTexture->Release();
			}
			return false;
		}
		if (lTempTexture) {
			lTempTexture->Release();
		}
		return true;
	}

}

