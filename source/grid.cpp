#include "grid.hpp"


HS::DNNHS::Grid::Cell::Cell(
    const std::vector<int> index) :
    m_index(index) {
}


HS::DNNHS::Grid::Cell::Cell(
	const std::vector<int>& parent_entry,
	const int&              index) {

	m_index = parent_entry;
	m_index.emplace_back(index);
}


HS::DNNHS::Grid::Cells::Cells(
	Grid*                                gds,
	const std::vector<std::vector<int>>& belongGrid) :
	m_gds (gds),
	m_root(Node()) {

	// セルの生成
	std::que<Node*> que;
	que.push(&m_root);
	while (!que.empty()) {
		
		Node* n = que.front();
		que.pop();

		if (n->m_entry.size() < gds->m_data.dims()-1) {
			for (int i=0; i<gds->m_grid_size; i++) {
				n->m_children.emplace_back(Node(n->m_entry, i));
				que.push(&(n->m_children[i]));
			}
		} else if (n->m_entry.size() == gds->m_data.dims()-1) {
			std::vector<int> index = n->m_entry;
			index.emplace_back();
			for (int i=0; i<gds->m_grid_size; i++) {
				n->m_children.emplace_back(Cell(n->m_entry, i));
			}
		} else { assert(); }

	}

	// データの所属情報をセルに格納
	for (int i=0; i<gds->m_data.size(); i++) {
		this.pts(belongGrid.row(i)).emplace_back(i);
	}

}


std::vector<int>& HS::DNNHS::Grid::Cells::pts(
	const std::vector<int>& index) {

	auto* n;

	n = &m_root;
	for (auto&& i : index) {
		n = &(n->m_children[i])
	}

	return n->m_pts;
}


std::vector<Cell&> HS::DNNHS::Grid::Cells::all() {

	std::vector<Cell&> cells;

	std::que<Node*> que;
	que.push(&m_root);
	while (!que.empty()) {
		
		Node* n = que.front();
		que.pop();

		if (n->m_entry.size() < gds->m_data.dims()-1) {
			for (int i=0; i<gds->m_grid_size; i++) {
				que.push(&(n->m_children[i]));
			}
		} else if (n->m_entry.size() == gds->m_data.dims()-1) {
			for (int i=0; i<gds->m_grid_size; i++) {
				cells.emplace_back(n->m_children[i]);
			}
		} else { assert(); }

	}
	
	return cells;
}


HS::DNNHS::Grid::Cells::Node::Node() {
}


HS::DNNHS::Grid::Cells::Node::Node(
	const std::vector<int>& parent_entry,
	const int&              index) {

	m_entry = parent_entry;
	m_entry.emplace_back(index);
}


HS::DNNHS::Grid::Grid(
    const Eigen::MatrixXd&               data, 
	const Eigen::VectorXd&               query, 
	const int&                           alpha,
    const int&                           gridSize,
	const std::vector<std::vector<int>>& belongGrid) :
    m_cells(Cells(this, belongGrid)),
	m_grid_size(gridSize)
    : DNNHS(data, query, alpha) {
}


int HS::DNNHS::Grid::run() {
	
	std::cout << "Index: points" << std::endl;
	for (auto&& c : m_cells.all()) {
		HS::printVector(c.index());
		std::cout << ": ";
		HS::printVector(c.pts());
		std::cout << std::endl;
	}

	return 0;
}
