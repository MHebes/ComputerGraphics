#include "Emitter.h"

#include <QVector3D>

#include "Renderable.h"

Emitter::Emitter(const QVector3D& position, const QVector3D& orientation,
                 float emitRate, Renderable* particleModel,
                 const QVector3D& initialVelocity, unsigned int lifespan)
    : m_particles(),
      m_position(position),
      m_orientation(orientation),
      m_timeBetweenParticlesMs(1000 / emitRate),
      m_timeToNextEmission(m_timeBetweenParticlesMs),
      m_particleModel(particleModel),
      m_initialVelocity(initialVelocity),
      m_lifespan(lifespan)
{
}

/**
 * @brief Update the emitters
 *
 * Figures out how many particles need to be emitted
 *
 * @param dt milliseconds since last frame
 */
void Emitter::update(int dt)
{
  m_timeToNextEmission -= dt;

  while (m_timeToNextEmission < 0) {
    // how long the particle should have been alive
    unsigned int timeAlive = -m_timeToNextEmission;

    Particle* particle =
        new Particle(m_particleModel, m_initialVelocity, m_lifespan);
    particle->update(timeAlive);
    m_particles.push_back(particle);

    m_timeToNextEmission += dt;
  }

  // delete dead ones
  m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
                                   [](Particle* p) { return p->isDead(); }));

  for (Particle* p : m_particles) {
    p->update(dt);
  }
}

// deletes particles
Emitter::~Emitter()
{
  for (Particle* p : m_particles) {
    delete p;
  }
}

/**
 * @brief Emit a particle
 *
 */
void Emitter::emitParticle()
{
  Particle* particle =
      new Particle(m_particleModel, m_initialVelocity, m_lifespan);
  m_particles.push_back(particle);
}