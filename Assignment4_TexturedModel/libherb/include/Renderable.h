#pragma once

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

class Renderable
{
protected:
	// Each renderable has its own model matrix
	QMatrix4x4 m_modelMatrix;
	// For now, we have only one shader per object
	QOpenGLShaderProgram m_shader;
	// For now, we have only one texture per object
	QOpenGLTexture m_texture;
	// For now, we have a single unified buffer per object
	QOpenGLBuffer m_vbo;
	// Make sure we have an index buffer.
	QOpenGLBuffer m_ibo;
	// We have a single draw call, so a single vao
	QOpenGLVertexArrayObject m_vao;
	// Keep track of how many triangles we actually have to draw in our ibo
	unsigned int m_numTris;
	int m_vertexSize;

	// Define our axis of rotation for animation
	QVector3D m_rotationAxis;
	float m_rotationSpeed;
	float m_rotationAngle;

	// Create our shader and fix it up
	void createShaders();

public:
	Renderable();
	virtual ~Renderable();

	// When we initialize our renderable, we pass it normals.  We 
	// currently don't use normals in our implementation, but the array is checked
	// for the appropriate size.  The values can be all 0, but must be the same size as
	// the position array!
	virtual void init(const QVector<QVector3D>& positions, const QVector<QVector3D>& normals, const QVector<QVector2D>& texCoords, const QVector<unsigned int>& indexes, const QString& textureFile);
	virtual void update(const qint64 msSinceLastFrame);
	virtual void draw(const QMatrix4x4& view, const QMatrix4x4& projection);

	void setModelMatrix(const QMatrix4x4& transform);
	void setRotationAxis(const QVector3D& axis);
	void setRotationSpeed(float speed);

private:

};