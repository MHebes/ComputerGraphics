#pragma once

#include <QMatrix4x4>
#include <memory>
#include <optional>
#include <vector>

#include "Renderable.h"

/**
 * @brief Represents a node in the tree
 *
 */
class Scene : std::enable_shared_from_this<Scene> {
public:
  // create root scene
  Scene();

  // create scene with parent
  Scene(std::weak_ptr<Scene> parent);

  // create scene with parent and object
  Scene(std::weak_ptr<Scene> parent, std::unique_ptr<Renderable> renderable);

  // change the parent of this scene
  void setParent(std::weak_ptr<Scene> parent);

  // change the renderable of this scene
  void setRenderable(std::unique_ptr<Renderable> renderable);

  // add a child scene
  void addChild(std::shared_ptr<Scene> child);

  // update this scene's renderable and all its children
  void update(int msSinceLastUpdate);

  // draw this scene's renderable (if it exists) and all its children
  void draw(const QMatrix4x4& view, const QMatrix4x4& projection,
            const QVector<Light*>& lights);

  QMatrix4x4 localTransform() { return m_localTransform; }

  QMatrix4x4 worldTransform() { return m_worldTransform; }

  // set the local transform & update the world transform for this and all
  // descendant scenes
  void setLocalTransform(const QMatrix4x4& transform);

  // TODO
  // void setWorldTransform(const QMatrix4x4& transform);

private:
  std::optional<std::weak_ptr<Scene>> m_parent;  // can be invalid
  std::vector<std::shared_ptr<Scene>> m_children;
  std::unique_ptr<Renderable> m_renderable;

  QMatrix4x4 m_localTransform;
  QMatrix4x4 m_worldTransform;

protected:
  // locks the parent to get its worldtranform
  void computeWorldTransform();

  // allows to pass the parent's transform explicitly for recursion
  void computeWorldTransform(const QMatrix4x4& parentWorldTransform);

  void removeChild(std::shared_ptr<Scene> child);
};