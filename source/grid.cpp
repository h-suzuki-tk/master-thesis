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
	const std::vector<std::vector<int>>& belongCell) :
	m_gds (gds),
	m_root(new Node( gds->gridSize() )),
	m_side(1.0/gds->gridSize()) {

	// Make cells
	for ( int id=0; id < m_gds->data().size(); ++id ) {
		add( m_gds->belongCell( m_gds->data(id) ) ).pts().emplace_back(id);
	}

}


HS::DNNHS::Grid::Cells::~Cells() {
	delete m_root;
}


HS::DNNHS::Grid::Cell* HS::DNNHS::Grid::Cells::operator[](
	const std::vector<int>& idx ) {

	assert(idx.size() == m_gds->dims());

	Node* nd = m_root;
	for (auto itr = idx.begin(); itr != idx.end()-1; ++itr) {
		if ( nd->child(*itr).isNull() ) return nullptr;
		nd = nd->child(*itr).node;
	}

	return nd->child(idx.back()).cell;

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


HS::DNNHS::Grid::Cell& HS::DNNHS::Grid::Cells::add( 
	const std::vector<int>& idx ) {
	/*
	 * Params:
	 *     idx: Index of the cell you want to add.
	 * Return:
	 *     Reference of the added Cell object.
	 *     If it already exists, this function returns its reference. 
	 */
	
	assert( idx.size() == m_gds->dims() );

	Node* nd = m_root;
	for ( auto itr = idx.begin(); itr != idx.end()-1; ++itr ) {

		if ( nd->child(*itr).isNull() ) {
			nd->child(*itr) = HS::DNNHS::Grid::Cells::Node::Child( new Node( m_gds->gridSize(), nd->entry(), *itr ) );
		}
		nd = nd->child(*itr).node;

	}
	if ( nd->child( idx.back() ).isNull() ) {
		nd->child( idx.back() ) = HS::DNNHS::Grid::Cells::Node::Child( new Cell( nd->entry(), idx.back() ) );
	}

	return *( nd->child( idx.back() ).cell );
}


void HS::DNNHS::Grid::Cells::remove(
	const std::vector<int>& idx ) {

	assert(idx.size() == m_gds->dims());

	Node* nd = m_root;
	for (auto itr = idx.begin(); itr != idx.end()-1; ++itr) {
		nd = nd->child(*itr).node;
	}
	Cell* c = nd->child(idx.back()).cell;
	
	delete c;
	nd->child(idx.back()).cell = nullptr;

}


HS::DNNHS::Grid::Cells::Node::Node(
	const int grid_size ) :
	m_children( std::vector<Child>( grid_size ) ) {
}


HS::DNNHS::Grid::Cells::Node::Node(
	const int               grid_size,
	const std::vector<int>& parent_entry,
	const int&              index) :
	m_children( std::vector<Child>( grid_size ) ) {

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


HS::DNNHS::Grid::ExpansionCells::ExpansionCells(
	Grid*                  gds,
	const Eigen::VectorXd& core_pt ) :
	m_gds( gds ),
	m_core_pt( core_pt ),
	m_core_cell( gds->belongCell( core_pt ) ),
	m_stage( DEFAULT_STAGE ),
	m_gtd_nn_range( 
		gds->cellSide() * ( 0.5 - ( 0.5 - 
		( core_pt / gds->cellSide() ).unaryExpr( []( const double a ) { 
			return std::modf( a, nullptr ); 
		} ).array() ).abs().maxCoeff() )
	),
	m_cells(),
	m_buf_cells() {
}


void HS::DNNHS::Grid::ExpansionCells::expand() {

	++m_stage;
	m_gtd_nn_range += m_gds->cellSide();
	m_cells         = expansionCells( 
		m_core_cell,
		m_stage, 
		m_gds->m_lower_bound_cell_index, 
		m_gds->m_upper_bound_cell_index 
	);

}


std::vector<int> HS::DNNHS::Grid::ExpansionCells::pts() {

	std::vector<int> pts;

	for ( auto* cell : m_cells ) {
		HS::insert( pts, cell->pts() );
	}

	return pts;
}


std::vector<HS::DNNHS::Grid::Cell*> HS::DNNHS::Grid::ExpansionCells::expansionCells( 
	const std::vector<int>& core_cell, 
	const int               stage, 
	const std::vector<int>& lower_bound_cell_idx, 
	const std::vector<int>& upper_bound_cell_idx ) {

	auto addCell = [this]( std::vector<Cell*>& cells, const std::vector<int>& cell_idx ) {

		if ( m_buf_cells[ cell_idx ] != nullptr &&
			!m_buf_cells[ cell_idx ]->pts().empty() ) {

			cells.emplace_back( m_buf_cells[ cell_idx ] );
		
		} else if ( m_gds->cell( cell_idx ) != nullptr ) {

			cells.emplace_back(  m_gds->cell( cell_idx ) );
		
		} else { assert(false); }

	};

	assert(stage >= 0);

	std::vector<std::vector<int>> crit_idx(m_gds->dims());
	std::vector<std::vector<int>> lower_idx(2, std::vector<int>(m_gds->dims()));
	std::vector<std::vector<int>> upper_idx(2, std::vector<int>(m_gds->dims()));

	// 初期化
	int l, u;
	for (int i=0; i<m_gds->dims(); ++i) {
		l = core_cell[i] - stage;
		u = core_cell[i] + stage;

		if ( l >= lower_bound_cell_idx[i] ) {
			crit_idx[i].emplace_back(l);
			lower_idx[0][i] = l+1;
		} else {
			l = lower_bound_cell_idx[i];
			lower_idx[0][i] = l;
		}
		if ( u <= upper_bound_cell_idx[i] ) {
			crit_idx[i].emplace_back(u);
			upper_idx[0][i] = u-1;
		} else {
			u = upper_bound_cell_idx[i];
			upper_idx[0][i] = u;
		}

		lower_idx[1][i] = l;
		upper_idx[1][i] = u;
	}

	std::vector<Cell*> cells;
	std::vector<int>   cell_idx(m_gds->dims());

	// ==================================================
	//
	// セルの探索
	//
	// ==================================================
	if ( stage == 0 ) {
		addCell( cells, core_cell );

	} else {

		if (m_gds->dims() == 1) {
			// --------------------------------------------------
			//  1 次元の場合
			// --------------------------------------------------
			for (auto&& idx : crit_idx[0]) {
				cell_idx[0] = idx;
				addCell( cells, cell_idx );
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
							addCell( cells, cell_idx );
						}
						// cell_idx の更新
						int scan_dim = 1;
						while (scan_dim < m_gds->dims()-1) {
							if (cell_idx[scan_dim] < upper_idx[1][scan_dim]) {
								++cell_idx[scan_dim];
								break;
							} else {
								cell_idx[scan_dim] = lower_idx[1][scan_dim];
							}
							++scan_dim;
						}
						if (scan_dim >= m_gds->dims() -1) { break; }

					}

					for (int i=1; i<m_gds->dims()-1; ++i) { cell_idx[i] = lower_idx[1][i]; }
					++cell_idx.back();
				}

			}

			// --- 基準次元 >= 1 の処理 ---
			int crit = 1;
			while (crit < m_gds->dims()) {

				if (crit_idx[crit].size() == 0) { continue; }

				cell_idx[crit-1] = lower_idx[0][crit-1];
				while (cell_idx[crit-1] <= upper_idx[0][crit-1]) {

					while (1) {
						// pts の更新
						for (auto&& idx: crit_idx[crit]) {
							cell_idx[crit] = idx;
							addCell( cells, cell_idx );
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
							while (scan_dim < m_gds->dims()) {
								if (cell_idx[scan_dim] < upper_idx[1][scan_dim]) {
									++cell_idx[scan_dim];
									break;
								} else {
									cell_idx[scan_dim] = lower_idx[1][scan_dim];
								}
								++scan_dim;
							}
							if (scan_dim >= m_gds->dims()) { break; }
						}
					}

					for (int i=0;      i<crit-1; ++i) { cell_idx[i] = lower_idx[1][i]; }
					for (int i=crit+1; i<m_gds->dims(); ++i) { cell_idx[i] = lower_idx[1][i]; }
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
				
				std::vector<std::vector<int>::iterator> cell_idx_ref(m_gds->dims());
				for ( int i=0; i<m_gds->dims(); ++i ) {
					cell_idx_ref[i] = crit_idx[i].begin();
				}

				while (1) {

					// pts の更新
					std::vector<int> idx(m_gds->dims());
					std::transform( cell_idx_ref.begin(), cell_idx_ref.end(),
						idx.begin(), [](std::vector<int>::iterator itr) { return *itr; } 
					);
					addCell( cells, cell_idx );

					// cell_idx の更新
					int scan_dim = 0;
					while ( scan_dim < m_gds->dims() ) {
						if ( cell_idx_ref[scan_dim] != crit_idx[scan_dim].end()-1 ) {
							++cell_idx_ref[scan_dim];
							break;
						} else {
							cell_idx_ref[scan_dim] = crit_idx[scan_dim].begin();
						}
						++scan_dim;
					}
					if ( scan_dim >= m_gds->dims() ) { break; }

				}
			}

		}
	}

	return cells;

}



