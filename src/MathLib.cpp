#include "MathLib.h"

template <>
const Vec2<float> Vec2<float>::Zero(0.0f, 0.0f);
template <>
const Vec2<int> Vec2<int>::Zero(0, 0);
template <>
const Vec2<bool> Vec2<bool>::Zero(false, false);
template <>
const Vec2<float> Vec2<float>::One(1.0f, 1.0f);
template <>
const Vec2<int> Vec2<int>::One(1, 1);
template <>
const Vec2<float> Vec2<float>::UnitX(1.0f, 0.0f);
template <>
const Vec2<float> Vec2<float>::UnitY(0.0f, 1.0f);
template <>
const Vec2<float> Vec2<float>::NegUnitX(-1.0f, 0.0f);
template <>
const Vec2<float> Vec2<float>::NegUnitY(0.0f, -1.0f);

template <>
const Vec3<float> Vec3<float>::Zero(0.0f, 0.0f, 0.0f);
template <>
const Vec3<int> Vec3<int>::Zero(0, 0, 0);
template <>
const Vec3<float> Vec3<float>::One(1.0f, 1.0f, 1.0f);
template <>
const Vec3<int> Vec3<int>::One(1, 1, 1);
template <>
const Vec3<float> Vec3<float>::UnitX(1.0f, 0.0f, 0.0f);
template <>
const Vec3<float> Vec3<float>::UnitY(0.0f, 1.0f, 0.0f);
template <>
const Vec3<float> Vec3<float>::UnitZ(0.0f, 0.0f, 1.0f);
template <>
const Vec3<float> Vec3<float>::NegUnitX(-1.0f, 0.0f, 0.0f);
template <>
const Vec3<float> Vec3<float>::NegUnitY(0.0f, -1.0f, 0.0f);
template <>
const Vec3<float> Vec3<float>::NegUnitZ(0.0f, 0.0f, -1.0f);
template <>
const Vec3<float> Vec3<float>::Infinity(junebug::Infinity, junebug::Infinity, junebug::Infinity);
template <>
const Vec3<float> Vec3<float>::NegInfinity(junebug::NegInfinity, junebug::NegInfinity, junebug::NegInfinity);

static float m3Ident[3][3] =
	{
		{1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 1.0f}};
const Matrix3 Matrix3::Identity(m3Ident);

static float m4Ident[4][4] =
	{
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}};

const Matrix4 Matrix4::Identity(m4Ident);

const Quaternion Quaternion::Identity(0.0f, 0.0f, 0.0f, 1.0f);

template <typename T>
Vec2<T> Vec2<T>::Transform(const Vec2<T> &vec, const Matrix3 &mat, float w /*= 1.0f*/)
{
	Vec2<T> retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] + w * mat.mat[2][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] + w * mat.mat[2][1];
	// ignore w since we aren't returning a new value for it...
	return retVal;
}

template <typename T>
Vec3<T> Vec3<T>::Transform(const Vec3<T> &vec, const Matrix4 &mat, float w /*= 1.0f*/)
{
	Vec3<T> retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
			   vec.z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
			   vec.z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
			   vec.z * mat.mat[2][2] + w * mat.mat[3][2];
	// ignore w since we aren't returning a new value for it...
	return retVal;
}

// This will transform the vector and renormalize the w component
template <typename T>
Vec3<T> Vec3<T>::TransformWithPerspDiv(const Vec3<T> &vec, const Matrix4 &mat, float w /*= 1.0f*/)
{
	Vec3<T> retVal;
	retVal.x = vec.x * mat.mat[0][0] + vec.y * mat.mat[1][0] +
			   vec.z * mat.mat[2][0] + w * mat.mat[3][0];
	retVal.y = vec.x * mat.mat[0][1] + vec.y * mat.mat[1][1] +
			   vec.z * mat.mat[2][1] + w * mat.mat[3][1];
	retVal.z = vec.x * mat.mat[0][2] + vec.y * mat.mat[1][2] +
			   vec.z * mat.mat[2][2] + w * mat.mat[3][2];
	float transformedW = vec.x * mat.mat[0][3] + vec.y * mat.mat[1][3] +
						 vec.z * mat.mat[2][3] + w * mat.mat[3][3];
	if (!NearZero(Abs(transformedW)))
	{
		transformedW = 1.0f / transformedW;
		retVal *= transformedW;
	}
	return retVal;
}

