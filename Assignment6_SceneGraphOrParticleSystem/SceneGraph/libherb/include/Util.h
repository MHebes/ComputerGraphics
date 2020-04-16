#pragma once

#include <QtGui>

namespace Util {

/**
 * @brief Calculates the tangent and bitangent vectors
 *
 * @param vertices  n elements
 * @param normals   n elements, lined up with vertices
 * @param texcoords n elements, lined up with vertices
 * @param indices   m elements, each triplet of indices point to positions in the above vectors
 * @param out_tangents   (output variable) m / 3 elements - one per face, lined up with every third index
 * @param out_bitangents (output variable) m / 3 elements - one per face, lined up with every third index
 */
void CalculateTangents(const QVector<QVector3D>& vertices,
                       const QVector<QVector3D>& normals,
                       const QVector<QVector2D>& texcoords,
                       const QVector<unsigned int>& indices,
                       QVector<QVector3D>& out_tangents,
                       QVector<QVector3D>& out_bitangents);

}  // namespace Util