#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <functional>

template <size_t DIMENSIONS>
using Coord = std::array<int, DIMENSIONS>;

template<class T, size_t DIMENSIONS>
class Matrix {
public:
    typedef std::initializer_list<typename Matrix<T, DIMENSIONS-1>::listType> listType;
    typedef Matrix<T, DIMENSIONS-1> elemType;
    typedef Coord<DIMENSIONS> coord;
    typedef std::vector<coord> groupOfCoords;

    Matrix(const listType& initList) {
        for (auto& listElem : initList) {
            m.emplace_back(listElem);
        }
    }
    
    template<class Func, class G = typename std::result_of<Func(T)>::type>
    std::map<G, std::vector<groupOfCoords>> groupValues(Func mappingFunc) {
        std::map<G, std::vector<groupOfCoords>> groups;
        
        for (int i = 0 ; i < m.size() ;) {
            auto layer = m[i];
            auto groupsForLayer = layer.groupValues(mappingFunc);
            i++; // TODO: fill in implementation
        }
        
        // TODO: change to real implementation - copied from 1d specialization
        // for (int i = 0 ; i < m.size() ;) {
        //     auto key = mappingFunc(m[i]);
        //     groupOfCoords group;
        //     for (; i < m.size() && m[i] == key ; i++) {
        //         group.emplace_back(i);
        //     }
        //     groups[key].push_back(std::move(group));
        // }
        
        return groups;
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
    typedef Coord<1> coord;
    typedef std::vector<coord> groupOfCoords;

    Matrix(const listType& initList) {
        for (auto& listElem : initList) {
            m.push_back(listElem);
        }
    }
    
    template<class Func, class G = typename std::result_of<Func(T)>::type>
    std::map<G, std::vector<groupOfCoords>> groupValues(Func mappingFunc) {
        std::map<G, std::vector<groupOfCoords>> groups;
        
        for (int i = 0 ; i < m.size() ;) {
            G key = mappingFunc(m[i]);
            groupOfCoords group;
            for (; i < m.size() && mappingFunc(m[i]) == key ; i++) {
                group.emplace_back(coord{{i}});
            }
            groups[key].emplace_back(std::move(group));
        }
        
        return groups;
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
    auto all_groups = m.groupValues([](auto i){return islower(i)? "L": "U";});
    print(all_groups);
}
