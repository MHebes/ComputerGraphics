#include "BasicWidget.h"

#include <QtCore>
#include <QtGui>
#include <iostream>

#include "MtlLoader.h"
#include "ObjLoader.h"
#include "ObjMesh.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(std::string objfile, QWidget* parent)
    : m_objfile(objfile), QOpenGLWidget(parent), logger_(this)
{
  setFocusPolicy(Qt::StrongFocus);
}

BasicWidget::~BasicWidget()
{
  for (auto renderable : renderables_) {
    delete renderable;
  }
  renderables_.clear();
}

//////////////////////////////////////////////////////////////////////
// Privates
///////////////////////////////////////////////////////////////////////
// Protected
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
  else {
    qDebug() << "You Pressed an unsupported Key!";
  }
}
void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();

  // FIRST CAT

  renderables_.push_back(new ObjMesh(m_objfile, 0, 0, -2, 0, 1, 0, 0.25));

  glViewport(0, 0, width(), height());
  frameTimer_.start();
}

void BasicWidget::resizeGL(int w, int h)
{
  if (!logger_.isLogging()) {
    logger_.initialize();
    // Setup the logger for real-time messaging
    connect(&logger_, &QOpenGLDebugLogger::messageLogged, [=]() {
      const QList<QOpenGLDebugMessage> messages = logger_.loggedMessages();
      for (auto msg : messages) {
        qDebug() << msg;
      }
    });
    logger_.startLogging();
  }
  glViewport(0, 0, w, h);
  view_.setToIdentity();
  view_.lookAt(QVector3D(0.0f, 0.0f, 2.0f), QVector3D(0.0f, 0.0f, 0.0f),
               QVector3D(0.0f, 1.0f, 0.0f));
  projection_.setToIdentity();
  projection_.perspective(70.f, (float)w / (float)h, 0.001, 1000.0);
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  qint64 dt = frameTimer_.elapsed();

  bool shouldUpdate = dt > 1000 / 60;  // 60 fps

  for (auto renderable : renderables_) {
    if (shouldUpdate) {
      renderable->update(dt);
    }
    renderable->draw(view_, projection_);
  }

  if (shouldUpdate) frameTimer_.start();

  update();
}
