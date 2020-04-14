#include "MtlLoader.h"

#include <fstream>
#include <sstream>

#define DEFAULT_TEXTURE "@DEFAULT_TEXTURE@"  // CMAKE: DEFAULT_TEXTURE

MtlLoader::MtlLoader() : m_map_Kd() {}

int MtlLoader::parse_file(const std::string filename)
{
  m_map_Kd.clear();

  const size_t last_slash_idx = filename.find_last_of("\\/");
  m_basedir = filename.substr(0, last_slash_idx + 1);

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

    if (type == "map_Kd") {
      // diffuse color map file
      ss >> m_map_Kd;
    }
  }

  if (m_map_Kd.empty()) {
    m_map_Kd = DEFAULT_TEXTURE;  // use default texture if we don't find one
    m_basedir = "";
  }

  return 0;
}