HS::DNNHS::Grid::Grid(
    const Eigen::MatrixXd&               data, 
	const Eigen::VectorXd&               query, 
	const int&                           alpha,
    const int&                           gridSize,
	const std::vector<std::vector<int>>& belongCell) : 
	DNNHS(data, query, alpha),
	m_grid_size(gridSize),
	m_cell_side(1.0/gridSize) {

	m_cells = Cells(this, belongCell);
	m_lower_bound_cell_index = std::vector<int>(dims(), 0);
	m_upper_bound_cell_index = std::vector<int>(dims(), gridSize-1);
}


int HS::DNNHS::Grid::run() {

	ExpansionCells   epcells( this, query() );
	std::vector<int> pts;
	
	while ( !epcells.cells().empty() ) {

		// pts のセット
		HS::insert( pts, epcells.pts() );
		filterPts( &pts );

		// pts をクエリに近い順にソート
		std::sort(
			pts.begin(), pts.end(), [&]( const int a, const int b ) {
			return fromQueryDist( a ) < fromQueryDist( b );
		} );

		// 各点について処理
		for (auto itr = pts.begin(); 
			itr != pts.end() && pts.size() > MIN_CLUSTER_SIZE; ++itr) {

			// NN か判定
			if ( fromQueryDist( *itr ) > epcells.gtdNNRange() ) { break; }
			
			// 所属グループを検索
			m_groups.emplace_back( findGroup( *itr ) ); /** TODO: findGroup **/

			// 総合近似度を計算・フィルタリング
			if ( m_groups.back().delta() < m_result.delta() ) {
				
				updateBound( m_groups.back() );
				updateBoundCellIdx( m_bound );
				m_result = m_groups.back();

				filterPts( &pts );
			}			

		}

		// 探索セルの拡大
		epcells.expand(); /** TODO: ExpansionCells::expand **/

	}

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


HS::DNNHS::Group HS::DNNHS::Grid::findGroup(
	const int core_pt ) {
	
	std::cout << "findGroup" << std::endl;

	/** TODO: **/

	return Group();
}


void HS::DNNHS::Grid::updateBoundCellIdx(
	const double bound ) {

	assert( bound >= 0.0 );

	m_lower_bound_cell_index = belongCell( query().array() - bound );
	m_upper_bound_cell_index = belongCell( query().array() + bound );
}
