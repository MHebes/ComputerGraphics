#pragma once

#include <QVector3D>
#include <memory>

#include "Particle.h"
#include "Renderable.h"

const unsigned int DEFAULT_POOL_SIZE = 100;

class Emitter {
public:
  /**
   * @brief Construct a new Emitter object
   *
   * @param position
   * @param orientation
   * @param emitRate in particles/second
   * @param particleModel (non owning) reference to the model
   * @param initialVelocity
   * @param lifespan in ms
   */
  Emitter(const QVector3D& position, const QVector3D& orientation,
          float emitRate, Renderable* particleModel,
          const QVector3D& initialVelocity, unsigned int lifespan);

  ~Emitter();

  /**
   * @brief Update the emitters
   *
   * @param dt milliseconds since last frame
   */
  void update(int dt);

  /**
   * @brief Emit a particle
   *
   */
  void emitParticle();

private:
  std::vector<Particle*> m_particles;

  int m_timeToNextEmission;

  QVector3D m_position;
  QVector3D m_orientation;
  float m_timeBetweenParticlesMs;
  Renderable* m_particleModel;
  QVector3D m_initialVelocity;
  unsigned int m_lifespan;
};