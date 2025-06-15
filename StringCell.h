#pragma once
#include "Cell.h"
#include "Constants.h"

#pragma warning (disable: 4996)

class StringCell : public Cell {
    char content[MAX_SYMBOLS];
public:
	StringCell() : content("") {}

    StringCell(const char* str) 
    {
        if (!str || strlen(str) >= MAX_SYMBOLS)
        {
            std::cout << "Invalid size of the string! Content will be set to nothing!\n";
            content[0] = '\0';
        }
        else
        {
            strcpy(content, str);
        }
    }

    const char* toString() const override 
    {
        return content;
    }

    void print() const override 
    {
        std::cout << content;
    }

    size_t getSize() const override 
    {
        return strlen(content);
    }

    Cell* clone() const override 
    {
        return new StringCell(content);
    }

    int getNumericValue() const override 
    {
        return 0;
    }

};
