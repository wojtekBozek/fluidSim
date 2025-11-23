#pragma once
#include "baseGrid.hpp"


class Grid2D : public BaseGrid
{
public:
	void run() override;
private:
	uint32_t sizeX, sizeY;

};