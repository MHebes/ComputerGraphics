#include "BasicWidget.h"

#include <iostream>

class SpinningCat : public Renderable {
public:
  SpinningCat(float x, float y, float z, float rx, float ry, float rz, float rot_speed) : Renderable()
  {
    qDebug() << QDir::currentPath();
    QString texFile = "../cat3.ppm";
    QVector<QVector3D> pos;
    QVector<QVector3D> norm;
    QVector<QVector2D> texCoord;
    QVector<unsigned int> idx;
    pos << QVector3D(-0.8, -0.8, 0.0);
    pos << QVector3D(0.8, -0.8, 0.0);
    pos << QVector3D(-0.8, 0.8, 0.0);
    pos << QVector3D(0.8, 0.8, 0.0);
    // We don't actually use the normals right now, but this will be useful
    // later!
    norm << QVector3D(0.0, 0.0, 1.0);
    norm << QVector3D(0.0, 0.0, 1.0);
    norm << QVector3D(0.0, 0.0, 1.0);
    norm << QVector3D(0.0, 0.0, 1.0);
    // Add texture coordinates to pass into the initialization of our renderable
    texCoord << QVector2D(0.0, 0.0);
    texCoord << QVector2D(1.0, 0.0);
    texCoord << QVector2D(0.0, 1.0);
    texCoord << QVector2D(1.0, 1.0);

    idx << 0 << 1 << 2 << 2 << 1 << 3;

    init(pos, norm, texCoord, idx, texFile);

    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.translate(x, y, z);
    setModelMatrix(mat);
    setRotationSpeed(rot_speed);
    
    QVector3D axis(rx, ry, rz);
    axis.normalize();
    setRotationAxis(axis);
  }
};

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent), logger_(this)
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

  renderables_.push_back(new SpinningCat(-0.5, 0, 0, 1.0f, 1.0f, 0.0f, 1));
  renderables_.push_back(new SpinningCat(0.5, 0, 0.5, 0.f, 1.0f, 0.f, 1));

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
  glDisable(GL_DEPTH_TEST);
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
