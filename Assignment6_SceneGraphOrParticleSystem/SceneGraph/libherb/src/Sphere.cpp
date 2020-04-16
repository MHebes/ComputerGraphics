#include "Sphere.h"

#include <QtGui>
#include <cmath>
#include <iostream>

#include "Renderable.h"
#include "Util.h"

#define DEFAULT_NORMAL_MAP "@DEFAULT_NORMAL_MAP@"  // CMake var

// Calls the initalization routine
Sphere::Sphere() {
    calculatePoints();
}

// Algorithm for rendering a sphere
// The algorithm was obtained here: http://learningwebgl.com/blog/?p=1253
// Please review the page so you can understand the algorithm. You may think
// back to your algebra days and equation of a circle! (And some trig with
// how sin and cos work
void Sphere::calculatePoints()
{
  unsigned int latitudeBands = 30;
  unsigned int longitudeBands = 30;
  float radius = 1.0f;
  double PI = 3.14159265359;

  for (unsigned int latNumber = 0; latNumber <= latitudeBands; latNumber++) {
    float theta = latNumber * PI / latitudeBands;
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);

    for (unsigned int longNumber = 0; longNumber <= longitudeBands;
         longNumber++) {
      float phi = longNumber * 2 * PI / longitudeBands;
      float sinPhi = sin(phi);
      float cosPhi = cos(phi);

      float x = cosPhi * sinTheta;
      float y = cosTheta;
      float z = sinPhi * sinTheta;
      // Why is this "1-" Think about the range of texture coordinates
      float u = 1 - ((float)longNumber / (float)longitudeBands);
      float v = 1 - ((float)latNumber / (float)latitudeBands);

      // Setup geometry
      positions_.push_back(
          QVector3D(radius * x, radius * y, radius * z));  // Position
      normals_.push_back(QVector3D(radius * x, radius * y, radius * z));
      textureCoords_.push_back(QVector2D(u, v));  // Texture
    }
  }

  // Now that we have all of our vertices
  // generated, we need to generate our indices for our
  // index element buffer.
  // This diagram shows it nicely visually
  // http://learningwebgl.com/lessons/lesson11/sphere-triangles.png
  for (unsigned int latNumber1 = 0; latNumber1 < latitudeBands; latNumber1++) {
    for (unsigned int longNumber1 = 0; longNumber1 < longitudeBands;
         longNumber1++) {
      unsigned int first = (latNumber1 * (longitudeBands + 1)) + longNumber1;
      unsigned int second = first + longitudeBands + 1;
      index_.push_back(first);
      index_.push_back(second);
      index_.push_back(first + 1);

      index_.push_back(second);
      index_.push_back(second + 1);
      index_.push_back(first + 1);
    }
  }

  // Calculate tangents
  Util::CalculateTangents(positions_, normals_, textureCoords_, index_,
                          tangents_, bitangents_);
}

void Sphere::init(std::string texture)
{
  QString qtexture = QString::fromStdString(texture);
  QString qnormalmap = QString::fromStdString(DEFAULT_NORMAL_MAP);

  Renderable::init(positions_, normals_, textureCoords_, tangents_, bitangents_,
                   index_, qtexture, qnormalmap);
}
