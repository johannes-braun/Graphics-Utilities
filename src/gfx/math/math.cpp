#include "math.h"
#include <Eigen/Eigenvalues>

namespace gfx
{
//eigenresults eig(const glm::dmat3& matrix)
//{
//    static Eigen::EigenSolver<Eigen::Matrix3d> solver;
//    Eigen::Matrix3d                            matrix_eigen = reinterpret_cast<const Eigen::Matrix3d&>(matrix);
//    solver.compute(matrix_eigen);
//
//    eigenresults    results;
//    Eigen::Matrix3d vectors = solver.eigenvectors().real();
//    Eigen::Vector3d values  = solver.eigenvalues().real();
//    results.eigenvalues     = reinterpret_cast<const glm::dvec3&>(values);
//    results.eigenvectors    = reinterpret_cast<const glm::dmat3&>(vectors);
//    return results;
//}
}