#include "Particle.h"

#include <QMatrix4x4>
#include <QVector3D>
#include <memory>

Particle::Particle(Renderable* model, const QVector3D& initialVelocity,
                   float lifespan)
    : m_model(model), m_velocity(initialVelocity), m_lifespan(lifespan), m_timeToLive(lifespan)
{
}

void Particle::update(int dt) {
  m_model->update(dt);
  m_timeToLive -= dt;
}

bool Particle::isDead() const { return m_timeToLive < 0.0; }
