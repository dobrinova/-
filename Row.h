#pragma once
#include "Alignment.h"
#include "Constants.h"
#include "Cell.h"
#include "CellFactory.h"
#include <fstream>
#include <sstream>
#include <string>

class Row {
	//friend class Table;    

    Cell** cells;
    size_t columnCount;

	void copyFrom(const Row& other);
    void free();
public:
    Row();
    Row(size_t cols);
    ~Row();
    Row(const Row& other);
    Row& operator=(const Row& other);

    void reinit(size_t cols);

    void parseRowFromFile(const char* buff);
    void saveRowToFile(std::ofstream& ofs) const;
    void printRow(size_t* width, Alignment align) const;
    void printRow(size_t width, Alignment align) const;

    void setCellAtInd(size_t index, const char* value);
    void setCellAtInd(size_t index, const std::string& value);
    void setCellAtInd(size_t index, int value);
    void setCellAtInd(size_t index, bool value);
    void setCellAtInd(size_t index, Cell* newCell);


    size_t getCellAtIndSize(size_t ind) const;
    Cell* getCellAtInd(size_t index) const;
};