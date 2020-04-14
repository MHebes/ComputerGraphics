#include "ObjLoader.h"
#include "VboIndexer.h"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

ObjLoader::ObjLoader() {}

std::ostream& operator<<(std::ostream& os, const triface& f)
{
  bool begin = true;

  os << f.idx[0];
  if (f.present[0] || f.present[1]) os << '/';
  if (f.present[0]) os << f.idx[1];
  if (f.present[1]) os << '/' << f.idx[2];

  os << ' ' << f.idx[3];
  if (f.present[2] || f.present[3]) os << '/';
  if (f.present[2]) os << f.idx[4];
  if (f.present[3]) os << '/' << f.idx[5];

  os << ' ' << f.idx[6];
  if (f.present[4] || f.present[5]) os << '/';
  if (f.present[4]) os << f.idx[7];
  if (f.present[5]) os << '/' << f.idx[8];

  return os;
}

// TODO(mike) this is broken ish
std::ostream& operator<<(std::ostream& os, const ObjLoader& loader)
{
  const auto vertices = loader.get_vertices();
  const auto normals = loader.get_normals();
  // const auto faces = loader.get_faces();

  for (int i = 0; i < vertices.size(); i ++) {
    os << "v " << vertices.at(i).x << " " << vertices.at(i).y << " " << vertices.at(i).z << std::endl;
  }

  for (int i = 0; i < normals.size(); i ++) {
    os << "vn " << normals.at(i).x << " " << normals.at(i).y << " " << normals.at(i).z << std::endl;
  }
  // for (const triface& f : faces) {
  //   os << "f " << f << std::endl;
  // }

  return os;
}

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

  // intermediary vectors read from the obj file itself
  std::vector<QVector3D> tmp_normals;
  std::vector<QVector3D> tmp_vertices;
  std::vector<QVector2D> tmp_uvs;
  std::vector<triface> tmp_faces;

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

      tmp_vertices.emplace_back(x, y, z);
    }
    else if (type == "vt") { // vertex texture
      float u;
      float v;

      ss >> u >> v;

      tmp_uvs.emplace_back(u, v);
    }
    else if (type == "vn") {  // normal
      float x;
      float y;
      float z;

      ss >> x >> y >> z;

      tmp_normals.emplace_back(x, y, z);
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

      tmp_faces.push_back(f);
    }
    else if (type == "mtllib") { // material
      ss >> m_mtllib;
    }
  }

  if (tmp_faces.size() > 0) {
    // use the first face item to detemine if we're using normals and uvs or not
    m_uses_uvs = tmp_faces[0].present[0];
    m_uses_normals = tmp_faces[0].present[1];
  } else {
    m_uses_uvs = false;
    m_uses_normals = false;
  }

  // TODO shouldn't really fail here
  assert(m_uses_uvs && m_uses_normals);

  // convert the things we got from the obj to usable buffers
  obj_to_vbo(tmp_vertices, tmp_uvs, tmp_normals, tmp_faces, m_indices, m_vertices, m_uvs, m_normals);

  return EXIT_SUCCESS;
}
