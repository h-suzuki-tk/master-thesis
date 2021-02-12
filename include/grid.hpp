#ifndef _GRID_HPP_
#define _GRID_HPP_

#include <iostream>
#include <cassert>
#include "data.hpp"

#define DEFAULT_DIM 2

namespace HS::DNNHS {
class Grid : public DNNHS {

    class Cell {

        public:
            Cell();
            Cell(DNNHS* ds);

        private:
          DNNHS* m_ds;  

    };

    public:
        Grid(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha, const int&             gridSize);
        //void run();
        //void printResult();

    protected:

    private:
        HS::MultiVector<Cell> m_cells;
        const int             m_grid_size;

};
}

#endif
