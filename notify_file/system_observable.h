#ifndef SYSTEM_OBSERVABLE_H_BB567786
#define SYSTEM_OBSERVABLE_H_BB567786

#include <memory>

class SystemObservable {
 public:
  template <typename T>
  SystemObservable(T&& monitor, const typename T::ProcessConsumer& cons)
      : holder(new Holder<T>{std::forward<T>(monitor), cons}) {}

  void handleIncommingEvents() { holder->handleIncommingEvents(); }

  void process() { holder->process(); }

  int getFileDescriptor() const { return holder->getFileDescriptor(); }

  bool match(int fileDescriptor) const {
    return fileDescriptor == holder->getFileDescriptor();
  }

 private:
  class HolderBase {
   public:
    virtual ~HolderBase() = default;
    virtual void process() = 0;
    virtual void handleIncommingEvents() = 0;
    virtual int getFileDescriptor() const = 0;
  };

  template <typename WRAPPED>
  class Holder : public HolderBase {
   public:
    WRAPPED wrapped;
    typename WRAPPED::ProcessConsumer consumer;

    Holder(WRAPPED&& w, const typename WRAPPED::ProcessConsumer& cons)
        : wrapped(std::move(w)), consumer(cons) {}

    virtual ~Holder() = default;

    virtual void process() override { wrapped.process(consumer); }

    virtual void handleIncommingEvents() override {
      wrapped.handleIncommingEvents();
    }

    virtual int getFileDescriptor() const override {
      return wrapped.getFileDescriptor();
    }
  };

  std::unique_ptr<HolderBase> holder;
};

#endif  // ! SYSTEM_OBSERVABLE_H_BB567786
