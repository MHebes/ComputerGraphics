#include "BasicWidget.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      m_vertbuf(QOpenGLBuffer::VertexBuffer),
      m_idxbuf(QOpenGLBuffer::IndexBuffer)
{
  setFocusPolicy(Qt::StrongFocus);
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

std::string BasicWidget::m_filename = "../objects/cube.obj";

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
  // TODO
  // Handle key events here.
  qDebug() << "You Pressed an unsupported Key!";
  // ENDTODO
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

  // initialize matrices
  m_model.setToIdentity();
  m_view.setToIdentity();
  m_projection.setToIdentity();

  // set up shaders
  createShaders();

  // read object file
  if (m_loader.parse_file(m_filename) != EXIT_SUCCESS) {
    perror("Error opening file: ");
  }

  // set up buffers
  const std::vector<GLfloat> vertices = {
      -0.8f, -0.8f, 0.0f,  // Left vertex position
      0.8f,  -0.8f, 0.0f,  // right vertex position
      -0.8f, 0.8f,  0.0f,  // Top vertex position
      0.8f,  0.8f,  0.0f};

  const std::vector<GLuint> idx = {0, 1, 2, 2, 1, 3};

  m_program.bind();

  // vertex buffer
  m_vertbuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertbuf.create();
  m_vertbuf.bind();
  m_vertbuf.allocate(vertices.data(), vertices.size() * sizeof(GL_FLOAT));

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
  // m_program.enableAttributeArray(0);
  // m_program.setAttributeBuffer(0, GL_UNSIGNED_INT, 0, )

  m_vao.release();
  m_program.release();

  glViewport(0, 0, width(), height());
}

void BasicWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void BasicWidget::paintGL()
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TODO:  render.
  m_program.bind();
  m_vao.bind();

  // qDebug() << m_idxbuf.size() / sizeof(GLuint);
  glDrawElements(GL_TRIANGLES, m_idxbuf.size() / sizeof(GLuint),
                 GL_UNSIGNED_INT, 0);

  m_vao.release();
  m_program.release();
}
