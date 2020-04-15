#include "BasicWidget.h"

#include "PointLight.h"
#include "UnitQuad.h"

#define BRICK_TEXTURE "../brick.ppm"
#define GRASS_TEXTURE "../grass.ppm"

const float LOOK_SPEED = 0.5f;
const float ZOOM_SPEED = 0.05f;

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent) : QOpenGLWidget(parent), logger_(this)
{
  setFocusPolicy(Qt::StrongFocus);
  camera_.setPosition(QVector3D(0.5, 0.5, -2.0));
  camera_.setLookAt(QVector3D(0.5, 0.5, 0.0));
  world_.setToIdentity();
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
  else if (keyEvent->key() == Qt::Key_R) {
    camera_.setPosition(QVector3D(0.5, 0.5, -2.0));
    camera_.setLookAt(QVector3D(0.5, 0.5, 0.0));
    update();
  }
  else {
    qDebug() << "You Pressed an unsupported Key!";
  }
}

void BasicWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
  if (mouseEvent->button() == Qt::LeftButton) {
    mouseAction_ = Rotate;
  }
  else if (mouseEvent->button() == Qt::RightButton) {
    mouseAction_ = Zoom;
  }
  lastMouseLoc_ = mouseEvent->pos();
}

void BasicWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
  if (mouseAction_ == NoAction) {
    return;
  }
  QPoint delta = mouseEvent->pos() - lastMouseLoc_;
  lastMouseLoc_ = mouseEvent->pos();
  if (mouseAction_ == Rotate) {
    QMatrix4x4 rot;
    rot.rotate(LOOK_SPEED * -delta.x(), 0, 1.0f, 0);  // yaw
    rot.rotate(LOOK_SPEED * delta.y(), 1, 0, 0);      // pitch
    camera_.setGazeVector(rot * camera_.gazeVector());
  }
  else if (mouseAction_ == Zoom) {
    QVector3D gaze = camera_.gazeVector();
    camera_.translateCamera(gaze * ZOOM_SPEED * -delta.y());
  }
  update();
}

void BasicWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
  mouseAction_ = NoAction;
}

void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();

  qDebug() << QDir::currentPath();
  // TODO:  You may have to change these paths.
  QString brickTex = BRICK_TEXTURE;
  QString grassTex = GRASS_TEXTURE;

  UnitQuad* backWall = new UnitQuad();
  backWall->init(brickTex);
  QMatrix4x4 backXform;
  backXform.setToIdentity();
  backXform.scale(1.0, 1.0, -1.0);
  backWall->setModelMatrix(backXform);
  renderables_.push_back(backWall);

  UnitQuad* rightWall = new UnitQuad();
  rightWall->init(brickTex);
  QMatrix4x4 rightXform;
  rightXform.setToIdentity();
  rightXform.rotate(90.0, 0.0, 1.0, 0.0);
  rightWall->setModelMatrix(rightXform);
  renderables_.push_back(rightWall);

  UnitQuad* leftWall = new UnitQuad();
  leftWall->init(brickTex);
  QMatrix4x4 leftXform;
  leftXform.setToIdentity();
  leftXform.translate(1.0, 0.0, -1.0);
  leftXform.rotate(-90.0, 0.0, 1.0, 0.0);
  leftWall->setModelMatrix(leftXform);
  renderables_.push_back(leftWall);

  UnitQuad* floor = new UnitQuad();
  floor->init(grassTex);
  QMatrix4x4 floorXform;
  floorXform.setToIdentity();
  floorXform.translate(-0.5, 0.0, 0.5);
  floorXform.scale(2.0, 2.0, 2.0);
  floorXform.rotate(-90.0, 1.0, 0.0, 0.0);
  floor->setModelMatrix(floorXform);
  renderables_.push_back(floor);

  m_lights[0] = {
    {-2.0f, 2.0f, -2.0f},  // position
    1.0f,                 // ambientIntensity
    3.0f,                 // constant
    0.0f,                 // linear
    0.2f,                 // quadratic
    {1.0f, 1.0f, 1.0f},   // ambient
    {1.0f, 1.0f, 1.0f},   // diffuse
    {1.0f, 1.0f, 1.0f},   // specular
  };

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

  camera_.setPerspective(70.f, (float)w / (float)h, 0.001, 1000.0);
  glViewport(0, 0, w, h);
}

void BasicWidget::paintGL()
{
  qint64 msSinceRestart = frameTimer_.restart();
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  for (auto renderable : renderables_) {
    renderable->update(msSinceRestart);
    // TODO:  Understand that the camera is now governing the view and
    // projection matrices
    renderable->draw(world_, camera_.getViewMatrix(),
                     camera_.getProjectionMatrix(),
                     m_lights);
  }
  update();
}
