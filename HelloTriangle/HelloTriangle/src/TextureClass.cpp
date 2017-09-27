#include "..\inc\TextureClass.h"
#include "../inc/util.h"

TextureClass::TextureClass()
{
    m_pTargaData   = 0;
    m_pTexture     = 0;
    m_pTextureView = 0;
}

TextureClass::TextureClass(const TextureClass&)
{
}

TextureClass::~TextureClass()
{
}

HRESULT TextureClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, char* pFilename)
{
    HRESULT hr;

    int width, height;
    // Load the targa image data into memory
    hr = LoadTarga(pFilename, height, width);
    CHECK_HR(hr);

    // Setup the description of the texture
    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Height             = height;
    textureDesc.Width              = width;
    textureDesc.MipLevels          = 0;
    textureDesc.ArraySize          = 1;
    textureDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count   = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage              = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.CPUAccessFlags     = 0;
    textureDesc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    // Create the empty texture
    hr = pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture);
    CHECK_HR(hr);

    // Set the row pitch of the targa image data
    UINT rowPitch = (width * 4) * sizeof(unsigned char);

    // Copy the targa image data to the texture
    pDeviceContext->UpdateSubresource(m_pTexture, 0, nullptr, m_pTargaData, rowPitch, 0);

    // Setup the shader resource view description
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format                    = textureDesc.Format;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels       = -1;

    // Create the shader resource view for the texture
    hr = pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pTextureView);
    CHECK_HR(hr);

    // Generate mipmaps for this texture
    pDeviceContext->GenerateMips(m_pTextureView);

    // Release the targa image data, now that it has been loaded into the texture
    delete[] m_pTargaData;
    m_pTargaData = nullptr;

    return S_OK;
}

void TextureClass::Shutdown()
{
    // Release the texture view resource
    SAFE_RELEASE(m_pTextureView);
    // Release the texture
    SAFE_RELEASE(m_pTexture);
    // Release the targa data
    if (m_pTargaData)
    {
        delete[] m_pTargaData;
        m_pTargaData = nullptr;
    }
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
    return m_pTextureView;
}

HRESULT TextureClass::LoadTarga(char* pFilename, int& height, int& width)
{
    FILE* pFilePtr;
    // Open the targa file for reading, in binary
    UINT error = fopen_s(&pFilePtr, pFilename, "rb");
    if (error)
    {
        return E_FAIL;
    }

    // Read in the file header
    TargaHeader targaFileHeader;
    UINT count = fread(&targaFileHeader, sizeof(TargaHeader), 1, pFilePtr);
    if (count != 1)
    {
        return E_FAIL;
    }

    int bpp;
    // Get the important information from the header
    height  = targaFileHeader.height;
    width   = targaFileHeader.width;
    bpp     = targaFileHeader.bpp;

    // Check that it is 32-bit (RGBA8)
    if (bpp != 32)
    {
        return E_FAIL;
    }

    // Calculate the size of the 32-bit image data
    int imageSize = width * height * 4;

    // Allocate memory for the targa image data
    unsigned char* targaImage;
    targaImage = new unsigned char[imageSize];
    if (!targaImage)
    {
        return E_OUTOFMEMORY;
    }

    // Read in the targa image data
    count = fread(targaImage, 1, imageSize, pFilePtr);
    if (count != imageSize)
    {
        return E_FAIL;
    }

    // Close the file
    error = fclose(pFilePtr);
    if (error)
    {
        return E_FAIL;
    }

    // Allocate memory for the targa destination data
    m_pTargaData = new unsigned char[imageSize];
    if (!m_pTargaData)
    {
        return E_OUTOFMEMORY;
    }

    // Initialize the index into the targa destination data array
    int index = 0;

    // Initialize the index into the targa image data
    int k = (width * height * 4) - (width * 4);

    // Copy the targa image data into the targa destination
    // array in the correct order since the targa format is stored upside down
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            m_pTargaData[index + 0] = targaImage[k + 2]; // Red
            m_pTargaData[index + 1] = targaImage[k + 1]; // Green
            m_pTargaData[index + 2] = targaImage[k + 0]; // Blue
            m_pTargaData[index + 3] = targaImage[k + 3]; // Alpha

            // Increment the indexes into the targa data
            k       += 4;
            index   += 4;
        }

        // Set the targa image data index back to the preceding row 
        // at the beginning of the column since its reading it in upside down
        k -= (width * 8);
    }

    // Release the targa image data, now that it was copied into the destination array
    delete[] targaImage;
    targaImage = nullptr;

    return S_OK;
}
