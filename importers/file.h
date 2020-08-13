//
// Created by Brian Landers on 8/12/20.
//

#ifndef RAY_TRACING2_FILE_H
#define RAY_TRACING2_FILE_H

class File {
 protected:
  std::shared_ptr<Group> default_group_;
  std::unordered_map<std::string, std::shared_ptr<Group>> named_groups_;

 public:
  explicit File(const std::string& blob, bool normalize = false)
      : default_group_(std::make_shared<Group>()), named_groups_({}) {}
  std::__1::shared_ptr<Group> to_group() {
    auto group = std::__1::make_shared<Group>();
    for (auto& c : default_group_->children()) {
      c->set_parent(group.get());
      group->add(c);
    }

    for (const auto& [k, v] : named_groups_) {
      group->add<Group>(v);
    }
    return group;
  }
  virtual ~File() = default;
  virtual Camera* camera() const = 0;
  virtual PointLight* light() const = 0;
};

#endif  // RAY_TRACING2_FILE_H
