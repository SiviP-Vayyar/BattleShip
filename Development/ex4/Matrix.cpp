#include <iostream>
#include <vector>

template<class T, size_t DIMENSIONS>
class Matrix {
public:
    typedef std::initializer_list<typename Matrix<T, DIMENSIONS-1>::listType> listType;
    typedef Matrix<T, DIMENSIONS-1> elemType;

    Matrix(const listType& initList) {
        for (auto& listElem : initList) {
            m.emplace_back(listElem);
        }
    }
private:
    std::vector<elemType> m;    
};

// specialization for 1d == Array
template<class T>
class Matrix<T, 1> {
public:
    typedef std::initializer_list<T> listType;
    typedef T elemType;

    Matrix(const listType& initList) {
        for (auto& listElem : initList) {
            m.push_back(listElem);
        }
    }
private:
    std::vector<elemType> m;
};



template<class T>
using Matrix2d = Matrix<T, 2>;

template<class T>
using Matrix3d = Matrix<T, 3>;



template<typename Groups>
void print(const Groups& all_groups) {
    using namespace std;
    for(const auto& groupType : all_groups) {
        cout << groupType.first << ":" << endl;
        for(const auto& groupOfType : groupType.second) {
            for(const auto& coord : groupOfType) {
                cout << "{ ";
                for(int i : coord) {
                    cout << i << ' ';
                }
                cout << "} ";
            }
            cout << endl;
        }
    }
}

int main() {
    std::cout << "Starting main" << std::endl;
    Matrix2d<char> m = {{'a', 'A', 'a'}, {'B', 'a', 'B'}, {'B', 'a', 'B'}};
    // auto all_groups = m.groupValues([](auto i){return islower(i)? "L": "U";});
    // print(all_groups);
}
