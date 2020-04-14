#include "Renderable.h"

#include <QtGui>
#include <QtOpenGL>

const char* vertShaderSource = R"(
#version 330
layout(location = 0) in vec3 position;
// Add an input for texture coordinates
in vec2 textureCoords;

// We now have our camera system set up.
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// We define a new output vec2 for our texture coorinates.
out vec2 texCoords;

void main()
{
    // We have our transformed position set properly now
    gl_Position = projectionMatrix*viewMatrix*modelMatrix*vec4(position, 1.0);
    // And we map our texture coordinates as appropriate
    texCoords = textureCoords;
}
)";

const char* fragShaderSource = R"(
#version 330

// Take in our texture coordinate from our vertex shader
in vec2 texCoords;
// We always define a fragment color that we output.
out vec4 fragColor;

// Add a sampler to retrieve our color data from!
uniform sampler2D tex;

void main() {
  // Set our output fragment color to whatever we pull from our input texture (Note, change 'tex' to whatever the sampler is named)
  fragColor = texture(tex, texCoords);
}
)";

Renderable::Renderable() : m_vbo(QOpenGLBuffer::VertexBuffer), m_ibo(QOpenGLBuffer::IndexBuffer), m_texture(QOpenGLTexture::Target2D), m_numTris(0), m_vertexSize(0), m_rotationAxis(0.0, 0.0, 1.0), m_rotationSpeed(0.25)
{
	m_rotationAngle = 0.0;
}

Renderable::~Renderable()
{
	if (m_texture.isCreated()) {
		m_texture.destroy();
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
	bool ok = m_shader.addShaderFromSourceCode(QOpenGLShader::Vertex, vertShaderSource);
	if (!ok) {
		qDebug() << m_shader.log();
	}
	ok = m_shader.addShaderFromSourceCode(QOpenGLShader::Fragment, fragShaderSource);
	if (!ok) {
		qDebug() << m_shader.log();
	}
	ok = m_shader.link();
	if (!ok) {
		qDebug() << m_shader.log();
	}
}

void Renderable::init(const QVector<QVector3D>& positions, const QVector<QVector3D>& normals, const QVector<QVector2D>& texCoords, const QVector<unsigned int>& indexes, const QString& textureFile)
{
	// NOTE:  We do not currently do anything with normals -- we just
	// have it here for a later implementation!
	// We need to make sure our sizes all work out ok.
	// if (positions.size() != texCoords.size() ||
	// 	positions.size() != normals.size()) {
	// 	qDebug() << "[Renderable]::init() -- positions size mismatch with normals/texture coordinates";
	// 	return;
	// }

	// Set our model matrix to identity
	m_modelMatrix.setToIdentity();
	// Load our texture.
	m_texture.setData(QImage(textureFile));

	// set our number of trianges.
	m_numTris = indexes.size() / 3;

	// num verts (used to size our vbo)
	int numVerts = positions.size();
	m_vertexSize = 3 + 2;  // Position + texCoord
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
	for (int i = 0; i < numVerts; ++i) {
		data[i * m_vertexSize + 0] = positions.at(i).x();
		data[i * m_vertexSize + 1] = positions.at(i).y();
		data[i * m_vertexSize + 2] = positions.at(i).z();
		data[i * m_vertexSize + 3] = texCoords.at(i).x();
		data[i * m_vertexSize + 4] = texCoords.at(i).y();
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

	// Make sure we setup our shader inputs properly
	m_shader.enableAttributeArray(0);
	m_shader.setAttributeBuffer(0, GL_FLOAT, 0, 3, m_vertexSize * sizeof(float));
	m_shader.enableAttributeArray(1);
	m_shader.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, m_vertexSize * sizeof(float));

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
	while(m_rotationAngle >= 360.0) {
		m_rotationAngle -= 360.0;
	}
}

void Renderable::draw(const QMatrix4x4& view, const QMatrix4x4& projection)
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
	m_shader.setUniformValue("viewMatrix", view);
	m_shader.setUniformValue("projectionMatrix", projection);

	m_vao.bind();
	m_texture.bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	m_texture.release();
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

void Renderable::setRotationSpeed(float speed)
{
	m_rotationSpeed = speed;
}
