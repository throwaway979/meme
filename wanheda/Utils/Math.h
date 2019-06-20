#pragma once
#include "..\SDK\VMatrix.h"

#define PI 3.14159265358979323846f

class Math {
public:

	void VectorAngles( const Vector& vecForward, Vector& vecAngles );

	void VectorAngles( const Vector& forward, Vector& up, Vector& angles );

	void VectorTransform( const Vector in1, const matrix3x4_t in2, Vector& out );

	Vector CalcAngle( const Vector& vecSource, const Vector& vecDestination );

	vec_t VectorNormalize( Vector& v );

	void AngleVectors( const Vector& angles, Vector* forward );

	void NormalizeAngles( Vector& angles );
	Vector NormalizeAngle( Vector angle );

	float NormalizeYaw( float yaw );

	void AngleVectors( const Vector& angles, Vector* forward, Vector* right, Vector* up );

	float YawDistance(float firstangle, float secondangle);

	void RandomSeed( int seed );

	float RandomFloat( float min, float max );

	bool Clamp( Vector& angles );

	void ClampAngles( Vector& angles );
	Vector ClampAngle( Vector angles );

	float GRD_TO_BOG( float GRD );

	float Distance( Vector2D point1, Vector2D point2 );

	void RotateTriangle( std::array<Vector2D, 3>& points, float rotation );

	void AngleMatrix( const Vector& angles, matrix3x4_t& matrix );

	void MatrixSetColumn( const Vector& in, int column, matrix3x4_t& out );

	void AngleMatrix( const Vector& angles, const Vector& position, matrix3x4_t& matrix );

	static constexpr float DEG_2_RAD( float degree )
	{
		return degree * ( PI / 180.f );
	}
};
extern Math g_Math;