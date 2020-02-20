#include "BasicWidget.h"

//////////////////////////////////////////////////////////////////////
// Publics
BasicWidget::BasicWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      m_vertbuf(QOpenGLBuffer::VertexBuffer),
      m_normalbuf(QOpenGLBuffer::VertexBuffer),
      m_idxbuf(QOpenGLBuffer::IndexBuffer),
      m_is_wireframe(false),
      m_eye(0.0f, 1.0f, 2.0f),
      m_center(0.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_light_pos(10.0f, 10.0f, 10.0f)
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

// Lighting code based on https://learnopengl.com/Lighting/Basic-Lighting
QString BasicWidget::m_vert_shader_src = R"(
#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aNormal;

out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  // move everything according to camera pos/perspective and model position
  gl_Position = projection * view * model * vec4(position, 1.0);
  fragPos = vec3(model * vec4(position, 1.0));
  normal = vec3(model * vec4(aNormal, 1.0));
};
)";

QString BasicWidget::m_frag_shader_src = R"(
#version 330

in vec3 fragPos;
in vec3 normal;

// just make everything orange
out vec4 color;

uniform vec3 lightPos;

void main()
{
  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

  color = vec4(diffuse, 1.0);
}
)";


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
  static const std::vector<GLfloat> verts = m_loader.get_vertices();
  static const std::vector<GLfloat> norms = m_loader.get_normals();
  static const std::vector<GLuint> idx = m_loader.get_idx();

  m_program.bind();

  // vertex buffer
  m_vertbuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertbuf.create();
  m_vertbuf.bind();
  m_vertbuf.allocate(verts.data(), verts.size() * sizeof(GL_FLOAT));

  // normal buffer
  m_normalbuf.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_normalbuf.create();
  m_normalbuf.bind();
  m_normalbuf.allocate(norms.data(), norms.size() * sizeof(GL_FLOAT));

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

  m_normalbuf.bind();
  m_program.enableAttributeArray(1);
  m_program.setAttributeBuffer(1, GL_FLOAT, 0, 3);

  m_idxbuf.bind();

  m_vao.release();

  m_program.setUniformValue("lightPos", m_light_pos);

  m_program.release();

  glViewport(0, 0, width(), height());
}

void BasicWidget::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);

  m_projection.perspective(70.0f, (float)w/h, 0.01f, 100.0f);

  m_program.bind();

  m_program.setUniformValue("model", m_model);
  m_program.setUniformValue("view", m_view);
  m_program.setUniformValue("projection", m_projection);
}

void BasicWidget::paintGL()
{
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // TODO:  render.
  m_program.bind();
  m_vao.bind();

  // TODO use the number of triangles rather than buffer size
  glDrawElements(GL_TRIANGLES, m_idxbuf.size() / sizeof(GLuint), GL_UNSIGNED_INT, 0);

  m_vao.release();
  m_program.release();
}
