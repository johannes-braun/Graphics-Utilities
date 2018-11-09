#ifndef MATRIXIO_H
#define MATRIXIO_H

#include <Eigen/Core>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>

std::vector<std::string> explode(const std::string& text, const std::string& separators);

template<typename T>
T convertToT(const std::string& s)
{
	std::istringstream i(s);
	T x;
	if (!(i >> x))
		throw std::runtime_error("convertToT(\"" + s + "\")");
	return x;
}

template<typename MatType>
bool readMatrix(const std::string& filename, MatType& mat)
{
	std::ifstream f(filename.c_str());
	if (!f.good())
		return false;

	int numCols=-1;
	if (MatType::ColsAtCompileTime!=Eigen::Dynamic)
		numCols=mat.cols();

	int numRows=-1;
	if (MatType::RowsAtCompileTime!=Eigen::Dynamic)
		numRows=mat.rows();

	std::vector<std::vector<double> > tmp;

	int lineNo=1;
	while (f.good())
	{
		std::string line;
		std::getline(f,line);
		std::vector<std::string> parts=explode(line," ");

		if (parts.empty())
		{
			lineNo++;
			continue;
		}

		if ((numCols!=-1)&&(parts.size()!=numCols))
		{
			std::stringstream ss;
			ss << "Number of columns is incorrect: Expected " << numCols << " got " << parts.size() << " in line " << lineNo;
			throw std::runtime_error(ss.str());
		}

		if (numCols==-1)
			numCols=parts.size();


		std::vector<double> col(parts.size());
		for (int i=0;i<parts.size();i++)
			col[i]=convertToT<typename MatType::Scalar>(parts[i]);
		tmp.push_back(col);
		lineNo++;
	}
	if ((numRows!=-1)&&(tmp.size()!=numRows))
	{
		std::stringstream ss;
		ss << "Number of rows is incorrect: Expected " << numRows << " got " << tmp.size();
		throw std::runtime_error(ss.str());
	}
	if (numRows==-1)
		numRows=tmp.size();

	mat.resize(numRows,numCols);

	for (int i=0;i<numRows;i++)
		for (int j=0;j<numCols;j++)
			mat(i,j)=tmp[i][j];

	return true;
}

template<typename MatType>
bool writeMatrix(const std::string& filename, MatType& mat)
{
	std::ofstream f(filename.c_str());
	if (!f.is_open())
		return false;

	f << mat;

	return true;
}

#endif // MATRIXIO_H
