#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <iostream>
#include <utility>

class ConcretePrinter {
public:
     void Print() const { std::cout << "ConcretePrinter" << std::endl; }   
};

class StonePrinter {
public:
     void Print() const { std::cout << "StonePrinter" << std::endl; }     
};

class AnyPrinter {
public:
    template <typename T>
    AnyPrinter(T x) : self_(boost::make_shared<model<T>>(std::move(x))) {}
 
    void Print() const
    { self_->Print_(); }
 
private:
    struct concept_t {
        virtual ~concept_t() = default;
        virtual void Print_() const = 0;
    };
    template <typename T>
    struct model : concept_t {
        model(T x) : data_(std::move(x)) {}
        void Print_() const override
        { data_.Print(); }
 
        T data_;
    };
   
    boost::shared_ptr<const concept_t> self_;
};

int main()
{
    ConcretePrinter cp;
    
    StonePrinter sp;

    std::vector<AnyPrinter> printables;
    printables.push_back(cp); //pushing copies
    printables.push_back(sp);

    for(const auto& p : printables) {
        p.Print();
    }
}