add_library(herb)

target_include_directories(herb PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(DEFAULT_TEXTURE "${CMAKE_CURRENT_SOURCE_DIR}/data/grid.ppm")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/MtlLoader.cpp" "${CMAKE_CURRENT_BINARY_DIR}/src/MtlLoader.cpp")

set(VERT_SHADER "${CMAKE_CURRENT_SOURCE_DIR}/shaders/vert.glsl")
set(FRAG_SHADER "${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.glsl")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/Renderable.cpp" "${CMAKE_CURRENT_BINARY_DIR}/src/Renderable.cpp")

set(DEFAULT_NORMAL_MAP "${CMAKE_CURRENT_SOURCE_DIR}/data/norm.ppm")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/TexturedQuad.cpp" "${CMAKE_CURRENT_BINARY_DIR}/src/TexturedQuad.cpp")

target_sources(herb PUBLIC
  "${CMAKE_CURRENT_BINARY_DIR}/src/Renderable.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/src/TexturedQuad.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Light.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Camera.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ObjLoader.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ObjMesh.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Util.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Emitter.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Particle.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/src/MtlLoader.cpp")

## Linking to Qt5

target_link_libraries(herb Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL OpenGL::GL)

target_include_directories(herb PUBLIC
  ${QtWidget_INCLUDES}
  ${QtCore_INCLUDES}
  ${QtGui_INCLUDES}
  ${QtOpenGL_INCLUDES}
)