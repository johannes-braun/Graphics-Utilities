#include "MatrixIO.h"
#include "SlamGraph.h"
#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <iostream>
#include <map>

#include <gfx.file/file.hpp>

using namespace Eigen;

//-----------------------------------------------------------------------------

Vector3d poseMinus(const Vector3d& b, const Vector3d& a)
{
    const auto sub         = b.z - a.z;
    const auto sub_z       = atan2(sin(sub), cos(sub));
    const auto sub_xy      = b.segment<2>(0) - a.segment<2>(0);
    const auto cos_theta_a = cos(a.z);
    const auto sin_theta_a = sin(a.z);
    return Vector3d(sub_xy.x * cos_theta_a + sub_xy.y * sin_theta_a, -sub_xy.x * sin_theta_a + sub_xy.y * cos_theta_a, sub_z);
}
Vector3d manifoldSub(const Vector3d& y, const Vector3d& x)
{
    const auto sub   = y.z - x.z;
    const auto sub_z = atan2(sin(sub), cos(sub));
    return Vector3d(y.segment<2>(0) - x.segment<2>(0), sub_z);
}
Vector3d manifoldAdd(const Vector3d& x, const Vector3d& d)
{
    const auto add   = x.z + d.z;
    const auto add_z = atan2(sin(add), cos(add));
    return Vector3d(x.segment<2>(0) + d.segment<2>(0), add_z);
}

Vector3d errFunc(const Vector3d& xi, const Vector3d& xj, const Vector3d& zij, Matrix3d* dxi, Matrix3d* dxj)
{
	const auto eij = manifoldSub(poseMinus(xj, xi), zij);

    // TODO: Implementieren
    return eij;
}

void optimizeGraph(SlamGraph& graph)
{
    VectorXd             rhs(graph.poses.size() * 3);
    SparseMatrix<double> lhs(graph.poses.size() * 3, graph.poses.size() * 3);

    // Hier werden Triplets erzeugt aus Zeile/Spalte/Wert. Damit werden Stellen in der Matrix ausgewaehlt
    // Die !=0 sein koennen. Alle hier nicht vorkommenden Zeilen/Spalten werden implizit als 0 angenommen!
    std::vector<Eigen::Triplet<double>> triplets;
    for (int k = 0; k < graph.constraints.size(); k++)
    {
        Constraint& c = graph.constraints[k];

        // Jedes Constraint verursacht insgesamt 4 3x3 Bloecke, die potentiell !=0 sind
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                triplets.push_back(Eigen::Triplet<double>(c.i * 3 + i, c.i * 3 + j, 1));
                triplets.push_back(Eigen::Triplet<double>(c.j * 3 + i, c.j * 3 + j, 1));
                triplets.push_back(Eigen::Triplet<double>(c.j * 3 + i, c.i * 3 + j, 1));
                triplets.push_back(Eigen::Triplet<double>(c.j * 3 + i, c.j * 3 + j, 1));
            }
        }
    }
    lhs.setFromTriplets(triplets.begin(), triplets.end());
    // Zu diesem Zeitpunkt sind alle 3x3 Bloecke auf der Diagonalen !=0, und alle Bloecke i,j bei denen Pose i mit Pose j eine Verbindung
    // hat Alle anderen Matrixeintraege sind implizit 0!

    VectorXd x(graph.poses.size() * 3);
    // Initialschaetzung in Vektor x uebernehmen
    for (int i = 0; i < graph.poses.size(); i++) x.segment<3>(i * 3) = graph.poses[i];

    Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> slu;

    for (int it = 0; it < 10; it++)
    {
        // Rechte Seite 0 Setzen
        rhs.setZero();

        // Alle Werte der SparseMatrix, die nicht implizit 0 sind, auf 0 setzen.
        for (int k = 0; k < lhs.outerSize(); ++k)
            for (Eigen::SparseMatrix<double>::InnerIterator it(lhs, k); it; ++it) it.valueRef() = 0;

        // Dies sorgt dafuer, dass die erste Pose an den Ursprung constrained wird.
        // Ohne diesen Schritt haette das Gleichungssystem unendlich viele Loesungen
        for (int i = 0; i < 3; i++) lhs.coeffRef(i, i) += 1;

        double totalErr = 0;

        for (int k = 0; k < graph.constraints.size(); k++)
        {
            Constraint& c = graph.constraints[k];

            // Achten Sie im Folgenden darauf, dass c.Sij_inv bereits die INVERSE der Kovarianzmatrix Sigma_ij enthaelt!

            Matrix<double, 3, 3> dxi;
            Matrix<double, 3, 3> dxj;
            Vector3d             err = errFunc(x.segment<3>(c.i * 3), x.segment<3>(c.j * 3), c.zij, &dxi, &dxj);
            // TODO: rhs befuellen.

            Matrix3d ASA;
            Matrix3d ASB;
            Matrix3d BSA;
            Matrix3d BSB;
            // TODO: ASA, ASB, BSA, BSB befuellen
            // Das sind diejenige Matrixbloecke aus J_ij^T * Sigma_ij^-1 * J_ij die !=0 sind!

            // Leider ist es bei Eigen nicht moeglich, eine normale 3x3 Matrix in eine Sparse Matrix einzutragen
            // Daher wird diese Schleife benoetigt, die jeden Eintrag einzeln uebernimmt.
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    // TODO: Die ASA Block in die Matrix lhs schreiben
                    // TODO: Die ASB Block in die Matrix lhs schreiben
                    // TODO: Die BSA Block in die Matrix lhs schreiben
                    // TODO: Die BSB Block in die Matrix lhs schreiben
                    // Um einen Eintrag aus lhs zu beschreiben:
                    // lhs.coeffRef(Zeile,Spalte)+=Wert;
                }
            }

            // Hier wird der gesamt Chi^2 Fehler bestimmt
            totalErr += err.transpose() * c.Sij_inv * err;
        }
        slu.compute(lhs);
        VectorXd h = slu.solve(rhs);

        // TODO: x=x [+] h implementieren


        std::cout << "It: " << it << " Error: " << totalErr << std::endl;

        if (h.norm() < 0.0001)
        {
            std::cout << "Early out!" << std::endl;
            ;
            break;
        }
    }

    for (int i = 0; i < graph.poses.size(); i++) graph.poses[i] = x.segment<3>(i * 3);
}

//-----------------------------------------------------------------------------

void writePoses(const std::string& filename, const SlamGraph& graph)
{
    std::ofstream outp(filename.c_str());
    for (int i = 0; i < graph.poses.size(); i++) outp << graph.poses[i].transpose() << std::endl;
}

//-----------------------------------------------------------------------------

int main()
{
    SlamGraph graph;
    readSlamGraph(gfx::file("killian-v.dat").path.string(), gfx::file("killian-e.dat").path.string(), graph);
    writePoses(gfx::file("before.txt").path.string(), graph);

    optimizeGraph(graph);

    writePoses(gfx::file("after.txt").path.string(), graph);

    return 0;
}
