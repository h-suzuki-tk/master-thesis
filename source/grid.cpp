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
	m_root(nullptr),
	m_side(-__DBL_MAX__) {
}


HS::DNNHS::Grid::Cells::Cells(
	Grid*                                gds,
	const std::vector<std::vector<int>>& belongGrid) :
	m_gds (gds),
	m_root(new Node()),
	m_side(1.0/gds->gridSize()) {

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


/*
HS::DNNHS::Grid::Cell& HS::DNNHS::Grid::Cells::operator()(
	const std::vector<int>& index) {

	assert(index.size() == m_gds->dims());
	Node* nd = m_root;
	for (auto itr = index.begin(); itr != index.end()-1; ++itr) {
		nd = nd->child(*itr).node;
	}
	Cell* cl = nd->child(index.back()).cell;

	return *cl;
}
*/


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
	
	std::vector<int> min_bound_idx(dims(), -__INT_MAX__);
	std::vector<int> max_bound_idx(dims(), gridSize());

	const double side = 1.0 / gridSize();

	const std::vector<int> query_belong_cell = belongCell(query());

	/*** std::vector<int> pts = expandCellPts(query_belong_cell, 0); ***/
	/*
	while (!pts.empty()) {

		// pts をクエリに近い順にソート
		// ***

		// 各点について処理
		for (auto itr = pts.begin(); itr != pts.end(); ++itr) {

			// NN であるか確認
			//*** if () { break; }
			
			// 所属グループを検索
			groups().emplace_back(findGroup(
				std::distance(pts.begin(), itr)
			));

			// 総合近似度を計算・フィルタリング
			if (groups().back().delta() < result().delta()) {
				
				//*** boundIdx も更新 ***
				updateBound(groups().back());
				result() = groups().back();

				filterPts(&pts);
			}			

		}

		// 探索セルの拡大・pts の更新
		//***

	}
	*/

	return 0;
}


std::vector<int> HS::DNNHS::Grid::belongCell(
	const Eigen::VectorXd& pt) {
	
	Eigen::VectorXi index_eigen = (pt / cells().side()).array().floor().cast<int>();
	return std::vector<int>(
		index_eigen.data(), 
		index_eigen.data() + index_eigen.size()
	);

}


std::vector<int> HS::DNNHS::Grid::expandCellPts(
	const std::vector<int>& coreCellIdx,
	const int               stage) {
	assert(stage >= 0);
	
	std::vector<int> pts;

	std::tuple<std::pair<int, int>, int, std::vector<int>> offset = std::make_tuple(
		std::make_pair(-stage, stage),
		-(stage-1),
		std::vector<int>(dims()-2, -stage)
	);
	//*** offset_list ***

	// 初期化
	std::vector<int> cell_idx = coreCellIdx;
	cell_idx[0] =  coreCellIdx[0] + std::get<0>(offset).first; 
	cell_idx[1] += std::get<1>(offset);
	//***　[2] 以降 ***

	for (int i=0; i<dims(); ++i) {

		// 参照の張替え
		//***

		while (std::get<1>(offset) <= stage-1) {

			// cell_idx の更新
			//***
			
			while (std::get<2>(offset) != std::vector<int>(dims()-2, stage)) { //*** !offset.isFilledWith(stage) ***

				// cell_idx の更新
				//***

				// pts の更新
				//***

				// offset の更新
				//*** HS::inc(offset, -stage, stage); ***
			}

			++itr.second;
		}

		++itr.first;
	}

	return pts;
}