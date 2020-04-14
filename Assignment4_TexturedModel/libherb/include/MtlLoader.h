#pragma once

#include <string>

/**
 * @brief Simple class for extracting data from .mtl files.
 *
 */
class MtlLoader {
public:
  MtlLoader();

  /**
   * Loads a .obj file and throws out the old one.
   *
   * @param filename  the filename
   * @return  EXIT_SUCCESS on success
   */
  int parse_file(const std::string filename);
  std::string get_map_Kd() const { return m_map_Kd; }

private:
  std::string m_map_Kd;
};