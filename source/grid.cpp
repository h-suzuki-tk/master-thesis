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
	m_lower_bound_cell_index = std::vector<int>(dims(), 0);
	m_upper_bound_cell_index = std::vector<int>(dims(), gridSize-1);
}


int HS::DNNHS::Grid::run() {

	const double side = 1.0 / gridSize();

	const std::vector<int> query_belong_cell = belongCell(query());

	//std::vector<int> pts = expandCellPts(query_belong_cell, 0);
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


//*** 要リファクタリング ***
std::vector<int> HS::DNNHS::Grid::expandCellPts(
	const std::vector<int>& coreCellIdx,
	const int               stage) {
	assert(stage >= 0);

	std::vector<std::vector<int>> crit_idx(dims());
	std::vector<std::vector<int>> lower_idx(2, std::vector<int>(dims()));
	std::vector<std::vector<int>> upper_idx(2, std::vector<int>(dims()));

	// 初期化
	int l, u;
	for (int i=0; i<dims(); ++i) {
		l = coreCellIdx[i] - stage;
		u = coreCellIdx[i] + stage;

		if ( l >= m_lower_bound_cell_index[i] ) {
			crit_idx[i].emplace_back(l);
			lower_idx[0][i] = l+1;
		} else {
			l = m_lower_bound_cell_index[i];
			lower_idx[0][i] = l;
		}
		if ( u <= m_upper_bound_cell_index[i] ) {
			crit_idx[i].emplace_back(u);
			upper_idx[0][i] = u-1;
		} else {
			u = m_upper_bound_cell_index[i];
			upper_idx[0][i] = u;
		}

		lower_idx[1][i] = l;
		upper_idx[1][i] = u;
	}

	std::vector<int> pts;
	std::vector<int> cell_idx(dims());

	// ==================================================
	//
	// セルの探索
	//
	// ==================================================
	if ( stage == 0 ) {
		HS::insert(pts, cells().pts(coreCellIdx));

	} else {

		if (dims() == 1) {
			// --------------------------------------------------
			//  1 次元の場合
			// --------------------------------------------------
			for (auto&& idx : crit_idx[0]) {
				cell_idx[0] = idx;
				HS::insert(pts, cells().pts(cell_idx));
			}

		} else {
			// --------------------------------------------------
			//  2 次元以上の場合
			// --------------------------------------------------

			// --- 基準次元 = 0 の処理 ---
			if (crit_idx[0].size() != 0) {

				cell_idx.back() = lower_idx[0].back();
				while (cell_idx.back() <= upper_idx[0].back()) {

					while (1) {
						// pts の更新
						for (auto&& idx : crit_idx[0]) {
							cell_idx[0] = idx;
							HS::insert(pts, cells().pts(cell_idx));
						}
						// cell_idx の更新
						int scan_dim = 1;
						while (scan_dim < dims()-1) {
							if (cell_idx[scan_dim] < upper_idx[1][scan_dim]) {
								++cell_idx[scan_dim];
								break;
							} else {
								cell_idx[scan_dim] = lower_idx[1][scan_dim];
							}
							++scan_dim;
						}
						if (scan_dim >= dims() -1) { break; }

					}

					for (int i=1; i<dims()-1; ++i) { cell_idx[i] = lower_idx[1][i]; }
					++cell_idx.back();
				}

			}

			// --- 基準次元 >= 1 の処理 ---
			int crit = 1;
			while (crit < dims()) {

				if (crit_idx[crit].size() == 0) { continue; }

				cell_idx[crit-1] = lower_idx[0][crit-1];
				while (cell_idx[crit-1] <= upper_idx[0][crit-1]) {

					while (1) {
						// pts の更新
						for (auto&& idx: crit_idx[crit]) {
							cell_idx[crit] = idx;
							HS::insert(pts, cells().pts(cell_idx));
						}
						// cell_idx の更新
						int scan_dim = 0;
						while (scan_dim < crit-1) {
							if (cell_idx[scan_dim] < upper_idx[1][scan_dim]) {
								++cell_idx[scan_dim];
								break;
							} else {
								cell_idx[scan_dim] = lower_idx[1][scan_dim];
							}
							++scan_dim;
						}
						if (scan_dim == crit-1) {
							scan_dim = crit + 1;
							while (scan_dim < dims()) {
								if (cell_idx[scan_dim] < upper_idx[1][scan_dim]) {
									++cell_idx[scan_dim];
									break;
								} else {
									cell_idx[scan_dim] = lower_idx[1][scan_dim];
								}
								++scan_dim;
							}
							if (scan_dim >= dims()) { break; }
						}
					}

					for (int i=0;      i<crit-1; ++i) { cell_idx[i] = lower_idx[1][i]; }
					for (int i=crit+1; i<dims(); ++i) { cell_idx[i] = lower_idx[1][i]; }
					++cell_idx[crit-1];
				}

				++crit;
			}

			bool existCorner = true;
			for ( auto&& idx : crit_idx ) {
				if ( idx.empty() ) { 
					existCorner = false;
					break; 
				}
			}
			if ( existCorner ) {
				
				std::vector<std::vector<int>::iterator> cell_idx_ref(dims());
				for ( int i=0; i<dims(); ++i ) {
					cell_idx_ref[i] = crit_idx[i].begin();
				}

				while (1) {

					// pts の更新
					std::vector<int> idx(dims());
					std::transform( cell_idx_ref.begin(), cell_idx_ref.end(),
						idx.begin(), [](std::vector<int>::iterator itr) { return *itr; } 
					);
					HS::insert(pts, cells().pts(idx));

					// cell_idx の更新
					int scan_dim = 0;
					while ( scan_dim < dims() ) {
						if ( cell_idx_ref[scan_dim] != crit_idx[scan_dim].end()-1 ) {
							++cell_idx_ref[scan_dim];
							break;
						} else {
							cell_idx_ref[scan_dim] = crit_idx[scan_dim].begin();
						}
						++scan_dim;
					}
					if ( scan_dim >= dims() ) { break; }

				}
			}

		}
	}

	return pts;
}