#ifndef _GRID_HPP_
#define _GRID_HPP_

#include <iostream>
#include <cassert>
#include <queue>
#include "data.hpp"
#include "dnnhs.hpp"

namespace HS::DNNHS {
class Grid : public DNNHS {

    class Cell {

        public:
            Cell(const std::vector<int> index);
			Cell(const std::vector<int>& parent_entry, const int& index);
			std::vector<int>& index() { return m_index; }
			std::vector<int>& pts()   { return m_pts; }

        private:
			std::vector<int> m_index;
			std::vector<int> m_pts;
    };

	class Cells {

		class Node {

			public:
				Node();
				Node(const std::vector<int>& parent_entry,	const int& index);

				std::vector<int>&  entry()                { return m_entry; }
				std::vector<Node>& next()                 { return m_next; }
				Node&              next(const int& index) { return m_next[index]; }
				std::vector<Cell>& cell()                 { return m_cell; }
				Cell&              cell(const int& index) { return m_cell[index]; }

			private:
				std::vector<int>  m_entry;
				std::vector<Node> m_next;
				std::vector<Cell> m_cell;

		};

		public:
			Cells();
			Cells(Grid* gds, const std::vector<std::vector<int>>& belongGrid);
			std::vector<int>& pts(const std::vector<int>& index);
			std::vector<Cell*> all(); 

		private:
			Grid* m_gds;
			Node  m_root;
			
	};

    public:
        Grid(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha, const int& gridSize, const std::vector<std::vector<int>>& belongGrid);
        int run();
        //void printResult();

    protected:

    private:
        Cells     m_cells;
        const int m_grid_size;

};
}

#endif
