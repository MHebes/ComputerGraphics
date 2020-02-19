#pragma once

#include <QtGui>
#include <QtOpenGL>
#include <QtWidgets>

#include "ObjLoader.h"

/**
 * This is just a basic OpenGL widget that will allow a change of background
 * color.
 */
class BasicWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

private:
  void createShaders();

  static QString m_vert_shader_src;
  static QString m_frag_shader_src;
  static std::string m_filename;

  ObjLoader m_loader;
  QOpenGLVertexArrayObject m_vao;

  QMatrix4x4 m_model;
  QMatrix4x4 m_view;
  QMatrix4x4 m_projection;

protected:
  // Required interaction overrides
  void keyReleaseEvent(QKeyEvent* keyEvent) override;

  // Required overrides from QOpenGLWidget
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

  QOpenGLBuffer m_vertbuf;
  QOpenGLBuffer m_idxbuf;
  QOpenGLShaderProgram m_program;
  QOpenGLDebugLogger m_logger;

public:
  BasicWidget(QWidget* parent = nullptr);
  virtual ~BasicWidget();

  // Make sure we have some size that makes sense.
  QSize sizeHint() const { return QSize(800, 600); }
};
