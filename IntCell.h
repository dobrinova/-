#pragma once
#include "Cell.h"

#pragma warning (disable: 4996)

class IntCell : public Cell {
    int value;
    mutable char buffer[20];
public:
    IntCell() : value(0) {}

    IntCell(int val) : value(val) {}

    const char* toString() const override 
    {
		sprintf(buffer, "%d", value);           // found this on the internet
        return buffer;
    }

    void print() const override 
    {
        std::cout << value;
    }

    size_t getSize() const override 
    {
        return strlen(toString());
    }

    Cell* clone() const override 
    {
        return new IntCell(value);
    }

    int getNumericValue() const override 
    {
        return value;
    }

};
