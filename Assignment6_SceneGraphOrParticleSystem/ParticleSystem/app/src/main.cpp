/**
 * Support code written by Erik W. Anderson
 */

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <cassert>
#include <iostream>

#include "App.h"

static bool enableGLDebug = true;

int main(int argc, char** argv)
{
  // assert(argc >= 2);
  if (argc < 2) {
    std::cerr << "Please supply a texture file" << std::endl;
    exit(1);
  }

  QApplication a(argc, argv);
  QString appDir = a.applicationDirPath();
  QDir::setCurrent(appDir);

  QSurfaceFormat fmt;
  fmt.setDepthBufferSize(24);
  fmt.setStencilBufferSize(8);
  //  fmt.setVersion(3,3);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  if (enableGLDebug) {
    fmt.setOption(QSurfaceFormat::DebugContext);
  }
  QSurfaceFormat::setDefaultFormat(fmt);

  App app(argv[1]);
  app.show();
  return QApplication::exec();
}
