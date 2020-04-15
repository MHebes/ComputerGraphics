#include "Renderable.h"

#include <QtGui>
#include <QtOpenGL>

#include "Light.h"

#define VERT_SHADER "@VERT_SHADER@"  // CMAKE: VERT_SHADER
#define FRAG_SHADER "@FRAG_SHADER@"  // CMAKE: FRAG_SHADER

const unsigned MAX_LIGHTS = 10;

Renderable::Renderable()
    : m_vbo(QOpenGLBuffer::VertexBuffer),
      m_ibo(QOpenGLBuffer::IndexBuffer),
      m_texture(QOpenGLTexture::Target2D),
      m_normalmap(QOpenGLTexture::Target2D),
      m_numTris(0),
      m_vertexSize(0),
      m_rotationAxis(0.0, 0.0, 1.0),
      m_rotationSpeed(0)
{
  m_rotationAngle = 0.0;
}

Renderable::~Renderable()
{
  if (m_texture.isCreated()) {
    m_texture.destroy();
  }
  if (m_normalmap.isCreated()) {
    m_normalmap.destroy();
  }
  if (m_vbo.isCreated()) {
    m_vbo.destroy();
  }
  if (m_ibo.isCreated()) {
    m_ibo.destroy();
  }
  if (m_vao.isCreated()) {
    m_vao.destroy();
  }
}

void Renderable::createShaders()
{
  bool ok =
      m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, VERT_SHADER);
  if (!ok) {
    qDebug() << m_shader.log();
  }
  ok = m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, FRAG_SHADER);
  if (!ok) {
    qDebug() << m_shader.log();
  }
  ok = m_shader.link();
  if (!ok) {
    qDebug() << m_shader.log();
  }
}

void Renderable::init(const QVector<QVector3D>& positions,
                      const QVector<QVector3D>& normals,
                      const QVector<QVector2D>& texCoords,
                      const QVector<QVector3D>& tangents,
                      const QVector<QVector3D>& bitangents,
                      const QVector<unsigned int>& indexes,
                      const QString& textureFile, const QString& normalMap)
{
  // We need to make sure our sizes all work out ok.
  if (positions.size() != texCoords.size() ||
      positions.size() != normals.size()) {
    qDebug() << "[Renderable]::init() -- positions size mismatch with "
                "normals/texture coordinates";
    return;
  }

  // Set our model matrix to identity
  m_modelMatrix.setToIdentity();

  // TODO got to be smarter about this mirroring somehow

  // Load our texture
  QImage img(textureFile);
  m_texture.setData(img.mirrored(true, true));

  // Load our normal map
  QImage norm(normalMap);
  m_normalmap.setData(norm.mirrored(true, true));

  // set our number of triangles.
  m_numTris = indexes.size() / 3;

  // num verts (used to size our vbo)
  int numVerts = positions.size();
  m_vertexSize = 3 + 3 + 2 + 3 +
                 3;  // Position + normal + texCoord + tangents + bitangents
  int numVBOEntries = numVerts * m_vertexSize;

  // Setup our shader.
  createShaders();

  // Now we can set up our buffers.
  // The VBO is created -- now we must create our VAO
  m_vao.create();
  m_vao.bind();

  m_vbo.create();
  m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vbo.bind();

  // Create a temporary data array
  float* data = new float[numVBOEntries];

  // fill the first three elements
  for (int i = 0; i < numVerts; ++i) {
    data[i * m_vertexSize + 0] = positions.at(i).x();
    data[i * m_vertexSize + 1] = positions.at(i).y();
    data[i * m_vertexSize + 2] = positions.at(i).z();

    data[i * m_vertexSize + 3] = normals.at(i).x();
    data[i * m_vertexSize + 4] = normals.at(i).y();
    data[i * m_vertexSize + 5] = normals.at(i).z();

    data[i * m_vertexSize + 6] = texCoords.at(i).x();
    data[i * m_vertexSize + 7] = texCoords.at(i).y();
  }

  // TODO(mike) warning: really ugly stuff below
  // fill the TN parts (this vector is a different structure - see Util.h)
  for (unsigned int i = 0; i < m_numTris; i++) {
    // find where in the data to put these TB parts
    unsigned int a = indexes.at(3*i);
    unsigned int b = indexes.at(3*i + 1);
    unsigned int c = indexes.at(3*i + 2);

    // put a's TB where it belongs
    data[a * m_vertexSize + 8] = tangents.at(i).x();
    data[a * m_vertexSize + 9] = tangents.at(i).y();
    data[a * m_vertexSize + 10] = tangents.at(i).z();
    data[a * m_vertexSize + 11] = bitangents.at(i).x();
    data[a * m_vertexSize + 12] = bitangents.at(i).y();
    data[a * m_vertexSize + 13] = bitangents.at(i).z();

    // and b's
    data[b * m_vertexSize + 8] = tangents.at(i).x();
    data[b * m_vertexSize + 9] = tangents.at(i).y();
    data[b * m_vertexSize + 10] = tangents.at(i).z();
    data[b * m_vertexSize + 11] = bitangents.at(i).x();
    data[b * m_vertexSize + 12] = bitangents.at(i).y();
    data[b * m_vertexSize + 13] = bitangents.at(i).z();

    // and c's
    data[c * m_vertexSize + 8] = tangents.at(i).x();
    data[c * m_vertexSize + 9] = tangents.at(i).y();
    data[c * m_vertexSize + 10] = tangents.at(i).z();
    data[c * m_vertexSize + 11] = bitangents.at(i).x();
    data[c * m_vertexSize + 12] = bitangents.at(i).y();
    data[c * m_vertexSize + 13] = bitangents.at(i).z();
  }

  m_vbo.allocate(data, numVBOEntries * sizeof(float));
  delete[] data;

  // Create our index buffer
  m_ibo.create();
  m_ibo.bind();
  m_ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);

  // create a temporary array for our indexes
  unsigned int* idxAr = new unsigned int[indexes.size()];
  for (int i = 0; i < indexes.size(); ++i) {
    idxAr[i] = indexes.at(i);
  }
  m_ibo.allocate(idxAr, indexes.size() * sizeof(unsigned int));
  delete[] idxAr;

  // positions
  m_shader.enableAttributeArray(0);
  m_shader.setAttributeBuffer(0, GL_FLOAT, 0, 3, m_vertexSize * sizeof(float));

  // normals
  m_shader.enableAttributeArray(1);
  m_shader.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3,
                              m_vertexSize * sizeof(float));

  // uvs
  m_shader.enableAttributeArray(2);
  m_shader.setAttributeBuffer(2, GL_FLOAT, (3 + 3) * sizeof(float), 2,
                              m_vertexSize * sizeof(float));

  // tangents
  m_shader.enableAttributeArray(3);
  m_shader.setAttributeBuffer(3, GL_FLOAT, (3 + 3 + 2) * sizeof(float), 3,
                              m_vertexSize * sizeof(float));

  // bitangents
  m_shader.enableAttributeArray(4);
  m_shader.setAttributeBuffer(4, GL_FLOAT, (3 + 3 + 2 + 3) * sizeof(float), 3,
                              m_vertexSize * sizeof(float));

  // Release our vao and THEN release our buffers.
  m_vao.release();
  m_vbo.release();
  m_ibo.release();
}

