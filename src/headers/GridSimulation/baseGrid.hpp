#pragma once

class BaseGrid
{
public:
	virtual void run() = 0;
	virtual void setup() = 0;
	virtual ~BaseGrid() = default;
};
