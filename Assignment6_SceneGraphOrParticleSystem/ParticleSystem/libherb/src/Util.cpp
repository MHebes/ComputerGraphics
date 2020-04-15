#include "Util.h"

void Util::CalculateTangents(const QVector<QVector3D>& vertices,
                             const QVector<QVector3D>& normals,
                             const QVector<QVector2D>& texcoords,
                             const QVector<unsigned int>& indices,
                             QVector<QVector3D>& out_tangents,
                             QVector<QVector3D>& out_bitangents)
{
  assert(vertices.size() == normals.size() &&
         vertices.size() == texcoords.size());

  unsigned int numTris = indices.size() / 3;

  // loop through each triangle
  for (unsigned int i = 0; i < indices.size(); i += 3) {
    unsigned int a = indices[i];
    unsigned int b = indices[i + 1];
    unsigned int c = indices[i + 2];

    // get data for this triangle
    const QVector3D& pos1 = vertices[a];
    const QVector3D& pos2 = vertices[b];
    const QVector3D& pos3 = vertices[c];

    const QVector3D& nm1 = normals[a];
    const QVector3D& nm2 = normals[b];
    const QVector3D& nm3 = normals[c];

    const QVector2D& uv1 = texcoords[a];
    const QVector2D& uv2 = texcoords[b];
    const QVector2D& uv3 = texcoords[c];

    // calculate its edges
    QVector3D edge1 = pos2 - pos1;
    QVector3D edge2 = pos3 - pos1;

    // and delta uv coords
    QVector2D duv1 = uv2 - uv1;
    QVector2D duv2 = uv3 - uv1;

    // finally compute with matrix magic the T and B vectors
    // credit to https://learnopengl.com/Advanced-Lighting/Normal-Mapping for
    // this
    float f = 1.0f / (duv1.x() * duv2.y() - duv2.x() * duv1.y());

    QVector3D tangent;
    tangent.setX(f * (duv2.y() * edge1.x() - duv1.y() * edge2.x()));
    tangent.setY(f * (duv2.y() * edge1.y() - duv1.y() * edge2.y()));
    tangent.setZ(f * (duv2.y() * edge1.z() - duv1.y() * edge2.z()));
    tangent.normalize();

    QVector3D bitangent;
    bitangent.setX(f * (-duv2.x() * edge1.x() + duv1.x() * edge2.x()));
    bitangent.setY(f * (-duv2.x() * edge1.y() + duv1.x() * edge2.y()));
    bitangent.setZ(f * (-duv2.x() * edge1.z() + duv1.x() * edge2.z()));
    bitangent.normalize();

    out_tangents.push_back(tangent);
    out_bitangents.push_back(bitangent);
  }
}