#include "Scene.h"

#include <QMatrix4x4>
#include <memory>
#include <optional>

#include "Renderable.h"

Scene::Scene()
    : m_parent(std::nullopt),
      m_children(),
      m_renderable(nullptr),
      m_localTransform(),
      m_worldTransform()
{
  QMatrix4x4 identity;
  computeWorldTransform(identity);
}

Scene::Scene(std::weak_ptr<Scene> parent)
    : m_parent(parent),
      m_children(),
      m_renderable(nullptr),
      m_localTransform(),
      m_worldTransform()
{
  computeWorldTransform();
}

Scene::Scene(std::weak_ptr<Scene> parent,
             std::unique_ptr<Renderable> renderable)
    : m_parent(parent),
      m_children(),
      m_renderable(std::move(renderable)),
      m_localTransform(),
      m_worldTransform()
{
  computeWorldTransform();
}

// change the parent of this scene
void Scene::setParent(std::weak_ptr<Scene> parent)
{
  if (!m_parent || m_parent->expired()) {
    // delete from the parent's list
    std::shared_ptr<Scene> parent = m_parent->lock();

    parent->removeChild(shared_from_this());
  }

  m_parent = parent;

  computeWorldTransform();
}

// change the renderable of this scene
void Scene::setRenderable(std::unique_ptr<Renderable> renderable)
{
  m_renderable = std::move(renderable);
}

// add a child scene
void Scene::addChild(std::shared_ptr<Scene> child)
{
  m_children.push_back(std::move(child));
}

// update this scene's renderable and all its children
void Scene::update(int msSinceLastUpdate)
{
  if (m_renderable) {
    m_renderable->update(msSinceLastUpdate);
  }
  for (auto child : m_children) {
    child->update(msSinceLastUpdate);
  }
}

// draw this scene's renderable (if it exists) and all its children
void Scene::draw(const QMatrix4x4& view, const QMatrix4x4& projection,
                 const QVector<Light*>& lights)
{
  if (m_renderable) {
    m_renderable->draw(m_worldTransform, view, projection, lights);
  }
  for (auto child : m_children) {
    child->draw(view, projection, lights);
  }
}

// set the local transform & update the world transform for this and all
// descendant scenes
void Scene::setLocalTransform(const QMatrix4x4& transform)
{
  m_localTransform = transform;
  computeWorldTransform();
}

void Scene::computeWorldTransform()
{
  if (!m_parent || m_parent->expired()) {
    // don't have a parent for some reason
    m_worldTransform = m_localTransform;
  }
  else {
    // do have a parent
    std::shared_ptr<Scene> parent = m_parent->lock();
    m_worldTransform = parent->worldTransform() * m_localTransform;
  }

  // compute for all children
  for (std::shared_ptr<Scene> child : m_children) {
    child->computeWorldTransform(m_worldTransform);
  }
}

void Scene::computeWorldTransform(const QMatrix4x4& parentWorldTransform)
{
  m_worldTransform = parentWorldTransform * m_localTransform;
}

void Scene::removeChild(std::shared_ptr<Scene> child)
{
  std::remove(m_children.begin(), m_children.end(), child);
}