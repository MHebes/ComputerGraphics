#pragma once

#include "Scene.h"
#include "Sphere.h"

class Planet : public Scene {
private:
  float m_angle;
  std::string m_texture;
  float m_radius;
  float m_size;
  float m_revolutionsPerSecond;
  // std::weak_ptr<Scene> m_parent;

public:
  Planet(std::string texture, float radius, float size, float angleOffset,
         float revolutionsPerSecond)
      : m_texture(texture),
        m_radius(radius),
        m_size(size),
        m_angle(angleOffset),
        m_revolutionsPerSecond(revolutionsPerSecond)
  {
  }

  virtual void onInit()
  {
    QMatrix4x4 mat;
    mat.translate(m_radius, 0, 0);
    setLocalTransform(mat);
    // setParent(m_parent);

    auto planet = std::make_unique<Sphere>(m_texture);
    planet->setModelMatrix(mat);
    setRenderable(std::move(planet));
  }

  virtual void onUpdate(float ms) {}
};