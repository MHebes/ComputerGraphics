#include "BasicWidget.h"

#include <QtCore>
#include <QtGui>
#include <iostream>

#include "MtlLoader.h"
#include "ObjLoader.h"
#include "Sphere.h"

const QVector3D DEFAULT_CAMERA_POS = QVector3D(0, 0.5, 3.0);
const QVector3D DEFAULT_CAMERA_LOOKAT = QVector3D(0, 0, 0.0);
const QVector3D DEFAULT_LIGHT_POS = DEFAULT_CAMERA_POS;

Sphere* sphere;

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(std::string objfile, QWidget* parent)
    : QOpenGLWidget(parent),
      m_file(objfile),
      m_lights(),
      m_scene(),
      m_camera(),
      m_logger(this)
{
  setFocusPolicy(Qt::StrongFocus);

  m_camera.setPosition(DEFAULT_CAMERA_POS);
  m_camera.setLookAt(DEFAULT_CAMERA_LOOKAT);

  m_lights.push_back(new Light);
  m_lights[0]->position = DEFAULT_LIGHT_POS;
  m_lights[0]->ambientIntensity = 0.1;
  m_lights[0]->diffuseIntensity = 0.5;
  m_lights[0]->specularIntensity = 20;
  m_lights[0]->setRange(20);

  sphere = new Sphere;
  m_scene.setRenderable(std::unique_ptr<Sphere>(sphere));
}

BasicWidget::~BasicWidget()
{
  for (auto& e : m_lights)
    if (e) delete e;
  // delete m_mesh
  for (auto& e : m_emitters)
    if (e) delete e;
}

void BasicWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
  // Handle key events here.
  if (keyEvent->key() == Qt::Key_Left) {
    qDebug() << "Left Arrow Pressed";
    update();  // We call update after we handle a key press to trigger a redraw
               // when we are ready
  }
  else if (keyEvent->key() == Qt::Key_Right) {
    qDebug() << "Right Arrow Pressed";
    update();  // We call update after we handle a key press to trigger a redraw
               // when we are ready
  }
  else if (keyEvent->key() == Qt::Key_R) {
    m_camera.setPosition(DEFAULT_CAMERA_POS);
    m_camera.setLookAt(DEFAULT_CAMERA_LOOKAT);
    update();
  }
  else {
    qDebug() << "You Pressed an unsupported Key!";
  }
}

void BasicWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
  if (mouseEvent->button() == Qt::LeftButton) {
    m_mouseAction = MouseControl::Rotate;
  }
  else if (mouseEvent->button() == Qt::RightButton) {
    m_mouseAction = MouseControl::Zoom;
  }
  m_lastMouseLoc = mouseEvent->pos();
}

void BasicWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
  if (m_mouseAction == MouseControl::NoAction) {
    return;
  }
  QPoint delta = mouseEvent->pos() - m_lastMouseLoc;
  m_lastMouseLoc = mouseEvent->pos();
  if (m_mouseAction == Rotate) {
    QMatrix4x4 rot;

    rot.rotate(LOOK_SPEED * -delta.x(), 0, 1.0f, 0);  // yaw
    rot.rotate(LOOK_SPEED * delta.y(), 1, 0, 0);      // pitch

    m_camera.setGazeVector(rot * m_camera.gazeVector());
  }
  else if (m_mouseAction == Zoom) {
    QVector3D gaze = m_camera.gazeVector();
    m_camera.translateCamera(gaze * ZOOM_SPEED * -delta.y());
  }
  update();
}

void BasicWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
  m_mouseAction = MouseControl::NoAction;
}

void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();

  sphere->init(m_file); // TODO hacky

  glViewport(0, 0, width(), height());
  m_frameTimer.start();
}

void BasicWidget::resizeGL(int w, int h)
{
  if (!m_logger.isLogging()) {
    m_logger.initialize();
    // Setup the logger for real-time messaging
    connect(&m_logger, &QOpenGLDebugLogger::messageLogged, [=]() {
      const QList<QOpenGLDebugMessage> messages = m_logger.loggedMessages();
      for (auto msg : messages) {
        qDebug() << msg;
      }
    });
    m_logger.startLogging();
  }
  glViewport(0, 0, w, h);

  m_camera.setPerspective(70.f, (float)w / (float)h, 0.001, 1000.0);
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  qint64 dt = m_frameTimer.elapsed();

  bool shouldUpdate = dt > 1000 / 60;  // 60 fps

  for (auto emitter : m_emitters) {
    if (shouldUpdate) {
      emitter->update(dt);
    }
  }

  if (shouldUpdate) {
    m_scene.update(dt);
  }
  m_scene.draw(m_camera.getViewMatrix(), m_camera.getProjectionMatrix(),
              m_lights);

  if (shouldUpdate) m_frameTimer.start();

  update();
}
