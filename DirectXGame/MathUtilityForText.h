#pragma once

///
/// AL3サンプルプログラム用の数学ライブラリ。
/// MT3準拠で、KamataEngine内部の数学ライブラリと重複する。
///

#include "Matrix4x4.h"
#include "Vector3.h"

// アフィン変換行列の作成
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// 代入演算子オーバーロード
Matrix4x4& operator*=(Matrix4x4& lhm, const Matrix4x4& rhm);

// 2項演算子オーバーロード
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);
