/** @file Sphere.h
 *  @brief Draw a simple sphere primitive.
 *
 *  Draws a simple sphere primitive, that is derived
 *  from the Object class.
 *
 *  @author Mike
 *  @bug No known bugs.
 */

#pragma once

#include <QtGui>
#include <cmath>
#include <iostream>
#include <string>

#include "Renderable.h"
#include "Util.h"

class Sphere : public Renderable {
public:
  // Constructor for the Sphere
  Sphere(std::string texture);

  // The intialization routine for this object.
  void init();

  // Getters for our data.
  QVector<QVector3D> positions() const { return positions_; }
  QVector<QVector3D> normals() const { return normals_; }
  QVector<QVector2D> texCoords() const { return textureCoords_; }
  QVector<QVector3D> tangents() const { return positions_; }
  QVector<QVector3D> bitangents() const { return normals_; }
  QVector<unsigned int> indexes() const { return index_; }

private:
  QVector<QVector3D> positions_;
  QVector<QVector3D> normals_;
  QVector<QVector2D> textureCoords_;
  QVector<QVector3D> tangents_;
  QVector<QVector3D> bitangents_;
  QVector<unsigned int> index_;
  std::string m_texture;

  void calculatePoints();
};
