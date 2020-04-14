/**
 * @brief Functions to combine multiple indices into one index buffer
 *
 * Modified from
 * https://github.com/huamulan/OpenGL-tutorial/blob/master/common/vboindexer.cpp
 *
 */

#include <QVector2D>
#include <QVector3D>
#include <cmath>
#include <vector>

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

// This is useful for converting buffers that may be the same size
void index_vbo(std::vector<QVector3D>& in_verts, std::vector<QVector2D>& in_uvs,
               std::vector<QVector3D>& in_normals,
               std::vector<unsigned int>& out_indices,
               std::vector<QVector3D>& out_vertices,
               std::vector<QVector2D>& out_uvs,
               std::vector<QVector3D>& out_normals)
{
  std::map<PackedVertex, unsigned int> vertexToOutIdx;

  // For each input vertex
  for (unsigned int i = 0; i < in_verts.size(); i++) {
    PackedVertex packed = {in_verts[i], in_uvs[i], in_normals[i]};

    // Try to find the same one already in the out map
    unsigned int index;
    bool found = getSimilarVertexIndex(packed, vertexToOutIdx, index);

    if (found) {
      // already have these same attributes in the vbo
      out_indices.push_back(index);
    }
    else {
      // don't have this exact set of attributes, so add all of them
      out_vertices.push_back(in_verts[i]);
      out_uvs.push_back(in_uvs[i]);
      out_normals.push_back(in_normals[i]);

      // and point to the new attributes with a new index
      unsigned int newindex = (unsigned int)out_vertices.size() - 1;
      out_indices.push_back(newindex);
      vertexToOutIdx[packed] = newindex;
    }
  }
}

void obj_to_vbo(std::vector<QVector3D>& in_verts,
                std::vector<QVector2D>& in_uvs,
                std::vector<QVector3D>& in_normals,
                std::vector<triface>& in_faces,
                std::vector<unsigned int>& out_indices,
                std::vector<QVector3D>& out_vertices,
                std::vector<QVector2D>& out_uvs,
                std::vector<QVector3D>& out_normals)
{
  
}