void Renderable::update(const qint64 msSinceLastFrame)
{
  // For this lab, we want our polygon to rotate.
  float sec = msSinceLastFrame / 1000.0f;
  float anglePart = sec * m_rotationSpeed * 360.f;
  m_rotationAngle += anglePart;
  while (m_rotationAngle >= 360.0) {
    m_rotationAngle -= 360.0;
  }
}

void Renderable::draw(const QMatrix4x4& view, const QMatrix4x4& projection,
                      const QVector<Light*>& lights)
{
  // Create our model matrix.
  QMatrix4x4 rotMatrix;
  rotMatrix.setToIdentity();
  rotMatrix.rotate(m_rotationAngle, m_rotationAxis);

  QMatrix4x4 modelMat = m_modelMatrix * rotMatrix;
  // Make sure our state is what we want
  m_shader.bind();
  // Set our matrix uniforms!
  QMatrix4x4 id;
  id.setToIdentity();
  m_shader.setUniformValue("modelMatrix", modelMat);
  QVector3D cameraPos = view.inverted().column(3).toVector3D();
  // qDebug() << cameraPos;
  m_shader.setUniformValue("viewPos", cameraPos);
  m_shader.setUniformValue("viewMatrix", view);
  m_shader.setUniformValue("projectionMatrix", projection);

  // Setup lights
  int numLights = lights.size();
  if (numLights > MAX_LIGHTS) numLights = MAX_LIGHTS;

  m_shader.setUniformValue("numLights", numLights);

  for (int i = 0; i < numLights; i++) {
    lights[i]->applyToUniform(m_shader, "lights[" + std::to_string(i) + "]");
  }

  m_shader.setUniformValue("diffuseMap", 0);
  m_shader.setUniformValue("normalMap", 1);

  // setup textures
  m_vao.bind();

  m_texture.bind(0);
  m_normalmap.bind(1);

  glDrawElements(GL_TRIANGLES, m_numTris * 3, GL_UNSIGNED_INT, 0);

  m_normalmap.release(1);
  m_texture.release(0);

  m_vao.release();
  m_shader.release();
}

void Renderable::setModelMatrix(const QMatrix4x4& transform)
{
  m_modelMatrix = transform;
}

void Renderable::setRotationAxis(const QVector3D& axis)
{
  m_rotationAxis = axis;
}

void Renderable::setRotationSpeed(float speed) { m_rotationSpeed = speed; }
