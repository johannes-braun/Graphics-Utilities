#include "MatrixIO.h"
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Cholesky>
#include <gfx.file/file.hpp>

using namespace Eigen;

double errFunc(const Vector2d& pt, const Matrix<double, 2, 1>& x, Matrix<double, 1, 2>* jac)
{
	// TODO
	const auto diff = pt - x;
	const auto squared_length = diff.dot(diff);
	*jac = diff.transpose();
	return squared_length;
}

void aufgabe1()
{
	// TODO
	Matrix<double, Dynamic, 3> data1;
	readMatrix(gfx::file("amr/a1data1.txt").path.string(), data1);

	Matrix<double, Dynamic, 3> data2;
	readMatrix(gfx::file("amr/a1data2.txt").path.string(), data2);

	const Matrix<double, Dynamic, 2> data1_positions = data1.block(0, 0, data1.rows(), 2);
	const Matrix<double, Dynamic, 2> data2_positions = data2.block(0, 0, data2.rows(), 2);
	const Matrix<double, Dynamic, 1> data1_deviation = data1.block(0, 2, data1.rows(), 1);
	const Matrix<double, Dynamic, 1> data2_deviation = data2.block(0, 2, data2.rows(), 1);

	for (auto i = 0; i < data1.rows(); ++i)
	{

	}
}

//-----------------------------------------------------------------------------

Vector2d errFunc2(const Vector2d& pt1, const Vector2d& pt2,
	const Vector3d& x, Matrix<double, 2, 3>* jac)
{
	// TODO
	return Vector2d::Zero();
}

void aufgabe2()
{
	// TODO
}

//-----------------------------------------------------------------------------

int main()
{
	std::cout << "Ergebnisse Aufgabe 1:" << std::endl;
	aufgabe1();
	std::cout << "Ergebnisse Aufgabe 2:" << std::endl;
	aufgabe2();

	return 0;
}
