#pragma once

#include <Eigen/Eigen>
#include <glm/glm.hpp>

namespace gfx {
template<glm::length_t C, typename T, glm::qualifier Q>
struct eigenresults
{
    glm::mat<C, C, T, Q> eigenvectors;
    glm::vec<C, T, Q>    eigenvalues;
};

template<glm::length_t C, typename T, glm::qualifier Q>
eigenresults<C, T, Q> eig(const glm::mat<C, C, T, Q>& matrix)
{
    static Eigen::EigenSolver<Eigen::Matrix<T, C, C>> solver;
    Eigen::Matrix3d                                   matrix_eigen = reinterpret_cast<const Eigen::Matrix3d&>(matrix);
    solver.compute(matrix_eigen);

    eigenresults<C, T, Q> results;
    const Eigen::Matrix3d       vectors = solver.eigenvectors().real();
    const Eigen::Vector3d       values  = solver.eigenvalues().real();
    results.eigenvalues           = reinterpret_cast<const glm::dvec3&>(values);
    results.eigenvectors          = reinterpret_cast<const glm::dmat3&>(vectors);
    return results;
}
}    // namespace gfx