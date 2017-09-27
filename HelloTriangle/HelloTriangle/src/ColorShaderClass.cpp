#include "..\inc\ColorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
    m_pVS           = 0;
    m_pPS           = 0;
    m_pLayout       = 0;
    m_pMatrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

HRESULT ColorShaderClass::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
    HRESULT hr;

    // Inititalize the vertex and pixel shaders
    hr = InitializeShader(pDevice, hwnd, L"shaders\\vs.cso", L"shaders\\ps.cso");
    CHECK_HR_MSG(hr, hwnd, "Unable to initialize one of the shaders");

    return S_OK;
}

void ColorShaderClass::Shutdown()
{
    // Shutdown the vertex and pixel shaders as well as the related objects
    ShutdownShader();
}

HRESULT ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX modelTransformationMatrix)
{
    HRESULT hr;

    // Set the shader parameters that it will use for rendering
    hr = SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix, modelTransformationMatrix);
    CHECK_HR(hr);

    // Now render the prepared buffers with the shader
    RenderShader(pDeviceContext, indexCount);

    return S_OK;
}

HRESULT ColorShaderClass::InitializeShader(ID3D11Device* pDevice, HWND hwnd, LPCWSTR vsFileName, LPCWSTR psFileName)
{
    HRESULT hr;

    ID3D10Blob* pVSBuffer = nullptr;

    hr = D3DReadFileToBlob(vsFileName, &pVSBuffer);
    CHECK_HR_MSG(hr, hwnd, "Unable to locate the vertex shader");

    ID3D10Blob* pPSBuffer = nullptr;

    hr = D3DReadFileToBlob(psFileName, &pPSBuffer);
    CHECK_HR_MSG(hr, hwnd, "Unable to locate the pixel shader");

    // Create the vertex shader from the buffer
    hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), nullptr, &m_pVS);
    CHECK_HR_MSG(hr, hwnd, "Unable to create vertex shader object");

    // Create the pixel shader from the buffer
    hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), nullptr, &m_pPS);
    CHECK_HR_MSG(hr, hwnd, "Unable to create pixel shader object");

    // Create the vertex input layout description
    // This setup needs to match the VertexType structure in the ModelClass and in the shader
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName         = "POSITION";
    polygonLayout[0].SemanticIndex        = 0;
    polygonLayout[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot            = 0;
    polygonLayout[0].AlignedByteOffset    = 0;
    polygonLayout[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName         = "COLOR";
    polygonLayout[1].SemanticIndex        = 0;
    polygonLayout[1].Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot            = 0;
    polygonLayout[1].AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout
    UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout
    hr = pDevice->CreateInputLayout(polygonLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pLayout);
    CHECK_HR_MSG(hr, hwnd, "Unable to create input layout");

    // Release the vertex shader buffer and the pixel shader buffer, they are no longer needed.
    SAFE_RELEASE(pVSBuffer);
    SAFE_RELEASE(pPSBuffer);

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth           = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags           = 0;
    matrixBufferDesc.StructureByteStride = 0;
    
    hr = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_pMatrixBuffer);
    CHECK_HR_MSG(hr, hwnd, "Unable to create matrix buffer");

    return S_OK;
}

void ColorShaderClass::ShutdownShader()
{
    // Release the matrix constant buffer
    SAFE_RELEASE(m_pMatrixBuffer);
    // Release the layout
    SAFE_RELEASE(m_pLayout);
    // Release the pixel shader
    SAFE_RELEASE(m_pPS);
    // Release the vertex shader
    SAFE_RELEASE(m_pVS);
}

HRESULT ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, XMMATRIX modelTransformationMatrix)
{
    HRESULT hr;

    // Transpose the matrices to prepare them for the shader (Dx 11 requirement)
    worldMatrix               = XMMatrixTranspose(worldMatrix);
    viewMatrix                = XMMatrixTranspose(viewMatrix);
    projectionMatrix          = XMMatrixTranspose(projectionMatrix);
    modelTransformationMatrix = XMMatrixTranspose(modelTransformationMatrix);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    // Lock the constant bufferso that it can be written to
    hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    CHECK_HR(hr);

    // Get a pointer to the data in the constant buffer
    MatrixBufferType* pDataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer
    pDataPtr->world               = worldMatrix;
    pDataPtr->view                = viewMatrix;
    pDataPtr->projection          = projectionMatrix;
    pDataPtr->modelTransformation = modelTransformationMatrix;

    // Unlock the constant buffer
    pDeviceContext->Unmap(m_pMatrixBuffer, 0);

    // Set the position fo the constant buffer in the vertex shader
    UINT bufferNum = 0;

    // Finally set the constant buffer in the vertex shader with the updated values
    pDeviceContext->VSSetConstantBuffers(bufferNum, 1, &m_pMatrixBuffer);

    return S_OK;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* pDeviceContext, int indexCount)
{
    // Set the vertex input layout
    pDeviceContext->IASetInputLayout(m_pLayout);

    // Set the vertex and pixel shaders that will be used to render this triangle
    pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
    pDeviceContext->PSSetShader(m_pPS, nullptr, 0);

    // Render the fucking triangle, finally!
    pDeviceContext->DrawIndexed(indexCount, 0, 0);
}
