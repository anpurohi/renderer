#pragma once

#ifndef _TEXCLASS_H_
#define _TEXCLASS_H_

#include <d3d11.h>
#include <stdio.h>

struct TargaHeader
{
    unsigned char data1[12];
    unsigned short width;
    unsigned short height;
    unsigned char bpp;
    unsigned char data2;
};

class TextureClass
{
public:
    TextureClass();
    TextureClass(const TextureClass&);
    ~TextureClass();

    HRESULT Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
    void Shutdown();
    ID3D11ShaderResourceView* GetTexture();

private:
    HRESULT LoadTarga(char*, int&, int&);

private:
    unsigned char* m_pTargaData;
    ID3D11Texture2D* m_pTexture;
    ID3D11ShaderResourceView* m_pTextureView;
};

#endif