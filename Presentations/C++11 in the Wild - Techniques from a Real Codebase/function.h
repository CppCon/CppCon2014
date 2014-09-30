#pragma once
#include <utility>

struct ContainerBase {
  virtual void perform() = 0;
  virtual ~ContainerBase() = default;
};

template <class Lambda> struct Container : ContainerBase {
  Lambda m_lambda;
  Container(Lambda&& lambda) : m_lambda(std::move(lambda)) {}
  virtual void perform() { m_lambda(); }
};

class function {  // equivalent to std::function<void(void)>
 ContainerBase *m_ctr;
public:
  template<class Lambda> function(Lambda lambda)
    : m_ctr(new Container<Lambda>(std::move(lambda))) {}
  void operator()() { m_ctr->perform(); }
  ~function() { delete m_ctr; }
};
