#include "ObjLoader.h"

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

ObjLoader::ObjLoader() : normals(), vertices(), faces() {}

std::ostream& operator<<(std::ostream& os, const vector3& vec)
{
  os << vec.x << " " << vec.y << " " << vec.z;

  return os;
}

std::ostream& operator<<(std::ostream& os, const face& f)
{
  bool begin = true;
  for (auto pair : f) {
    if (!begin) os << ' ';
    begin = false;
    os << pair.first << "//" << pair.second;
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, const ObjLoader& loader)
{
  for (const vector3& vec : loader.vertices) {
    os << "v " << vec << std::endl;
  }
  for (const vector3& n : loader.normals) {
    os << "vn " << n << std::endl;
  }
  for (const face& f : loader.faces) {
    os << "f " << f << std::endl;
  }

  return os;
}

int ObjLoader::parse_file(const std::string filename)
{
  vertices.clear();
  normals.clear();
  faces.clear();
  idx.clear();

  std::ifstream infile(filename);

  if (!infile.is_open()) {
    char* name = get_current_dir_name();
    std::cout << "CWD: " << name << std::endl;
    free(name);
    return EXIT_FAILURE;
  }

  std::string line;
  std::istringstream ss;

  std::string type;  // should be v, vn, f, etc.

  while (std::getline(infile, line)) {
    ss.str(line);

    ss >> type;

    if (type == "v") {  // vertex
      float x;
      float y;
      float z;

      ss >> x >> y >> z;

      vector3 vec{x, y, z};

      vertices.push_back(vec);
    }
    else if (type == "vn") {  // normal
      float x;
      float y;
      float z;

      ss >> x >> y >> z;

      vector3 vec{x, y, z};

      normals.push_back(vec);
    }
    else if (type == "f") {  // face
      face f;

      size_t first;
      size_t second;

      while (ss >> first) {
        if (ss.peek() == '/')
          ss.ignore();
        else
          break;
        if (ss.peek() == '/')
          ss.ignore();
        else
          break;

        ss >> second;

        // stores face as indices into vertices and normals vectors
        f.emplace_back(first - 1, second - 1);
      }

      ss.clear();

      if (f.size() >= 3) {
        faces.push_back(f);
        for (int i = 0; i < 3; i++) {  // TODO support more than 3 vertices
          idx.push_back(f.at(i).first);
        }
      }
    }
  }

  return EXIT_SUCCESS;
}
