#pragma once

#include <QtGui>
#include <QVector3D>
#include <memory>

#include "Renderable.h"

/**
 * @brief Private particle class, only created by Emitters
 *
 */
class Particle {
  friend class Emitter;

private:
  /**
   * @brief Construct a new Particle object
   *
   * @param model (non-owning) reference
   * @param initialVelocity
   * @param lifespan in ms
   */
  Particle(Renderable* model, const QVector3D& initialVelocity, float lifespan);

  void update(int dt);

  bool isDead() const;

  Renderable* m_model;
  QMatrix4x4 m_transform;
  QVector3D m_velocity;
  float m_lifespan; // ms
  float m_timeToLive; // ms
};