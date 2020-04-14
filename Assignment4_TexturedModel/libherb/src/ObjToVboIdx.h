/**
 * @brief Functions to combine multiple indices into one index buffer
 *
 * Modified from
 * https://github.com/huamulan/OpenGL-tutorial/blob/master/common/vboindexer.cpp
 *
 */

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <cmath>
#include <vector>

// face is 9 indices: v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
typedef struct triface {
  bool present[6];  //    vt1,vn1,    vt2,vn2,    vt3,vn3
  int idx[9];       // v1,vt1,vn1, v2,vt2,vn2, v3,vt3,vn3
} triface;

// floating point cmp
bool is_near(float v1, float v2) { return fabs(v1 - v2) < 0.01f; }

struct PackedVertex {
  QVector3D position;
  QVector2D uv;
  QVector3D normal;

  // To allow this to be used in a map
  bool operator<(const PackedVertex that) const
  {
    // compare the bytes
    return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
  }
};

// Tries to find the same values in a map
bool getSimilarVertexIndex(PackedVertex& packed,
                           std::map<PackedVertex, unsigned int>& vertexToOutIdx,
                           unsigned int& result)
{
  auto it = vertexToOutIdx.find(packed);
  if (it == vertexToOutIdx.end()) {
    return false;
  }
  else {
    result = it->second;
    return true;
  }
}

/**
 * @brief Convert the file format from the obj to one gl can use
 *
 * @param in_verts list of vertices
 * @param in_uvs list of uvs
 * @param in_normals list of normals
 * @param in_faces list of faces (which index into the vertices, uvs, and
 * normals independently)
 * @param out_indices return parameter for new indices list
 * @param out_vertices return parameter for new vertices list
 * @param out_uvs return parameter for new uvs list
 * @param out_normals return parameter for new normals list
 */
void obj_to_vbo(QVector<QVector3D>& in_verts,
                QVector<QVector2D>& in_uvs,
                QVector<QVector3D>& in_normals,
                QVector<triface>& in_faces,
                QVector<unsigned int>& out_indices,
                QVector<QVector3D>& out_vertices,
                QVector<QVector2D>& out_uvs,
                QVector<QVector3D>& out_normals)
{
  std::map<PackedVertex, unsigned int> vertexToOutIdx;

  QVector2D zero2d;
  QVector3D zero3d;

  // For each face
  for (const triface& f : in_faces) {
    // For each vertex
    for (int i = 0; i < 3; i++) {
      // Extract data from the face
      unsigned int vertidx = f.idx[3 * i];
      bool has_uv = f.present[2 * i];
      unsigned int uvidx = f.idx[3 * i + 1];
      bool has_norm = f.present[2 * i + 1];
      unsigned int normidx = f.idx[3 * i + 2];

      QVector3D vert = in_verts[vertidx];
      QVector2D uv = has_uv ? in_uvs[uvidx] : zero2d;
      QVector3D norm = has_norm ? in_normals[normidx] : zero3d;

      // pack the data together so we can use it as a key in our map
      PackedVertex packed = {vert, uv, norm};

      // Try to find the same one already in the out map
      unsigned int index;
      bool found = getSimilarVertexIndex(packed, vertexToOutIdx, index);

      if (found) {
        // already have these same attributes in the vbo
        out_indices.push_back(index);
      }
      else {
        // don't have this exact set of attributes, so add all of them
        out_vertices.push_back(vert);
        out_uvs.push_back(uv);
        out_normals.push_back(norm);

        // and point to the new attributes with a new index
        unsigned int newindex = (unsigned int)out_vertices.size() - 1;
        out_indices.push_back(newindex);
        vertexToOutIdx[packed] = newindex;
      }
    }
  }
}