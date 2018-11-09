#ifndef SLAMGRAPH_H
#define SLAMGRAPH_H

#include <string>
#include <vector>
#include <Eigen/Core>

using namespace Eigen;

struct Constraint
{
	int i;
	int j;
	Vector3d zij;
	Matrix3d Sij_inv;
};

struct SlamGraph
{
	std::vector<Vector3d> poses;
	std::vector<Constraint> constraints;
};

bool readSlamGraph(const std::string& nodeFile, const std::string& edgeFile, SlamGraph& graph, int maxNodes=-1);

#endif
