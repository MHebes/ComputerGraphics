#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

typedef struct vector3 {
  float x;
  float y;
  float z;
} vector3;

typedef std::vector<std::pair<unsigned int, unsigned int>> face;

class ObjLoader {
public:
  ObjLoader();

  /**
   * Loads a .obj file and throws out the old one.
   *
   * @param filename  the filename
   * @return  EXIT_SUCCESS on success
   */
  int parse_file(const std::string filename);

  /**
   * Gets the list of vertex indices
   *
   * @return    a vector of vertex indices
   */

  std::vector<vector3> normals;
  std::vector<vector3> vertices;
  std::vector<face> faces;
  std::vector<unsigned int> idx;
};

std::ostream& operator<<(std::ostream& os, const ObjLoader& loader);
std::ostream& operator<<(std::ostream& os, const vector3& vec);
std::ostream& operator<<(std::ostream& os, const face& f);
