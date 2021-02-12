#include "grid.hpp"


HS::DNNHS::Grid::Cell::Cell() :
    m_ds(nullptr) {    
}


HS::DNNHS::Grid::Cell::Cell(
    DNNHS* ds) :
    m_ds(ds) {
}


HS::DNNHS::Grid::Grid(
    const Eigen::MatrixXd& data, 
	const Eigen::VectorXd& query, 
	const int&             alpha,
    const int&             gridSize) :
    m_grid_size(gridSize) 
    : DNNHS(data, query, alpha) {
    
    /*** m_cellsの初期化 ***/
}
