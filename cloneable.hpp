#ifndef CLONEABLE_HPP
#define CLONEABLE_HPP

#include <iostream>
#include <memory>

template<typename Base, typename Derived>
class Cloneable : public Base {
public:
    virtual ~Cloneable() = default;

    // We could use the override specifier here (as of C++11),
    // but it wouldn't work when the base class doesn't have
    // a clone() method defined.
    //
    // Thus we're heavily relying on the fact that the virtual
    // specifier can be used both in the base and the derived
    // class's declaration of the function.
    virtual Base* clone() {
        return new Derived;
    }
};

class BaseCloneable {
public:
    virtual ~BaseCloneable() = default;

    virtual BaseCloneable* clone() = 0;
};

class YetAnotherBaseClass {
public:
    virtual ~YetAnotherBaseClass() = default;

    virtual void greet() = 0;
};

class BaseIncompatibleDerived : public Cloneable<YetAnotherBaseClass, BaseIncompatibleDerived> {
public:
    void greet() override {
        std::cout << "Greetings!" << std::endl;
    }
};

class NonTrivialDerived : public BaseCloneable {
public:
    BaseCloneable* clone() override {
        std::cout << "This is a non-trivial cloning procedure, please wait..." << std::endl;
        return new NonTrivialDerived;
    }
};

class TrivialDerived : public Cloneable<BaseCloneable, TrivialDerived> {
public:
    void sayHi() {
        std::cout << "Hey there!" << std::endl;
    }
};

//int main() {
//    NonTrivialDerived ntd;
//    TrivialDerived td;
//    td.sayHi();
//
//    BaseCloneable& ref_ntd = ntd;
//    BaseCloneable& ref_td = td;
//
//    BaseCloneable* clone_ntd = ref_ntd.clone();
//    delete clone_ntd;
//    BaseCloneable* clone_td = ref_td.clone();
//    static_cast<TrivialDerived&>(*clone_td).sayHi();
//    delete clone_td;
//
//
//    // it is not compatible with the BaseCloneable interface, but still has a clone() that is automatically implemented
//    BaseIncompatibleDerived bid;
//    bid.greet();
//
//    // from a "YetAnotherBaseClass-perspective" it is not a cloneable class whatsoever
//    YetAnotherBaseClass& ref_bid = bid;
//    // but the virtual function declared by it can still be used
//    ref_bid.greet();
//
//    // and
//    YetAnotherBaseClass* clone_bid = bid.clone();
//    clone_bid->greet();
//    delete clone_bid;
//
//    return 0;
//}

#endif // CLONEABLE_HPP
