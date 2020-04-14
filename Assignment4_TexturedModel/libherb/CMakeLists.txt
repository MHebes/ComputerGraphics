add_library(herb)

target_include_directories(herb PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(DEFAULT_TEXTURE "${CMAKE_CURRENT_SOURCE_DIR}/data/grid.ppm")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/MtlLoader.cpp" "${CMAKE_CURRENT_BINARY_DIR}/src/MtlLoader.cpp")

target_sources(herb PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/src/Renderable.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ObjLoader.cpp"
  "${CMAKE_CURRENT_BINARY_DIR}/src/MtlLoader.cpp")

## Linking to Qt5

target_link_libraries(herb Qt5::Widgets Qt5::Core Qt5::Gui Qt5::OpenGL OpenGL::GL)

target_include_directories(herb PUBLIC
  ${QtWidget_INCLUDES}
  ${QtCore_INCLUDES}
  ${QtGui_INCLUDES}
  ${QtOpenGL_INCLUDES}
)