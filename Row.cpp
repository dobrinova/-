#include "Row.h"
#include "StringCell.h"
#include "IntCell.h"
#include "BoolCell.h"
#include "Table.h"

void Row::copyFrom(const Row& other)
{
    columnCount = other.columnCount;
    cells = new Cell*[columnCount]{};
    for (size_t i = 0; i < columnCount; i++) 
    {
        if (other.cells[i])
            cells[i] = other.cells[i]->clone();
    }
}

void Row::free()
{
    for (size_t i = 0; i < columnCount; i++)
        delete cells[i];
    delete[] cells;
}

void Row::reinit(size_t cols) 
{
    free();
    columnCount = cols;
    cells = new Cell * [columnCount];
    for (size_t i = 0; i < columnCount; i++)
        cells[i] = new StringCell("");
}

Row::Row() : Row(MAX_COLS) {}

Row::Row(size_t cols) : columnCount(cols) 
{
    cells = new Cell*[columnCount];
    for (size_t i = 0; i < columnCount; i++) 
        cells[i] = new StringCell("");
}

Row::~Row() 
{
    for (size_t i = 0; i < columnCount; i++) 
        delete cells[i];
    delete[] cells;
}

Row::Row(const Row& other) 
{
	copyFrom(other);
}

Row& Row::operator=(const Row& other) 
{
    if (this != &other) 
    {
        free();
		copyFrom(other);
    }
    return *this;
}

void Row::parseRowFromFile(const char* buff) 
{
    std::stringstream ss(buff);
    size_t i = 1;
    char content[MAX_SYMBOLS];
       
    while (ss.getline(content, MAX_SYMBOLS, COMMA)) 
    {
        if (i < columnCount) 
        {
            delete cells[i];
            cells[i++] = createCellFromText(content);
        }
    }
}

void Row::saveRowToFile(std::ofstream& ofs) const 
{
    for (size_t i = 1; i < columnCount; i++) 
    {
        if (i > 1) 
            ofs << COMMA;
        ofs << cells[i]->toString();
    }
    ofs << '\n';
}

void Row::printRow(size_t* width, Alignment align) const 
{
	std::cout << PEG << SPACE;          // for the first col
    cells[0]->print();
    if (cells[0]->getSize() == 0) 
        std::cout << SPACE;
    std::cout << SPACE << PEG;

    for (int i = 1; i < columnCount; i++) 
    {
        size_t padding = 0, left = 0, right = 0;;
        if (width[i] > cells[i]->getSize())
			padding = width[i] - cells[i]->getSize();  
        
        switch (align) 
        {
        case Alignment::LEFT:
            left = 0; 
            right = padding;
            break;
        case Alignment::RIGHT:
            left = padding; 
            right = 0;
            break;
        case Alignment::CENTER:
        default:
            left = (padding + 1)/2; 
            right = padding/2;
            break;
        }

        for (size_t i = 0; i < left; i++)
            std::cout << SPACE;

        cells[i]->print();

        for (size_t i = 0; i < right; i++)
            std::cout << SPACE;

        std::cout << PEG;
    }
    std::cout << std::endl;
}

void Row::printRow(size_t width, Alignment align) const 
{
    std::cout << PEG << SPACE;
    cells[0]->print();
    if (cells[0]->getSize() == 0) 
        std::cout << SPACE;
    std::cout << SPACE << PEG;

    for (int i = 1; i < columnCount; i++)
    {
        const char* val = cells[i]->toString();
        size_t valLen = strlen(val);

        size_t contentLen = std::min(valLen, width);
        size_t padding = width > contentLen ? width - contentLen : 0;

        size_t left = 0, right = 0;

        switch (align) {
        case Alignment::LEFT:
            left = 0;
            right = padding;
            break;
        case Alignment::RIGHT:
            left = padding;
            right = 0;
            break;
        case Alignment::CENTER:
        default:
            left = (padding + 1) / 2;
            right = padding / 2;
            break;
        }

        for (size_t j = 0; j < left; j++) 
            std::cout << SPACE;

        for (size_t j = 0; j < contentLen; j++)
            std::cout << val[j];

        for (size_t j = 0; j < right; j++) 
            std::cout << SPACE;

        std::cout << PEG;
    }
    std::cout << std::endl;
}

void Row::setCellAtInd(size_t index, const char* value) 
{
    if (index >= columnCount) 
    {
        std::cout << "Index out of range!\n";
        return;
    }
    if (!value) 
    {
        std::cout << "Missing value!\n";
        return;
	}
    delete cells[index];
    cells[index] = new StringCell(value);
}

void Row::setCellAtInd(size_t index, const std::string& value) 
{
    setCellAtInd(index, value.c_str());
}

void Row::setCellAtInd(size_t index, int value) 
{
    if (index >= columnCount)
    {
        std::cout << "Index out of range!\n";
        return;
    }
    delete cells[index];
    cells[index] = new IntCell(value);
}

void Row::setCellAtInd(size_t index, bool value) 
{
    if (index >= columnCount)
    {
        std::cout << "Index out of range!\n";
        return;
    }
    delete cells[index];
    cells[index] = new BoolCell(value);
}

void Row::setCellAtInd(size_t index, Cell* newCell)
{
    if (index >= columnCount) 
        return;
    delete cells[index];
    cells[index] = newCell;
}

size_t Row::getCellAtIndSize(size_t index) const 
{
    if (index >= columnCount)
    {
        std::cout << "Index out of range!\n";
        return 0;
    }

    if (!cells[index]) 
        return 0;

    return cells[index]->getSize();
}

Cell* Row::getCellAtInd(size_t index) const 
{
    if (index >= columnCount)
        return nullptr;
    return cells[index];
}