// Transform a Vec3 by a quaternion
template <typename T>
Vec3<T> Vec3<T>::Transform(const Vec3<T> &v, const Quaternion &q)
{
	// v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
	Vec3<T> qv(q.x, q.y, q.z);
	Vec3 retVal = v;
	retVal += 2.0f * Vec3::Cross(qv, Vec3::Cross(qv, v) + q.w * v);
	return retVal;
}

void Matrix4::Invert()
{
	// Thanks slow math
	float tmp[12]; /* temp array for pairs */
	float src[16]; /* array of transpose source matrix */
	float dst[16]; /* storage */
	float det;	   /* determinant */
	/* transpose matrix */

	// row 1 to col 1
	src[0] = mat[0][0];
	src[4] = mat[0][1];
	src[8] = mat[0][2];
	src[12] = mat[0][3];

	// row 2 to col 2
	src[1] = mat[1][0];
	src[5] = mat[1][1];
	src[9] = mat[1][2];
	src[13] = mat[1][3];

	// row 3 to col 3
	src[2] = mat[2][0];
	src[6] = mat[2][1];
	src[10] = mat[2][2];
	src[14] = mat[2][3];

	// row 4 to col 4
	src[3] = mat[3][0];
	src[7] = mat[3][1];
	src[11] = mat[3][2];
	src[15] = mat[3][3];

	// 	for (int i = 0; i < 4; i++) {
	// 		src[i] = mat[i*4];
	// 		src[i + 4] = mat[i*4 + 1];
	// 		src[i + 8] = mat[i*4 + 2];
	// 		src[i + 12] = mat[i*4 + 3];
	// 	}
	/* calculate pairs for first 8 elements (cofactors) */
	tmp[0] = src[10] * src[15];
	tmp[1] = src[11] * src[14];
	tmp[2] = src[9] * src[15];
	tmp[3] = src[11] * src[13];
	tmp[4] = src[9] * src[14];
	tmp[5] = src[10] * src[13];
	tmp[6] = src[8] * src[15];
	tmp[7] = src[11] * src[12];
	tmp[8] = src[8] * src[14];
	tmp[9] = src[10] * src[12];
	tmp[10] = src[8] * src[13];
	tmp[11] = src[9] * src[12];
	/* calculate first 8 elements (cofactors) */
	dst[0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
	dst[0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
	dst[1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
	dst[1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
	dst[2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
	dst[2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
	dst[3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
	dst[3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
	dst[4] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
	dst[4] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
	dst[5] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
	dst[5] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
	dst[6] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
	dst[6] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
	dst[7] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
	dst[7] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = src[2] * src[7];
	tmp[1] = src[3] * src[6];
	tmp[2] = src[1] * src[7];
	tmp[3] = src[3] * src[5];
	tmp[4] = src[1] * src[6];
	tmp[5] = src[2] * src[5];
	tmp[6] = src[0] * src[7];
	tmp[7] = src[3] * src[4];
	tmp[8] = src[0] * src[6];
	tmp[9] = src[2] * src[4];
	tmp[10] = src[0] * src[5];
	tmp[11] = src[1] * src[4];
	/* calculate second 8 elements (cofactors) */
	dst[8] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
	dst[8] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
	dst[9] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
	dst[9] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
	dst[10] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
	dst[10] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
	dst[11] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
	dst[11] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
	dst[12] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
	dst[12] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
	dst[13] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
	dst[13] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
	dst[14] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
	dst[14] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
	dst[15] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
	dst[15] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
	/* calculate determinant */
	det = src[0] * dst[0] + src[1] * dst[1] + src[2] * dst[2] + src[3] * dst[3];
	/* calculate matrix inverse */
	det = 1 / det;
	for (int j = 0; j < 16; j++)
		dst[j] *= det;

	// Set it back
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mat[i][j] = dst[i * 4 + j];
		}
	}
}

Matrix4 Matrix4::CreateFromQuaternion(const class Quaternion &q)
{
	float mat[4][4];

	mat[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
	mat[0][1] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
	mat[0][2] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
	mat[0][3] = 0.0f;

	mat[1][0] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
	mat[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	mat[1][2] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
	mat[1][3] = 0.0f;

	mat[2][0] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
	mat[2][1] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
	mat[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
	mat[2][3] = 0.0f;

	mat[3][0] = 0.0f;
	mat[3][1] = 0.0f;
	mat[3][2] = 0.0f;
	mat[3][3] = 1.0f;

	return Matrix4(mat);
}
