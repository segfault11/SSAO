//-----------------------------------------------------------------------------
#ifndef VECTOR_H
#define VECTOR_H
//-----------------------------------------------------------------------------
namespace GL
{
//-----------------------------------------------------------------------------
class Vector3f 
{
public:
	Vector3f ();
    Vector3f (float x, float y, float z);
    ~Vector3f ();
	
	void Normalize ();
	void Rotate (const Vector3f& axis, float angle);	

	float& X() {return mX;};
	float& Y() {return mY;};
	float& Z() {return mZ;};
	
	float X() const {return mX;};
	float Y() const {return mY;};
	float Z() const {return mZ;};
	
	

	void Dump () const;

	static void ComputeCrossProduct (Vector3f& res, const Vector3f& a, 
		const Vector3f& b);
	static void ComputeDotProduct (float& res, const Vector3f& a, 
		const Vector3f& b);
	

private:
    float mX;
    float mY;
    float mZ;
};
//-----------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
#endif /* end of include guard: VECTOR_H */
//-----------------------------------------------------------------------------
