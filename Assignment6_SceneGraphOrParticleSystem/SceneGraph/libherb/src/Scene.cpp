#include "Scene.h"

#include <QMatrix4x4>
#include <iostream>
#include <memory>
#include <optional>

#include "Sphere.h"
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

// change the parent of this scene
void Scene::setParent(std::weak_ptr<Scene> parent)
{
  if (m_parent && !m_parent->expired()) {
    // delete from the current parent's list
    std::shared_ptr<Scene> curParent = m_parent->lock();

    if (curParent) {
      curParent->removeChild(shared_from_this());
    }
  }

  // update the new parent's children list
  if (!parent.expired()) {
    std::shared_ptr<Scene> newParent = parent.lock();

    if (newParent) {
      m_parent = newParent;

      // add to the parent's child list (_addChild won't call any child methods)
      newParent->_addChild(shared_from_this());
    }
  }

  computeWorldTransform();
}

// doesn't call new parent methods (does still delete from the old list)
void Scene::_setParent(std::weak_ptr<Scene> parent)
{
  if (m_parent && !m_parent->expired()) {
    // delete from the current parent's list
    std::shared_ptr<Scene> curParent = m_parent->lock();

    if (curParent) {
      curParent->removeChild(shared_from_this());
    }
  }

  // update the new parent's children list
  if (!parent.expired()) {
    std::shared_ptr<Scene> newParent = parent.lock();

    if (newParent) {
      m_parent = newParent;
    }
  }

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
  child->_setParent(shared_from_this());
  m_children.push_back(std::move(child));
  
  for (auto child : m_children) { 
      child->computeWorldTransform(m_worldTransform);
  }
}

// add a child scene without calling the child's _setParent method
void Scene::_addChild(std::shared_ptr<Scene> child)
{
  m_children.push_back(std::move(child));
  
  for (auto child : m_children) { 
      child->computeWorldTransform(m_worldTransform);
  }
}

// update this scene's renderable and all its children
void Scene::update(int msSinceLastUpdate)
{
  if (m_renderable) {
    m_renderable->update(msSinceLastUpdate);
  }
  onUpdate(msSinceLastUpdate);
  for (auto child : m_children) {
    child->update(msSinceLastUpdate);
  }
}

// draw this scene's renderable (if it exists) and all its children
void Scene::draw(const QMatrix4x4& view, const QMatrix4x4& projection,
                 const QVector<Light*>& lights)
{
  if (m_renderable) {
  // std::cout << "Drawing (" << m_children.size() << " children)" << std::endl;
    m_renderable->draw(m_worldTransform, view, projection, lights);
  }
  for (auto child : m_children) {
    child->draw(view, projection, lights);
  }
}

void Scene::init()
{
  if (m_renderable) {
    m_renderable->init();
  }
  onInit();
  for (auto child : m_children) {
    child->init();
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