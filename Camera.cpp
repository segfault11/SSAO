//-----------------------------------------------------------------------------
#include "Camera.h"
#include <iostream>
#include <cmath>
//-----------------------------------------------------------------------------
GL::Camera::Camera 
(
	const Vector3f& pos, 
	const Vector3f& dir, 
	const Vector3f& up,
	float aspect, 
	float fovy, 
	float near, 
	float far
) 
:
	mPosition(pos),
	mDir(dir),
	mUp(up),
	mAspect(aspect),
	mFovY(fovy),
	mNear(near),
	mFar(far)
{
	mDir.Normalize();
	mUp.Normalize();
	Vector3f::ComputeCrossProduct(mV, mDir, mUp);
	mV.Normalize();
	Vector3f::ComputeCrossProduct(mUp, mV, mDir);
	mUp.Normalize();
}
//-----------------------------------------------------------------------------
GL::Camera::~Camera () 
{

}
//-----------------------------------------------------------------------------
void GL::Camera::Rotate (float upAngle, float vAngle)
{
	mDir.Rotate(mUp, upAngle);
	mV.Rotate(mUp, upAngle);
	mDir.Rotate(mV, vAngle);
	mUp.Rotate(mV, vAngle);
}
//-----------------------------------------------------------------------------
void GL::Camera::ComputeViewMatrix (float *mat, const Camera& camera)
{
	mat[0] = camera.mV.X();
	mat[1] = camera.mUp.X();
	mat[2] = -camera.mDir.X();
	mat[3] = 0.0f; 
	mat[4] = camera.mV.Y();
	mat[5] = camera.mUp.Y();
	mat[6] = -camera.mDir.Y();
	mat[7] = 0.0f; 
	mat[8] = camera.mV.Z();
	mat[9] = camera.mUp.Z();
	mat[10] = -camera.mDir.Z();
	mat[11] = 0.0f; 
	mat[12] = -camera.mV.X()*camera.mPosition.X() -
	 camera.mV.Y()*camera.mPosition.Y() -
	 camera.mV.Z()*camera.mPosition.Z(); 
	mat[13] = -camera.mUp.X()*camera.mPosition.X() -
	 camera.mUp.Y()*camera.mPosition.Y() -
	 camera.mUp.Z()*camera.mPosition.Z(); 
	mat[14] = camera.mDir.X()*camera.mPosition.X() +
	 camera.mDir.Y()*camera.mPosition.Y() +
	 camera.mDir.Z()*camera.mPosition.Z(); 
	mat[15] = 1.0f; 
}
//-----------------------------------------------------------------------------
void GL::Camera::ComputeProjectionMatrix (float *mat, const Camera& camera)
{
	float tt = tanf((camera.mFovY/2.0)*(3.141593f/180.0f));
	float h = camera.mNear*tt;
	float w = h*camera.mAspect;
	float n = camera.mNear;
	float f = camera.mFar;
	float l = -w;
	float r = w;
	float t = h;
	float b = -h;

	mat[0] = 2.0f*n/(r - l);
	mat[1] = 0.0f;
	mat[2] = 0.0f;
	mat[3] = 0.0f;
	
	mat[4] = 0.0f;
	mat[5] = 2.0f*n/(t - b);
	mat[6] = 0.0f;
	mat[7] = 0.0f;

	mat[8] = (r + l)/(r - l);
	mat[9] = (t + b)/(t - b);
	mat[10] = (f + n)/(n - f);
	mat[11] = -1.0f;

	mat[12] = 0.0f;
	mat[13] = 0.0f;
	mat[14] = -2.0f*f*n/(f - n);
	mat[15] = 0.0f;
}
//-----------------------------------------------------------------------------
