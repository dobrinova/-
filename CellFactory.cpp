#include "CellFactory.h"
#include "BoolCell.h"
#include "IntCell.h"
#include "StringCell.h"
#include <cstring>
#include <cstdlib>

Cell* createCellFromText(const char* text) 
{
    if (strcmp(text, "true") == 0)
        return new BoolCell(true);
    if (strcmp(text, "false") == 0)
        return new BoolCell(false);

    char* endptr = nullptr;
    long num = strtol(text, &endptr, 10);
    if (endptr != text && *endptr == '\0')
        return new IntCell(static_cast<int>(num));

    return new StringCell(text);
}
