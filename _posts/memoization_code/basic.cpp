#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

double funnyThing(int a, double b) {
    return 3.141 * a * b;
}

template <typename T>
struct MemoizeG;

template<typename ResultType, typename... ArgTypes>
class MemoizeG<ResultType(ArgTypes...)>
{
private:
    using StoredResultType = std::pair<std::tuple<ArgTypes...>, ResultType>;
public:
    MemoizeG(std::function<ResultType(ArgTypes...)> f) : _f(f) {}
    
    ResultType operator()(ArgTypes... args) {
        const auto argsAsTuple = std::make_tuple(args...);
        auto memoized = std::find_if(_table.begin(), _table.end(), [&argsAsTuple](const StoredResultType& candidate) {
            return candidate.first == argsAsTuple;
        });
        if(memoized == _table.end()) {
            auto const r = _f(args...);
            _table.push_back({ {args...}, r});
            std::cout << "computed: ...->" << r  << std::endl;
            return r;
        } else {
            std::cout << "memoized: ...->" << memoized->second << std::endl;
            return memoized->second;
        }
    }
private:
    std::vector<StoredResultType> _table;
    std::function<ResultType(ArgTypes...)> _f;
};

template<typename ResultType, typename... ArgsTypes>
MemoizeG<ResultType(ArgsTypes...)> MakeMemoizeG(ResultType f(ArgsTypes...)) {
    return MemoizeG<ResultType(ArgsTypes...)>(f);
}

int main() {
    auto memoizedFunnyThing = MemoizeG<double(int, double)>(funnyThing);
    memoizedFunnyThing(1, 1.1);
    memoizedFunnyThing(2, 1.1);
    memoizedFunnyThing(3, 1.1);
    memoizedFunnyThing(1, 1.1);
    
    auto convenientMemoizedFunnyThing = MakeMemoizeG(funnyThing);
    convenientMemoizedFunnyThing(1, 1.1);
    convenientMemoizedFunnyThing(1, 1.1);
}
