#pragma once
#include "baseGrid.hpp"
#include "shaderProgram.hpp"

class Grid2D : public BaseGrid
{
public:
	void run() override;
	void setDimensions(uint32_t x, uint32_t y);
	void setCellSize(double x, double y);
	void initilizeGrid();
private:
	uint32_t nx, ny;
	double cellSizeX, cellSizeY;
	//std::vector<double> grid, vx, vy;
	ShaderProgram computeShader;
};