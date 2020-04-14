#include "BasicWidget.h"

#include <iostream>

#include "MtlLoader.h"
#include "ObjLoader.h"

class ObjMesh : public Renderable {
public:
  ObjMesh(std::string filename, float x, float y, float z, float rx, float ry,
          float rz, float rot_speed)
      : Renderable(), m_obj()
  {
    qDebug() << QDir::currentPath();

    // TODO(mike) error handling

    // parse obj, get texture file name
    if (m_obj.parse_file(filename) != EXIT_SUCCESS) {
      std::cout << "Could not read file " << filename << std::endl;
      assert(false);
    }
    MtlLoader mtl;
    mtl.parse_file(m_obj.get_mtllib());

    QVector<QVector3D> pos;
    QVector<QVector3D> norm;
    QVector<QVector2D> texCoord;
    QVector<unsigned int> idx;

    // load verts into q-friendly vector
    std::vector<float> verts = m_obj.get_vertices();
    for (size_t i = 0; i < verts.size(); i += 3) {
      pos << QVector3D(verts.at(i), verts.at(i+1), verts.at(i+2));
    }

    // load norms
    std::vector<float> norms = m_obj.get_normals();
    for (size_t i = 0; i < norms.size(); i += 3) {
      pos << QVector3D(norms.at(i), norms.at(i+1), norms.at(i+2));
    }

    // load texcoords
    std::vector<float> texcoords = m_obj.get_texcoords();
    for (size_t i = 0; i < norms.size(); i += 2) {
      pos << QVector2D(norms.at(i), norms.at(i+1));
    }

    // load idxs
    // TODO this only loads the mesh ones rn.. normals will be fucked up
    std::vector<triface> faces = m_obj.get_faces();
    for (size_t i = 0; i < faces.size(); i++) {
      idx << faces.at(i).idx[0] << faces.at(i).idx[3] << faces.at(i).idx[6];
    }

    init(pos, norm, texCoord, idx, QString::fromStdString(mtl.get_map_Kd()));

    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.translate(x, y, z);
    setModelMatrix(mat);
    setRotationSpeed(rot_speed);

    QVector3D axis(rx, ry, rz);
    axis.normalize();
    setRotationAxis(axis);
  }

private:
  ObjLoader m_obj;
};

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

  renderables_.push_back(new ObjMesh(m_objfile, -0.5, 0, 0, 1.0f, 1.0f, 0.0f, 1));

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
