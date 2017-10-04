#include "..\inc\ModelClass.h"

ModelClass::ModelClass()
{
    m_pVertexBuffer      = nullptr;
    m_pIndexBuffer       = nullptr;

    SetRotation(0.0f, 0.0f, 0.0f);
    SetScaling(1.0f, 1.0f, 1.0f);
    SetTranslation(0.0f, 0.0f, 0.0f);

#if USE_TEXTURE
    m_pTexture = nullptr;
#endif
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

#if USE_TEXTURE
HRESULT ModelClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, char* pTextureFilename)
{
    HRESULT hr;

    // Initialize the vertex and index buffers
    hr = InitializeBuffer(pDevice);
    CHECK_HR(hr);

    // Load the texture for this model
    hr = LoadTexture(pDevice, pDeviceContext, pTextureFilename);
    CHECK_HR(hr);

    return S_OK;
}
#else
HRESULT ModelClass::Initialize(ID3D11Device* pDevice)
{
    HRESULT hr;

    // Initialize the vertex and index buffers
    hr = InitializeBuffer(pDevice);
    CHECK_HR(hr);

    return S_OK;
}
#endif
void ModelClass::Shutdown()
{
#if USE_TEXTURE
    // Release the model texture
    ReleaseTexture();
#endif

    // Shutdown the vertex and index buffers
    ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* pDeviceContext)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
    RenderBuffers(pDeviceContext);
}

UINT ModelClass::GetIndexCount()
{
    return m_indexCount;
}

#if USE_TEXTURE
ID3D11ShaderResourceView* ModelClass::GetTexture()
{
    return m_pTexture->GetTexture();
}
#endif

HRESULT ModelClass::InitializeBuffer(ID3D11Device* pDevice)
{
    HRESULT hr;

    // Set the number of vertices in the vertex array
    m_vertexCount = 3;
    
    // Set the number of indices in the index array
    m_indexCount = 3;

    // Create the vertex array
    VertexType* pVertices = new VertexType[m_vertexCount];
    if (!pVertices)
    {
        return E_OUTOFMEMORY;
    }

    // Create the index array
    unsigned long* pIndices = new unsigned long[m_indexCount];
    if (!pIndices)
    {
        return E_OUTOFMEMORY;
    }

    // Load the vertex array with data
    pVertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.f);        // Bottom-left
    pVertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.f);          // Top-middle
    pVertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.f);         // Bottom-right
    
#if USE_TEXTURE
    pVertices[0].texture  = XMFLOAT2(0.000f, 0.876f);           // Bottom-left
    pVertices[1].texture  = XMFLOAT2(0.207f, 0.001f);           // Top-middle
    pVertices[2].texture  = XMFLOAT2(0.799f, 0.876f);           // Bottom-right
#else
    pVertices[0].color    = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);   // Bottom-left
    pVertices[1].color    = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);   // Top-middle
    pVertices[2].color    = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);   // Bottom-right
#endif

    // Load the index array with data
    pIndices[0] = 0; // Bottom-left
    pIndices[1] = 1; // Top-middle
    pIndices[2] = 2; // Bottom-right

    // Set up the description for the static vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth           = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags      = 0;
    vertexBufferDesc.MiscFlags           = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem          = pVertices;
    vertexData.SysMemPitch      = 0;
    vertexData.SysMemSlicePitch = 0;

    // Create the vertex buffer
    hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
    CHECK_HR(hr);

    // Set up the description for the static index buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth           = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags      = 0;
    indexBufferDesc.MiscFlags           = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem                   = pIndices;
    indexData.SysMemPitch               = 0;
    indexData.SysMemSlicePitch          = 0;

    // Create the index buffer
    hr = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
    CHECK_HR(hr);

    // Release the arrays now that the vertex and index buffers have been created and loaded
    delete[] pVertices;
    delete[] pIndices;

    pVertices = nullptr;
    pIndices  = nullptr;

    return S_OK;
}

void ModelClass::ShutdownBuffers()
{
    // Release the index buffer
    SAFE_RELEASE(m_pIndexBuffer);
    // Release the vertex buffer
    SAFE_RELEASE(m_pVertexBuffer);
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* pDeviceContext)
{
    // Set the vertex buffer stride and offset
    UINT stride = sizeof(VertexType);
    UINT offset = 0;

    // Set the vertex buffer to active in the input assembler, so it can be rendered
    pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler, so it can be rendered
    pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer. In this case: triangles
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ModelClass::GetRotationMatrix(XMMATRIX& pRotationMatrix)
{
    pRotationMatrix = m_rotationMatrix;
}

void ModelClass::GetScaleMatrix(XMMATRIX& pScalingMatrix)
{
    pScalingMatrix = m_scalingMatrix;
}

void ModelClass::GetTranslationMatrix(XMMATRIX& pTranslationMatrix)
{
    pTranslationMatrix = m_translationMatrix;
}

void ModelClass::SetRotation(float pitch, float yaw, float roll)
{
    m_rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
}

void ModelClass::SetScaling(float sx, float sy, float sz)
{
    m_scalingMatrix = XMMatrixScaling(sx, sy, sz);
}

void ModelClass::SetTranslation(float x, float y, float z)
{
    m_translationMatrix = XMMatrixTranslation(x, y, z);
}

void ModelClass::Rotate(float deltaPitch, float deltaYaw, float deltaRoll)
{
    m_rotationMatrix = m_rotationMatrix * XMMatrixRotationRollPitchYaw(deltaPitch, deltaYaw, deltaRoll);
}

void ModelClass::Scale(float deltaX, float deltaY, float deltaZ)
{
    m_scalingMatrix = m_scalingMatrix * XMMatrixScaling(deltaX, deltaY, deltaZ);
}

void ModelClass::Translate(float deltaX, float deltaY, float deltaZ)
{
    m_translationMatrix = m_translationMatrix * XMMatrixTranslation(deltaX, deltaY, deltaZ);
}

void ModelClass::GetTransformationMatrix(XMMATRIX& pModelMatrix)
{
    // Multiplications are carried out as S * R * T, because the transformation 
    // matrix is being transposed before being sent to the shader (in ColorShaderClass::SetShaderParameters)
    pModelMatrix = m_scalingMatrix * m_rotationMatrix * m_translationMatrix;
}

#if USE_TEXTURE
HRESULT ModelClass::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, char* pFilename)
{
    HRESULT hr;

    // Create the new texture object
    m_pTexture = new TextureClass;
    if (!m_pTexture)
    {
        return E_OUTOFMEMORY;
    }

    // Initialize the texture object
    hr = m_pTexture->Initialize(pDevice, pDeviceContext, pFilename);
    CHECK_HR(hr);

    return S_OK;
}

void ModelClass::ReleaseTexture()
{
    // Release the texture object
    if (m_pTexture)
    {
        m_pTexture->Shutdown();
        delete m_pTexture;
        m_pTexture = nullptr;
    }
}
#endif
