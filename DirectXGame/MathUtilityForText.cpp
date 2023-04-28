#include "MathUtilityForText.h"
#include <cassert>
#include <cmath>

Matrix4x4 MakeIdentityMatrix() {
	static const Matrix4x4 result{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	                              0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	Matrix4x4 result{scale.x, 0.0f, 0.0f,    0.0f, 0.0f, scale.y, 0.0f, 0.0f,
	                 0.0f,    0.0f, scale.z, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeRotateXMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cos,  sin,  0.0f,
	                 0.0f, -sin, cos,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeRotateYMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result{cos, 0.0f, -sin, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	                 sin, 0.0f, cos,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeRotateZMatrix(float theta) {
	float sin = std::sin(theta);
	float cos = std::cos(theta);

	Matrix4x4 result{cos,  sin,  0.0f, 0.0f, -sin, cos,  0.0f, 0.0f,
	                 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

	return result;
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 result{1.0f, 0.0f, 0.0f, 0.0f, 0.0f,        1.0f,        0.0f,        0.0f,
	                 0.0f, 0.0f, 1.0f, 0.0f, translate.x, translate.y, translate.z, 1.0f};

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {

	// スケーリング行列の作成
	Matrix4x4 matScale = MakeScaleMatrix(scale);

	Matrix4x4 matRotX = MakeRotateXMatrix(rot.x);
	Matrix4x4 matRotY = MakeRotateYMatrix(rot.y);
	Matrix4x4 matRotZ = MakeRotateZMatrix(rot.z);
	// 回転行列の合成
	Matrix4x4 matRot = matRotZ * matRotX * matRotY;

	// 平行移動行列の作成
	Matrix4x4 matTrans = MakeTranslateMatrix(translate);

	// スケーリング、回転、平行移動の合成
	Matrix4x4 matTransform = matScale * matRot * matTrans;

	return matTransform;
}

Matrix4x4& operator*=(Matrix4x4& lhm, const Matrix4x4& rhm) {
	Matrix4x4 result{};

	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 0; j < 4; j++) {
			for (size_t k = 0; k < 4; k++) {
				result.m[i][j] += lhm.m[i][k] * rhm.m[k][j];
			}
		}
	}
	lhm = result;
	return lhm;
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = m1;

	return result *= m2;
}
