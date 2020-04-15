#include "ObjLoader.h"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include "ObjToVboIdx.h"

ObjLoader::ObjLoader() {}

/**
 * @brief Helper to parse one face vertex
 *
 * @param present if vt, vn are present
 * @param vert values of v, vt, vn
 * @param str face vertex
 *
 * examples:
 *
 * "5"      -> {0, 0}, {5, x, x}
 * "1/2"    -> {1, 0}, {1, 2, x}
 * "1/2/3"  -> {1, 1}, {1, 2, 3}
 * "1//2"   -> {0, 1}, {1, x, 2}
 */
void parse_face_vert(bool present[2], int vert[3], std::string str)
{
  vert[0] = std::stoi(str);

  const int idx = str.find('/');

  if (idx == std::string::npos) {  // no first slash
    // first vertex only
    present[0] = false;
    present[1] = false;
  }
  else {  // at least one slash
    const int idx2 = str.find('/', idx + 1);

    if (idx2 == std::string::npos) {  // no second slash
      // 1/2 format
      vert[1] = std::stoi(str.substr(idx + 1));
      present[0] = true;
      present[1] = false;
    }
    else {                    // second slash
      if (idx2 == idx + 1) {  // right next to each other
        // 1//2 format
        present[0] = false;
        vert[2] = std::stoi(str.substr(idx2 + 1));
        present[1] = true;
      }
      else {
        // 1/2/3 format
        present[0] = true;
        present[1] = true;

        vert[1] = std::stoi(str.substr(idx + 1));
        vert[2] = std::stoi(str.substr(idx2 + 1));
      }
    }
  }
}

int ObjLoader::parse_file(const std::string filename)
{
  m_normals.clear();
  m_vertices.clear();
  m_uvs.clear();
  m_indices.clear();
  m_mtllib.clear();

  const size_t last_slash_idx = filename.find_last_of("\\/");
  m_basedir = filename.substr(0, last_slash_idx + 1);

  // intermediary vectors read from the obj file itself
  QVector<QVector3D> tmp_normals;
  QVector<QVector3D> tmp_vertices;
  QVector<QVector2D> tmp_uvs;
  QVector<triface> tmp_faces;

  std::ifstream infile(filename);

  if (!infile.is_open()) {
    return EXIT_FAILURE;
  }

  std::string line;
  std::istringstream ss;

  std::string type;  // should be v, vn, f, etc.

  while (std::getline(infile, line)) {
    ss.clear();
    ss.str(line);

    ss >> type;

    if (type == "v") {  // vertex
      float x;
      float y;
      float z;

      ss >> x >> y >> z;

      QVector3D v(x, y, z);
      tmp_vertices << v;
    }
    else if (type == "vt") {  // vertex texture
      float u;
      float v;

      ss >> u >> v;

      QVector2D p(u, v);
      tmp_uvs << p;
    }
    else if (type == "vn") {  // normal
      float x;
      float y;
      float z;

      ss >> x >> y >> z;

      QVector3D v(x, y, z);
      tmp_normals << v;
    }
    else if (type == "f") {  // face
      triface f;

      std::string a;
      std::string b;
      std::string c;

      ss >> a >> b >> c;  // TODO(mike) don't assume three vertices

      parse_face_vert(&(f.present[0]), &(f.idx[0]), a);
      parse_face_vert(&(f.present[2]), &(f.idx[3]), b);
      parse_face_vert(&(f.present[4]), &(f.idx[6]), c);

      tmp_faces << f;
    }
    else if (type == "mtllib") {  // material
      ss >> m_mtllib;
    }
  }

  if (tmp_faces.size() > 0) {
    // use the first face item to detemine if we're using normals and uvs or not
    m_uses_uvs = tmp_faces[0].present[0];
    m_uses_normals = tmp_faces[0].present[1];
  }
  else {
    m_uses_uvs = false;
    m_uses_normals = false;
  }

  // TODO shouldn't really fail here
  assert(m_uses_uvs && m_uses_normals);

  // for (const QVector3D& e : tmp_vertices) {
  //   std::cout << "v " << e.x() << " " << e.y() << " " << e.z() << "\n";
  // }
  // for (const QVector2D& e : tmp_uvs) {
  //   std::cout << "vt " << e.x() << " " << e.y() << "\n";
  // }
  // for (const QVector3D& e : tmp_normals) {
  //   std::cout << "vn " << e.x() << " " << e.y() << " " << e.z() << "\n";
  // }
  // for (const triface& e : tmp_faces) {
  //   std::cout << "f " << e << "\n";
  // }
  // std::cout << std::endl << std::endl;

  // convert the things we got from the obj to usable buffers
  obj_to_vbo(tmp_vertices, tmp_uvs, tmp_normals, tmp_faces, m_indices,
             m_vertices, m_uvs, m_normals);

  // print();

  return EXIT_SUCCESS;
}

void ObjLoader::print()
{
  // print all vertices
  for (size_t i = 0; i < m_vertices.size(); i++) {
    QVector3D v = m_vertices[i];
    QVector2D vt = m_uvs[i];
    QVector3D vn = m_normals[i];
    std::cout << i << ":\t(" << v.x() << ", " << v.y() << ", " << v.z() << "),\t("
              << vt.x() << ", " << vt.y() << "),\t(" << vn.x() << ", " << vn.y() << ", " << vn.z() << ")\n";
  }

  // print face indices
  for (size_t i = 0; i < m_indices.size(); i += 3) {
    std::cout << "{ " << m_indices[i] << ", " << m_indices[i + 1] << ", "
              << m_indices[i + 2] << "}\n";
  }

  std::cout << std::endl;
}