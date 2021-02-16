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
	m_root(Node()) {
}


HS::DNNHS::Grid::Cells::Cells(
	Grid*                                gds,
	const std::vector<std::vector<int>>& belongGrid) :
	m_gds (gds),
	m_root(Node()) {

	int va = 0;

	// セルの生成
	std::queue<Node*> que;
	que.push(&m_root);
	while (!que.empty()) {
		
		Node* n = que.front();
		
		std::cout << va++ << std::endl;
		std::cout << n->entry().size() << std::endl;

		if (n->entry().size() < m_gds->m_data.dims()-1) {
			for (int i=0; i<m_gds->m_grid_size; i++) {
				n->next().emplace_back(Node(n->entry(), i));
				que.push(&(n->next(i)));
			}
		} else if (n->entry().size() == m_gds->m_data.dims()-1) {
			for (int i=0; i<m_gds->m_grid_size; i++) {
				n->cell().emplace_back(Cell(n->entry(), i));
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

	Node& n = m_root;
	for (auto itr = index.begin(); itr != index.end()-1; ++itr) {
		n = n.next(*itr);
	}
	Cell& c = n.cell(index.back());

	return c.pts();
}


std::vector<HS::DNNHS::Grid::Cell*> HS::DNNHS::Grid::Cells::all() {

	std::vector<Cell*> cells;

	std::queue<Node*> que;
	que.push(&m_root);
	while (!que.empty()) {
		
		Node* n = que.front();
		que.pop();

		if (n->entry().size() < m_gds->m_data.dims()-1) {
			for (int i=0; i<m_gds->m_grid_size; i++) {
				que.push(&(n->next(i)));
			}
		} else if (n->entry().size() == m_gds->m_data.dims()-1) {
			for (int i=0; i<m_gds->m_grid_size; i++) {
				cells.emplace_back(&(n->cell(i)));
			}
		} else { assert(true); }

	}
	
	return cells;
}


HS::DNNHS::Grid::Cells::Node::Node() {
}


HS::DNNHS::Grid::Cells::Node::Node(
	const std::vector<int>& parent_entry,
	const int&              index) {

	/***/
	entry() = parent_entry;
	entry().emplace_back(index);
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
