#pragma once
#include "Cell.h"
#include "Row.h"

class ReferenceCell : public Cell {
    const Row* refRow;
    size_t refCol;

public:
    ReferenceCell(const Row* row, size_t col) : refRow(row), refCol(col) {}

    void print() const override
    {
        if (refRow)
            refRow->getCellAtInd(refCol)->print();
        else
            std::cout << "0";
    }

    const char* toString() const override
    {
        if (refRow)
            return refRow->getCellAtInd(refCol)->toString();
        return "0";
    }

    size_t getSize() const override
    {
        if (refRow)
            return refRow->getCellAtInd(refCol)->getSize();
        return 1;
    }

    Cell* clone() const override
    {
        return new ReferenceCell(refRow, refCol);
    }

    int getNumericValue() const override {
        if (!refRow->getCellAtInd(refCol)) return 0;
        return refRow->getCellAtInd(refCol)->getNumericValue();
    }

};
