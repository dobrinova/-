#pragma once
#include <iostream>

class Cell {
public:
    virtual ~Cell() = default;
    virtual const char* toString() const = 0;
    virtual void print() const = 0;
    virtual size_t getSize() const = 0;
    virtual Cell* clone() const = 0;
    virtual int getNumericValue() const = 0;
};
