#include "camera.h"
#include <math.h>


camera::camera(float x_position, float y_position, float z_position, float camera_rotation)
{
	m_x = x_position;
	m_y = y_position;
	m_z = z_position;
	m_camera_rotation= camera_rotation;

	m_xangle = 0.0f;
	m_zangle = 0.0f;
	m_yangle = 0.0f;

	m_dx = sin(m_camera_rotation *(XM_PI / 180.0));
	m_dz = cos(m_camera_rotation *(XM_PI / 180.0));
}

camera::~camera()
{
}

//setting the camera rotation method
void camera::Rotate(float degree_nummber)
{
	//adding the rotation
	m_camera_rotation += degree_nummber;

	m_dx = sin(m_camera_rotation *(XM_PI / 180.0));
	m_dz = cos(m_camera_rotation *(XM_PI / 180.0));

}

//move the camera forward
void camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_z += m_dz * distance;

}

void camera::LookAt_XZ(float x, float z)
{

	//calculate dx and dz between the object and the look at position passed in.
	float DX = x - m_x;
	float DZ = z - m_z;

	//update m_yangle using the arctangent calculation and converting to degrees.
	float f = m_yangle;
	m_yangle = atan2(DX, DZ) * (180.0 / XM_PI);

	m_lookat = XMVectorSet(m_x + DX, m_y, m_z + DZ, 0.0);
	Rotate(m_yangle-f);
}

//void camera::SetCameraAI(bool Stop_camera_ai) { m_StopCameraAi = Stop_camera_ai; }
//bool camera::GeCameraAI() { return m_StopCameraAi; }


void camera::Up(float heigh)
{
	m_y += heigh;
}

XMMATRIX camera::GetViewMatrix()
{
	m_position = XMVectorSet(m_x, m_y, m_z, 0.0);
	m_lookat = XMVectorSet(m_x+ m_dx, m_y,m_z+m_dz, 0.0);
	m_up = XMVectorSet(0.0, 1.0, 0.0, 0.0);

	XMMATRIX view = XMMatrixLookAtLH(m_position, m_lookat, m_up);

	return view;
}


void camera::SetXPos(float x) { m_x += x; }
void camera::SetYPos(float y) { m_y += y; }
void camera::SetZPos(float z) { m_z += z; }


float camera::Getx()

{

	return m_x;

}

float camera::Gety()

{

	return m_y;

}

float camera::Getz()

{

	return m_z;

}