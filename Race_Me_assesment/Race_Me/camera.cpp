#include "camera.h"
#include <math.h>


camera::camera(float x_position, float y_position, float z_position, float camera_rotation)
{
	m_x = x_position;
	m_y = y_position;
	m_z = z_position;
	m_camera_rotation= camera_rotation;

	m_dx = sin(m_camera_rotation *(XM_PI / 180.0));
	m_dz = cos(m_camera_rotation *(XM_PI / 180.0));
}

camera::~camera()
{
}

void camera::Rotate(float degree_nummber)
{
	
	m_camera_rotation += degree_nummber;

	m_dx = sin(m_camera_rotation *(XM_PI / 180.0));
	m_dz = cos(m_camera_rotation *(XM_PI / 180.0));

}

void camera::Forward(float distance)
{
	m_x += m_dx * distance;
	m_z += m_dz * distance;

}

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