#include "BasicWidget.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      m_vertbuf(QOpenGLBuffer::VertexBuffer),
      m_idxbuf(QOpenGLBuffer::IndexBuffer),
      m_is_wireframe(false),
      m_eye(0.0f, 1.0f, -2.0f),
      m_center(0.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f)
{
  setFocusPolicy(Qt::StrongFocus);

  // initialize matrices
  m_model.setToIdentity();
  m_view.setToIdentity();
  m_projection.setToIdentity();
  m_view.lookAt(m_eye, m_center, m_up);
}

BasicWidget::~BasicWidget()
{
  m_vertbuf.release();
  m_vertbuf.destroy();

  m_idxbuf.release();
  m_idxbuf.destroy();
}

//////////////////////////////////////////////////////////////////////
// Privates

static const std::string OBJ_FILE = "../objects/bunny_centered.obj";
static const std::string OBJ_FILE_ALT = "../objects/monkey_centered.obj";

QString BasicWidget::m_vert_shader_src =
    "#version 330\n"

    "layout(location = 0) in vec3 position;\n"

    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"

    "void main()\n"
    "{\n"
    // move everything according to camera pos/perspective and model position
    "  gl_Position = projection * view * model * vec4(position, 1.0);\n"
    "}\n";

QString BasicWidget::m_frag_shader_src =
    "#version 330\n"
    // just draw everything in red
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "  color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
    "}\n";


void BasicWidget::createShaders()
{
  bool ok;

  ok = m_program.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                         m_vert_shader_src);
  if (!ok) {
    qDebug() << m_program.log();
  }

  ok = m_program.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                         m_frag_shader_src);
  if (!ok) {
    qDebug() << m_program.log();
  }

  ok = m_program.link();

  if (!ok) {
    qDebug() << m_program.log();
  }
}
///////////////////////////////////////////////////////////////////////
// Protected
void BasicWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
  // Handle key events here.
  if (keyEvent->key() == Qt::Key_Q) {
    qDebug() << "Q Pressed";
    QApplication::quit();
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_1) {
    qDebug() << "1 Pressed";
  } else if (keyEvent->key() == Qt::Key_2) {
    qDebug() << "2 Pressed";
  } else if (keyEvent->key() == Qt::Key_Left) {
    qDebug() << "Left Arrow Pressed";
    m_model.rotate(-10.0f, 0.0f, 1.0f, 0.0f);
    m_program.bind();
    m_program.setUniformValue("model", m_model);
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_Right) {
    qDebug() << "Right Arrow Pressed";
    m_model.rotate(10.0f, 0.0f, 1.0f, 0.0f);
    m_program.bind();
    m_program.setUniformValue("model", m_model);
    update();  // We call update after we handle a key press to trigger a redraw when we are ready
  } else if (keyEvent->key() == Qt::Key_W) {
    qDebug() << "W Pressed";
    m_is_wireframe = !m_is_wireframe;
    if (m_is_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    update();
  } else {
    qDebug() << "You Pressed an unsupported Key!";
  }
}

void BasicWidget::initializeGL()
{
  makeCurrent();
  initializeOpenGLFunctions();

  m_logger.initialize();
  // Setup the logger for real-time messaging (from lab 6)
  connect(&m_logger, &QOpenGLDebugLogger::messageLogged, [=]() {
    const QList<QOpenGLDebugMessage> messages = m_logger.loggedMessages();
    for (auto msg : messages) {
      qDebug() << msg;
    }
  });
  m_logger.startLogging();

  QOpenGLContext* curContext = this->context();
  qDebug() << "[BasicWidget]::initializeGL() -- Context Information:";
  qDebug() << "  Context Valid: "
           << std::string(curContext->isValid() ? "true" : "false").c_str();
  qDebug() << "  GL Version Used: " << curContext->format().majorVersion()
           << "." << curContext->format().minorVersion();
  qDebug() << "  Vendor: "
           << reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  qDebug() << "  Renderer: "
           << reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  qDebug() << "  Version: "
           << reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << "  GLSL Version: "
           << reinterpret_cast<const char*>(
                  glGetString(GL_SHADING_LANGUAGE_VERSION));

  // set up shaders
  createShaders();

  // read object file
  if (m_loader.parse_file(OBJ_FILE) != EXIT_SUCCESS) {
    perror("Error opening file: ");
  }

  // set up buffers
  // Define our verts
  static const std::vector<GLfloat> verts = m_loader.get_vertices();

  // Define our indices
  static const std::vector<GLuint> idx = m_loader.get_idx();

  m_program.bind();

  // vertex buffer
  m_vertbuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertbuf.create();
  m_vertbuf.bind();
  m_vertbuf.allocate(verts.data(), verts.size() * sizeof(GL_FLOAT));

  // index buffer
  m_idxbuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_idxbuf.create();
  m_idxbuf.bind();
  m_idxbuf.allocate(idx.data(), idx.size() * sizeof(GL_UNSIGNED_INT));

  // create vao to keep track of things
  m_vao.create();
  m_vao.bind();

  m_vertbuf.bind();
  m_program.enableAttributeArray(0);
  m_program.setAttributeBuffer(0, GL_FLOAT, 0, 3);

  m_idxbuf.bind();

  m_vao.release();
  m_program.release();

  glViewport(0, 0, width(), height());
}

void BasicWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);

  m_projection.perspective(70.0f, (float)w/h, 0.0f, 100.0f);

  m_program.bind();

  m_program.setUniformValue("model", m_model);
  m_program.setUniformValue("view", m_view);
  m_program.setUniformValue("projection", m_projection);
}

void BasicWidget::paintGL()
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO:  render.
  m_program.bind();
  m_vao.bind();

  // TODO use the number of triangles rather than buffer size
  glDrawElements(GL_TRIANGLES, m_idxbuf.size() / sizeof(GLuint), GL_UNSIGNED_INT, 0);

  m_vao.release();
  m_program.release();
}
