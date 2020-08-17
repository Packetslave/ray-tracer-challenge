//
// Created by Brian Landers on 8/12/20.
//

#ifndef RAY_TRACING2_FILE_H
#define RAY_TRACING2_FILE_H

#include "../shapes/group.h"

class File {
 protected:
  std::shared_ptr<Group> default_group_;
  std::unordered_map<std::string, std::shared_ptr<Group>> named_groups_;

 public:
  explicit File(const std::string& blob, bool normalize = false)
      : default_group_(std::make_shared<Group>()), named_groups_({}) {}
  Group* to_group() {
    owned_group_ = std::make_unique<Group>();
    for (auto& c : default_group_->children()) {
      c->set_parent(owned_group_.get());
      owned_group_->add(c);
    }
    return owned_group_.get();
  }
  virtual ~File() = default;
  virtual const Camera* camera() const = 0;
  virtual const Light* light() const = 0;

  std::unique_ptr<Group> owned_group_;
};

#endif  // RAY_TRACING2_FILE_H
