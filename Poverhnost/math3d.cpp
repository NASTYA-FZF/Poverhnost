#define _USE_MATH_DEFINES
#include "math3d.h"
#include <algorithm>
#include <math.h>
#include <iterator>

math3d::mat4::mat4(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33, float dx, float dy, float dz)
{
	_matrix[0][0] = m11; _matrix[0][1] = m12; _matrix[0][2] = m13; _matrix[0][3] = 0.;
	_matrix[1][0] = m21; _matrix[1][1] = m22; _matrix[1][2] = m23; _matrix[1][3] = 0.;
	_matrix[2][0] = m31; _matrix[2][1] = m32; _matrix[2][2] = m33; _matrix[2][3] = 0.;
	_matrix[3][0] = dx; _matrix[3][1] = dy; _matrix[3][2] = dz; _matrix[3][3] = 1.;
}

math3d::mat4& math3d::mat4::operator=(const mat4& right)
{
	// TODO: вставьте здесь оператор return
	std::copy_n(right._matrix[0], 4, _matrix[0]);
	return *this;
}

math3d::mat4 math3d::mat4::operator*(const mat4& right)
{
	mat4 result;
	for (int i = 0; i < _rank; i++)
	{
		for (int j = 0; j < _rank; j++)
		{
			result._matrix[i][j] = 0.;
			for (int k = 0; k < _rank; k++)
			{
				result._matrix[i][j] += _matrix[i][k] * right._matrix[k][j];
			}
		}
	}

	return result;
}

void math3d::mat4::reset()
{
	for (int i = 0; i < _rank; i++)
	{
		memset(_matrix[i], 0., 4 * sizeof(float));
		_matrix[i][i] = 1.;
	}
}

math3d::mat4& math3d::mat4::rotateX(float angle)
{
	// TODO: вставьте здесь оператор return
	mat4 mat_rotateX(1., 0., 0., 0., cos(angle * M_PI / 180), sin(angle * M_PI / 180), 0., -sin(angle * M_PI / 180), cos(angle * M_PI / 180));
	*this = *this * mat_rotateX;
	return *this;
}

math3d::mat4& math3d::mat4::rotateY(float angle)
{
	// TODO: вставьте здесь оператор return
	mat4 mat_rotateY(cos(angle * M_PI / 180), 0., -sin(angle * M_PI / 180), 0., 1., 0., sin(angle * M_PI / 180), 0., cos(angle * M_PI / 180));
	*this = *this * mat_rotateY;
	return *this;
}

math3d::mat4& math3d::mat4::rotateZ(float angle)
{
	// TODO: вставьте здесь оператор return
	mat4 mat_rotateZ(cos(angle * M_PI / 180), sin(angle * M_PI / 180), 0., -sin(angle * M_PI / 180), cos(angle * M_PI / 180), 0., 0., 0., 1.);
	*this = *this * mat_rotateZ;
	return *this;
}

math3d::mat4& math3d::mat4::translate(float x, float y, float z)
{
	// TODO: вставьте здесь оператор return
	mat4 mat_translate;
	mat_translate._matrix[3][0] = x; mat_translate._matrix[3][1] = y; mat_translate._matrix[3][2] = z;
	*this = *this * mat_translate;
	return *this;
}

math3d::mat4& math3d::mat4::scale(float x, float y, float z)
{
	// TODO: вставьте здесь оператор return
	mat4 mat_scale;
	mat_scale._matrix[0][0] = x; mat_scale._matrix[1][1] = y; mat_scale._matrix[2][2] = z;
	*this = *this * mat_scale;
	return *this;
}

math3d::mat4& math3d::mat4::perspective(float center)
{
	// TODO: вставьте здесь оператор return
	mat4 mat;
	mat._matrix[0][3] = -1. / center;
	*this = *this * mat;
	return *this;
}

math3d::vec4::vec4(float x, float y, float z)
{
	_vector[0] = x;
	_vector[1] = y;
	_vector[2] = z;
	_vector[3] = 1.;
}

math3d::vec4& math3d::vec4::operator=(const vec4& right)
{
	// TODO: вставьте здесь оператор return
	std::copy(std::begin(right._vector), std::end(right._vector), std::begin(_vector));
	return *this;
}

math3d::vec4 math3d::vec4::operator*(const mat4& right)
{
	vec4 result;
	result._vector[3] = 0.;

	for (int i = 0; i < _size; i++)
	{
		for (int j = 0; j < _size; j++)
		{
			result._vector[i] += _vector[j] * right._matrix[j][i];
		}
	}

	for (int i = 0; i < _size; i++)
	{
		result._vector[i] /= result._vector[3];
	}

	return result;
}
