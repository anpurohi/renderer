#include "..\inc\CameraClass.h"

CameraClass::CameraClass()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CameraClass::CameraClass(const CameraClass&)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
    m_rotation.x = x;
    m_rotation.y = y;
    m_rotation.z = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
    return XMFLOAT3(m_position);
}

XMFLOAT3 CameraClass::GetRotation()
{
    return XMFLOAT3(m_rotation);
}

void CameraClass::Render()
{
    // Setup the vector that points upward
    XMFLOAT3 up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    // Load it into a XMVECTOR structure
    XMVECTOR upVector = XMLoadFloat3(&up);

    // TODO: Check if the copy constructor works properly here
    XMFLOAT3 position = GetPosition();
    
    // Load it into a XMVECTOR structure
    XMVECTOR posVector = XMLoadFloat3(&position);

    // Setup where the camera is looking by default
    XMFLOAT3 lookAt;
    lookAt.x = 0.f;
    lookAt.y = 0.f;
    lookAt.z = 1.f;

    // Load it into a XMVECTOR structure
    XMVECTOR lookAtVector = XMLoadFloat3(&lookAt);

    // Set the yaw (Y-axis), pitch (X-axis) and roll (Z-axis) rotations in radians
    float pitch = m_rotation.x * 0.0174532925f;
    float yaw   = m_rotation.y * 0.0174532925f;
    float roll  = m_rotation.z * 0.0174532925f;

    // Create the rotation matrix from the yaw, pitch and roll values
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // Transform the lookAt and up vector by the rotation matrix, so that the view is correctly rotated at the origin
    lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
    upVector = XMVector3TransformCoord(upVector, rotationMatrix);

    // Translate the rotated camera position to the location of the viewer
    lookAtVector = XMVectorAdd(posVector, lookAtVector);

    // Finally, create the view matrix from the three updated vectors
    m_viewMatrix = XMMatrixLookAtLH(posVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = m_viewMatrix;
}
