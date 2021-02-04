#ifndef _GRID_DNNHS_HPP_
#define _GRID_DNNHS_HPP_

#include <iostream>
#include <cassert>

#define DEFAULT_DIM 2

namespace HS {
class GridDNNHSearch {

    public:
        GridDNNHSearch() { }
        void setData(const std::string dataPath, const int dataSize, const int dataDim, const int gridSize);
        //setQuery();
        void run(const double alpha = 2.0);
        //void printResult();

    protected:

    private:
        //multiVector<Cell> m_cells; /***/

        /*
        template <class T, size_t Dim>
        class multi_vector : public std::vector<multi_vector<T, Dim-1> > {
            public:
                template <typename N, typename... Sizes>
                multi_vector(T i, N n, Sizes... sizes) : std::vector<multi_vector<T, Dim-1> >(n, multi_vector<T, Dim-1>(i, sizes...)) { }
        };

        template <class T>
        class multi_vector<T, 1> : public std::vector<T> {
            public:
                template <typename N>
                multi_vector(T i, N n) : std::vector<T>(n, i) { }
        };
        */


};
}

#endif
