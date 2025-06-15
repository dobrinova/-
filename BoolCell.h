#pragma once
#include "Cell.h"

class BoolCell : public Cell {
    bool value;
public:
	BoolCell() : value(false) {}

    BoolCell(bool val) : value(val) {}

    const char* toString() const override 
    {
        return value ? "TRUE" : "FALSE";
    }

    void print() const override 
    {
        std::cout << (value ? "TRUE" : "FALSE");
    }

    size_t getSize() const override 
    {
        return strlen(toString());
    }

    Cell* clone() const override 
    {
        return new BoolCell(value);
    }

    int getNumericValue() const override 
    {
        return value ? 1 : 0;
    }

};