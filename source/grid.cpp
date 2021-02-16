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


HS::DNNHS::Grid::Cells::Cells() :
	m_gds(nullptr),
	m_root(nullptr) {
}


HS::DNNHS::Grid::Cells::Cells(
	Grid*                                gds,
	const std::vector<std::vector<int>>& belongGrid) :
	m_gds (gds),
	m_root(new Node()) {

	// セルの生成
	std::queue<Node*> que;
	que.push(m_root);
	while (!que.empty()) {
		
		Node* nd = que.front();

		if (nd->depth() < m_gds->dims()-1) {
			for (int i=0; i<m_gds->gridSize(); ++i) {
				nd->children().emplace_back(
					HS::DNNHS::Grid::Cells::Node::Child(new Node(nd->entry(), i))
				);
				que.push(nd->child(i).node);
			}

		} else if (nd->depth() == m_gds->dims()-1) {
			for (int i=0; i<m_gds->gridSize(); ++i) {
				nd->children().emplace_back(
					HS::DNNHS::Grid::Cells::Node::Child(new Cell(nd->entry(), i))
				);
			}

		} else { assert(true); }

		que.pop();
	}

	// データの所属情報をセルに格納
	for (int i=0; i<m_gds->m_data.size(); i++) {
		pts(belongGrid[i]).emplace_back(i);
	}

}


std::vector<int>& HS::DNNHS::Grid::Cells::pts(
	const std::vector<int>& index) {
	assert(index.size() == m_gds->m_data.dims());

	Node* nd = m_root;
	for (auto itr = index.begin(); itr != index.end()-1; ++itr) {
		nd = nd->child(*itr).node;
	}
	Cell* c = nd->child(index.back()).cell;

	return c->pts();
}


std::vector<HS::DNNHS::Grid::Cell*> HS::DNNHS::Grid::Cells::all() {

	std::vector<Cell*> cells;

	std::queue<Node*> que;
	que.push(m_root);
	while (!que.empty()) {
		
		Node* nd = que.front();

		if (nd->entry().size() < m_gds->m_data.dims()-1) {
			for (int i=0; i<m_gds->m_grid_size; i++) {
				que.push(nd->child(i).node);
			}
		} else if (nd->entry().size() == m_gds->m_data.dims()-1) {
			for (int i=0; i<m_gds->m_grid_size; i++) {
				cells.emplace_back(nd->child(i).cell);
			}
		} else { assert(true); }

		que.pop();
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


HS::DNNHS::Grid::Cells::Node::~Node() {
	if (!m_children.empty()) {
		for (auto child : m_children) {
			if (child.node != nullptr) { delete child.node; }
			else if (child.cell != nullptr) { delete child.cell; }
			else { assert(true); }
		}
	}
}


HS::DNNHS::Grid::Grid(
    const Eigen::MatrixXd&               data, 
	const Eigen::VectorXd&               query, 
	const int&                           alpha,
    const int&                           gridSize,
	const std::vector<std::vector<int>>& belongGrid) : 
	DNNHS(data, query, alpha),
	m_grid_size(gridSize) {

	m_cells = Cells(this, belongGrid);
}


int HS::DNNHS::Grid::run() {
	
	std::cout << "Index: points" << std::endl;
	for (auto* c : m_cells.all()) {
		HS::printVector(c->index());
		std::cout << ": ";
		HS::printVector(c->pts());
		std::cout << std::endl;
	}

	return 0;
}
