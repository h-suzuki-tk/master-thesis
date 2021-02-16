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
            Cell(const std::vector<int> index);
			std::vector<int>& index() { return m_index; }
			std::vector<int>& pts()   { return m_pts; }

        private:
			const std::vector<int> m_index;
			std::vector<int>       m_pts;
    };

	class Cells {

		class Node {

			public:
				Node();
				Node(const std::vector<int>& parent_entry,	const int& index);

			private:
				union Child {
					Node next;
					Cell cell;
				};
				std::vector<int>   m_entry;
				std::vector<Child> m_children;
		};

		public:
			Cells(Grid* gds, const std::vector<std::vector<int>>& belongGrid);
			std::vector<int>& pts(const std::vector<int>& index);
			std::vector<Cell&> all(); 

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
