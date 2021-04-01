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
			union Child {
				Node* node;
				Cell* cell;

				public:
					Child()           { this->node = nullptr; }
					Child(Node* node) { this->node = node; }
					Child(Cell* cell) { this->cell = cell; }

					bool isNull() { return this->node == nullptr; };
			};

			public:
				Node( const int grid_size );
				Node( const int grid_size, const std::vector<int>& parent_entry, const int& index);
				~Node();

				std::vector<int>&   entry   ()                 { return m_entry; }
				std::vector<Child>& children()                 { return m_children; }
				union Child&        child   (const int& index) { return m_children[index]; }
				
				int depth() { return m_entry.size(); }

			private:
				std::vector<int>   m_entry;
				std::vector<Child> m_children;

		};

		public:
			Cells();
			Cells(Grid* gds, const std::vector<std::vector<int>>& belongCell);
			~Cells();
			
			Cell*              operator[](const std::vector<int>& idx);
			std::vector<Cell*> all(); 
			double             side()    const { return m_side; }

			bool               isEmpty() const { return m_root == nullptr; }

			Cell&              add( const std::vector<int>& idx ); /** TODO: **/
			void               remove(const std::vector<int>& idx);

		private:
			Grid*        m_gds;
			Node*        m_root;
			double m_side;
			
	};

	class ExpansionCells {

		static constexpr int DEFAULT_STAGE = 0;

    	public:
    		ExpansionCells( Grid* gds, const Eigen::VectorXd& core_pt );

			void expand();
			void reset(); /** TODO: **/

			double              gtdNNRange() const { return m_gtd_nn_range; }
			std::vector<Cell*>& cells()            { return m_cells; }
			std::vector<int>    pts();
			std::vector<int>    core()             { return m_core_cell; }

			bool                isOverBound()      { return m_is_over_bound; }

		private:
			Grid*              m_gds;
			Eigen::VectorXd    m_core_pt;
			std::vector<int>   m_core_cell;
			int                m_stage;
			double             m_gtd_nn_range;
			std::vector<Cell*> m_cells;
			Cells              m_buf_cells;
			bool               m_is_over_bound;

			std::vector<Cell*> expansionCells( const std::vector<int>& core_cell, const int stage, const std::vector<int>& lower_bound_cell_idx, const std::vector<int>& upper_bound_cell_idx );
	};

	static constexpr int MIN_CLUSTER_SIZE = 2;
	

    public:
        Grid(const Eigen::MatrixXd& data, const Eigen::VectorXd& query, const int& alpha, const int& gridSize, const std::vector<std::vector<int>>& belongCell);
        int run();
		
		int    gridSize()                      const { return m_grid_size; }
		Cells& cells   ()                            { return m_cells; }
		Cell*  cell    (const std::vector<int>& idx) { return m_cells[idx]; }
		double cellSide()                      const { return m_cell_side; }

    protected:

    private:
        Cells        m_cells;
        const int    m_grid_size;
		const double m_cell_side;
		std::vector<int> m_lower_bound_cell_index;
		std::vector<int> m_upper_bound_cell_index;

		std::vector<int> belongCell(const Eigen::VectorXd& pt);
		std::vector<int> expandCellPts(const std::vector<int>& coreCellIdx, const int stage); /** TODO: いずれ消す **/
		Group            findGroup(const int core_pt);
		void             updateBoundCellIdx(const double bound);
};
}

#endif
