//-----------------------------------------------------------------------------
#ifndef CAMERA_H
#define CAMERA_H
//-----------------------------------------------------------------------------
#include "Vector3f.h"
//-----------------------------------------------------------------------------
namespace GL
{
//-----------------------------------------------------------------------------
class Camera
{
public:
	Camera (const Vector3f& pos, const Vector3f& dir, const Vector3f& up,
		float aspect, float fovy, float near, float far);
	~Camera ();

	void Rotate (float upAngle, float vAngle);

	static void ComputeViewMatrix (float *mat, const Camera& camera);
	static void ComputeProjectionMatrix (float *mat, const Camera& camera);

private:
	Vector3f mPosition;
	Vector3f mUp;
	Vector3f mDir;
	Vector3f mV;

	float mAspect;
	float mFovY;
	float mNear;
	float mFar;

};
//-----------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
#endif /* end of include guard: CAMERA_H */
//-----------------------------------------------------------------------------
