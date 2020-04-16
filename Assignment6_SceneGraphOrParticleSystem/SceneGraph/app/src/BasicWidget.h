#pragma once

#include <QtGui>
#include <QtOpenGL>
#include <QtWidgets>

#include "Camera.h"
#include "Light.h"
#include "Renderable.h"
#include "Emitter.h"
#include "Scene.h"

const float LOOK_SPEED = 0.5f;
const float ZOOM_SPEED = 0.05f;

/**
 * This is just a basic OpenGL widget that will allow a change of background
 * color.
 */
class BasicWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

private:
  QElapsedTimer m_frameTimer;

  Camera m_camera;
  QVector<Light*> m_lights;
  QVector<Emitter*> m_emitters;

  std::shared_ptr<Scene> m_scene;

  QOpenGLDebugLogger m_logger;

  std::string m_file;

  // mouse controls
  enum MouseControl { NoAction = 0, Rotate, Zoom };
  QPoint m_lastMouseLoc;
  MouseControl m_mouseAction;

protected:
  // Required interaction overrides
  void keyReleaseEvent(QKeyEvent* keyEvent) override;
  void mousePressEvent(QMouseEvent* mouseEvent) override;
  void mouseMoveEvent(QMouseEvent* mouseEvent) override;
  void mouseReleaseEvent(QMouseEvent* mouseEvent) override;

  // Required overrides form QOpenGLWidget
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

public:
  BasicWidget(std::string objfile, QWidget* parent = nullptr);
  virtual ~BasicWidget();

  // Make sure we have some size that makes sense.
  QSize sizeHint() const { return QSize(800, 600); }
};
