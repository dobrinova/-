#include "Table.h"
#include "Constants.h"
#include "ReferenceCell.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cassert>      // for assert
#include <cstdlib>
#include "IntCell.h"

Table::Table(const char* config_file) 
{
    assert(loadConfig(config_file) && initializeEmptyTable());
}

Table::Table(const char* config_file, const char* table_file) 
{
    assert(loadConfig(config_file) && initializeTableFromFile(table_file));
}

void Table::displayLine() const 
{
    std::cout << PEG << DASH << DASH << DASH << PEG;
    for (size_t i = 1; i < initialTableCols; i++) 
    {
        for (size_t j = 0; j < visibleCellSymbols; j++) 
            std::cout << DASH;
        std::cout << PEG;
    }
    std::cout << std::endl;
}

void Table::displayLine(const size_t* maxLenPerCol) const 
{
    std::cout << PEG << DASH << DASH << DASH << PEG;
    for (size_t i = 1; i < initialTableCols; i++) 
    {
        for (size_t j = 0; j < maxLenPerCol[i]; j++) 
            std::cout << DASH;
        std::cout << PEG;
    }
    std::cout << std::endl;
}

void Table::display() const 
{
    if (!autoFit) 
    {
        displayLine();
        for (size_t i = 0; i < initialTableRows; i++) 
        {
            rows[i].printRow(visibleCellSymbols, initialAlignment);
            displayLine();
        }
    }
    else 
    {
        size_t maxLenPerCol[MAX_COLS] = { 0 };
        for (size_t row = 0; row < initialTableRows; row++) 
            for (size_t col = 0; col < initialTableCols; col++) 
                maxLenPerCol[col] = std::max(maxLenPerCol[col], std::max(rows[row].getCellAtIndSize(col), size_t(3)));
            
        displayLine(maxLenPerCol);
        for (size_t i = 0; i < initialTableRows; i++) 
        {
            rows[i].printRow(maxLenPerCol, initialAlignment);
            displayLine(maxLenPerCol);
        }
    }
}

void Table::doInsert(const char* cellName, std::stringstream& ss) 
{
    int row = cellName[0] - 'A' + 1;
    int col = atoi(cellName + 1);
    if (row <= 0 || col <= 0 || row >= initialTableRows || col >= initialTableCols)
    {
        std::cout << "Index out of range!\n";
        return;
    }

    char valueStr[BUFFER_SIZE];
    ss.get();
    ss.getline(valueStr, BUFFER_SIZE);

    if (valueStr[0] == QUOT) 
    {
        char* end = strrchr(valueStr + 1, QUOT);         // found it on the internet
        if (!end)
        {
            setErrorValue(cellName);
            return;
        }
        
        *end = NUL;
        rows[row].setCellAtInd(col, valueStr + 1);
    }
    else if (strcmp(valueStr, "TRUE") == 0 || strcmp(valueStr, "FALSE") == 0) 
    {
        bool val = strcmp(valueStr, "TRUE") == 0;
        rows[row].setCellAtInd(col, val);
    }
    else {                                     
        char* endptr = nullptr;
        long num = strtol(valueStr, &endptr, 10);
        if (endptr != valueStr && *endptr == NUL) 
        {
            int val = atoi(valueStr);
            rows[row].setCellAtInd(col, val);
        }
        else
            setErrorValue(cellName);
    }
}

void Table::doDelete(const char* cellName) 
{
    int row = cellName[0] - 'A' + 1;
    int col = atoi(cellName + 1);
    if (row <= 0 || col <= 0 || row >= initialTableRows || col >= initialTableCols) 
    {
        std::cout << "Index out of range!\n";
        return;
    }

    rows[row].setCellAtInd(col, "");
}

void Table::doReference(const char* targetCell, const char* refCell) 
{
    int tRow = targetCell[0] - 'A' + 1;
    int tCol = atoi(targetCell + 1);
    int rRow = refCell[0] - 'A' + 1;
    int rCol = atoi(refCell + 1);

    if (tRow <= 0 || tCol <= 0 || rRow <= 0 || rCol <= 0 ||
        tRow >= initialTableRows || tCol >= initialTableCols ||
        rRow >= initialTableRows || rCol >= initialTableCols)
    {
        std::cout << "Index out of range!\n";
        return;
    }

    rows[tRow].setCellAtInd(tCol, new ReferenceCell(&rows[rRow], rCol));
}

std::vector<int> Table::getNumericValues(std::stringstream& ss)
{
	std::vector<int> values;

    char buff[BUFFER_SIZE];
    ss.getline(buff, sizeof(buff), OP_BRC);

    std::stringstream ss2;
    ss2 << buff;

    int argCount = countCommas(ss2) + 1;

    while (argCount--)
    {
        char parameter[BUFFER_SIZE];
        ss2.getline(parameter, sizeof(parameter), SPACE);  

        char from[POS_SIZE], to[POS_SIZE];
        bool isRange = 0;
        size_t i = 0, j = 0;

        for (; parameter[i] != COMMA && parameter[i] != CL_BRC; i++)
        {
            if (parameter[i] == DELIMITER)
            {
                from[i++] = NUL;
                isRange = true;
                break;
            }
            from[i] = parameter[i];
        }

        if (isRange)
        {
            for (; parameter[i] != COMMA && parameter[i] != CL_BRC; j++, i++)
                to[j] = parameter[i];
            to[j] = NUL;

            int r_from = from[0] - 'A' + 1;
            int c_from = atoi(from + 1);

            int r_to = to[0] - 'A' + 1;
            int c_to = atoi(to + 1);

            if (r_from > r_to) 
                std::swap(r_from, r_to);
            if (c_from > c_to) 
                std::swap(c_from, c_to);

            for (size_t i = r_from; i <= r_to; i++)
                for (size_t j = c_from; j <= c_to; j++)
                    if (rows[i].getCellAtInd(j))
                        values.push_back(rows[i].getCellAtInd(j)->getNumericValue());
        }
        else
        {
            if (getCellFromName(parameter)) 
                values.push_back(getCellFromName(parameter)->getNumericValue());
            else if (strcmp(parameter, "TRUE)") == 0 || strcmp(parameter, "TRUE,") == 0) 
                values.push_back(1);
            else 
                values.push_back(atoi(parameter));
        }
    }

    return values;
}

int Table::countCommas(std::stringstream& ss)
{
    std::string content = ss.str();
    return std::count(content.begin(), content.end(), COMMA);
}

void Table::doSUM(const char* cellName, std::stringstream& ss)
{
    std::vector<int> values = getNumericValues(ss);

    if (values.empty())
        setErrorValue(cellName);
    else 
    {
        int sum = 0;
        for (size_t i = 0; i < values.size(); i++)
            sum += values[i];

        setNumericValue(cellName, sum);
    }
}

void Table::doAVERAGE(const char* cellName, std::stringstream& ss)
{
    std::vector<int> values = getNumericValues(ss);

    if (values.empty())
        setErrorValue(cellName);
    else
    {
        int sum = 0;
        for (size_t i = 0; i < values.size(); i++)
            sum += values[i];

        setNumericValue(cellName, sum / values.size());
    }
}

void Table::doMAX(const char* cellName, std::stringstream& ss)
{
	if (!isRange(ss))
		setErrorValue(cellName);
    else
    {
        std::vector<int> values = getNumericValues(ss);

        if (values.empty())
            setErrorValue(cellName);
        else
        {
            int maxVal = values[0];
            for (size_t i = 0; i < values.size(); i++)
                if (values[i] > maxVal)
                    maxVal = values[i];

            setNumericValue(cellName, maxVal);
        }
    }
}

void Table::doMIN(const char* cellName, std::stringstream& ss)
{
    if (!isRange(ss))
        setErrorValue(cellName);
    else
    {
        std::vector<int> values = getNumericValues(ss);

        if (values.empty())
            setErrorValue(cellName);
        else 
        {
            int minVal = values[0];
            for (size_t i = 0; i < values.size(); i++)
                if (values[i] < minVal)
                    minVal = values[i];

            setNumericValue(cellName, minVal);
        }
    }
}

void Table::doLEN(const char* cellName, std::stringstream& ss)
{
    if (isRange(ss) || countCommas(ss))
        setErrorValue(cellName);
    else
    {
        char parameter[BUFFER_SIZE];
        ss.getline(parameter, sizeof(parameter), OP_BRC);       // to skip this part

        if (getCellFromName(parameter))
            setNumericValue(cellName, getCellFromName(parameter)->getSize());
        else
			setNumericValue(cellName, strlen(parameter) - 1);           // -1 to ignore the closing bracket
    }
}

void Table::doCONCAT(const char* cellName, std::stringstream& ss)
{
    if (countCommas(ss) != 1) 
    {
        setErrorValue(cellName);
        return;
    }

    char args[BUFFER_SIZE];
    ss.getline(args, sizeof(args), OP_BRC);   
    
    std::stringstream ss2;
    ss2 << args;

    ss2.getline(args, sizeof(args), COMMA);
    std::stringstream ss3;
    ss3 << args;
    
    if (!isRange(ss3))
        setErrorValue(cellName);
    else
    {
        char parameter[BUFFER_SIZE];
        ss3.getline(parameter, sizeof(parameter));

        ss2.getline(args, sizeof(args), QUOT);
        if (ss2.fail())
        {
            setErrorValue(cellName);
            return;
        }

        char delimiter = ss2.get();

        char from[POS_SIZE], to[POS_SIZE];

        size_t i = 0, j = 0;
        for (; parameter[i] != NUL; i++)
        {
            if (parameter[i] == DELIMITER)
            {
                from[i++] = NUL;
                break;
            }
            from[i] = parameter[i];
        }

        for (; parameter[i] != NUL; j++, i++)
            to[j] = parameter[i];
        to[j] = NUL;

        int r_from = from[0] - 'A' + 1;
        int c_from = atoi(from + 1);

        int r_to = to[0] - 'A' + 1;
        int c_to = atoi(to + 1);

        if (r_from > r_to)
            std::swap(r_from, r_to);
        if (c_from > c_to)
            std::swap(c_from, c_to);

        std::stringstream content;
        for (size_t r = r_from; r <= r_to; r++)
        {
            for (size_t c = c_from; c <= c_to; c++)
            {
                if (strlen(rows[r].getCellAtInd(c)->toString()))
                    content << rows[r].getCellAtInd(c)->toString() << delimiter;
            }
        }

        content.seekp(-1, std::ios::end);
        content.put(NUL);
        setStringValue(cellName, content);
    }
}

void Table::doSUBSTR(const char* cellName, std::stringstream& ss)
{
    if (countCommas(ss) != 2)
    {
        setErrorValue(cellName);
        return;
    }

    char args[BUFFER_SIZE];
    ss.getline(args, sizeof(args), OP_BRC);   

    std::stringstream ss2;
    ss2 << args;

    ss2.getline(args, sizeof(args), COMMA);
    
    std::stringstream ss3;
    ss3 << args;
	
    if (isRange(ss3))
        setErrorValue(cellName);
    else
    {
        char startStr[POS_SIZE], lenStr[POS_SIZE];
        ss2.getline(startStr, sizeof(startStr), COMMA);
        ss2.getline(lenStr, sizeof(lenStr), COMMA);

        int startIndex = atoi(startStr);
        int length = atoi(lenStr);

        if (startIndex < 0 || length <= 0)
        {
            setErrorValue(cellName);
            return;
        }

        const char* text = nullptr;

        if (getCellFromName(args))
        {
            text = getCellFromName(args)->toString();
        }
        else
        {
            setErrorValue(cellName);
            return;
        }

        size_t fullLen = strlen(text);
        if (startIndex >= fullLen || startIndex + length > fullLen)
        {
            setErrorValue(cellName);
            return;
        }

        char result[BUFFER_SIZE];
        strcpy(result, text + startIndex);
        result[length] = NUL;

        rows[cellName[0] - 'A' + 1].setCellAtInd(atoi(cellName + 1), result);
    }
}

void Table::doCOUNT(const char* cellName, std::stringstream& ss)
{
    if (countCommas(ss) != 0) 
    {
        setErrorValue(cellName);
        return;
    }

    char args[BUFFER_SIZE];
    ss.getline(args, sizeof(args), OP_BRC);

    std::stringstream ss2;
    ss2 << args;

    ss2.getline(args, sizeof(args), COMMA);
    std::stringstream ss3;
    ss3 << args;

    if (!isRange(ss3))
        setErrorValue(cellName);
    else
    {
        char parameter[BUFFER_SIZE];
        ss3.getline(parameter, sizeof(parameter));

        int count = 0;
        char from[POS_SIZE], to[POS_SIZE];

        size_t i = 0, j = 0;
        for (; parameter[i] != NUL; i++)
        {
            if (parameter[i] == DELIMITER)
            {
                from[i++] = NUL;
                break;
            }
            from[i] = parameter[i];
        }

        for (; parameter[i] != NUL; j++, i++)
            to[j] = parameter[i];

        to[j] = NUL;

        int r_from = from[0] - 'A' + 1;
        int c_from = atoi(from + 1);

        int r_to = to[0] - 'A' + 1;
        int c_to = atoi(to + 1);

        if (r_from > r_to)
            std::swap(r_from, r_to);
        if (c_from > c_to)
            std::swap(c_from, c_to);

        for (size_t r = r_from; r <= r_to; r++)
        {
            for (size_t c = c_from; c <= c_to; c++)
                if (rows[r].getCellAtInd(c) && (rows[r].getCellAtInd(c)->toString()[0] != NUL))
                    count++;
        }

		setNumericValue(cellName, count);
    }
}

void Table::executeCommand(const char* command) 
{

    std::stringstream ss(command);
    char cellName[POS_SIZE], action[BUFFER_SIZE];       //valueStr[BUFFER_SIZE];

    ss >> cellName;

    if(!isCllNameValid(cellName))
    {
        std::cout << "Invalid cell name!\n";
        return;
	}

    ss.ignore();                                        // ignore the space 
	char first_char = ss.peek();

    if (first_char == '=')
    {
		ss.ignore();                                    // ignore the '=' character
		ss.getline(action, sizeof(action), OP_BRC);
        if (ss.fail())
            doReference(cellName, action + 1);          // doReference handles invalid input as well
        else if (strcmp(action, "SUM") == 0)
            doSUM(cellName, ss);
        else if (strcmp(action, "AVERAGE") == 0)
            doAVERAGE(cellName, ss);
        else if (strcmp(action, "MAX") == 0)
            doMAX(cellName, ss);
        else if (strcmp(action, "MIN") == 0)
            doMIN(cellName, ss);
        else if (strcmp(action, "LEN") == 0)
            doLEN(cellName, ss);
        else if (strcmp(action, "CONCAT") == 0)
            doCONCAT(cellName, ss);
        else if (strcmp(action, "SUBSTR") == 0)
            doSUBSTR(cellName, ss);
        else if (strcmp(action, "COUNT") == 0)
            doCOUNT(cellName, ss);
        else
            std::cout << "Invalid command!\n";
    }
    else
    {
		ss >> action;
        if (strcmp(action, "insert") == 0)
            doInsert(cellName, ss);
        else if (strcmp(action, "delete") == 0)
            doDelete(cellName);
        else
            std::cout << "Invalid command!\n";
    }
    if (clearConsoleAfterCommand)
    {
        system("cls");
    }
}

bool Table::isRange(std::stringstream& ss)
{
    std::stringstream temp;
    temp << ss.str();

    if (countCommas(temp))                        // more than one argument
		return false; 

    char buff[BUFFER_SIZE];
    temp.getline(buff, sizeof(buff));
    char* colon = strchr(buff, DELIMITER);
    if (!colon)
        return false;
	else
		return true;
}

void Table::setNumericValue(const char* cellName, int value)
{
    int row = cellName[0] - 'A' + 1;
    int col = atoi(cellName + 1);
    rows[row].setCellAtInd(col, value);
}

void Table::setStringValue(const char* cellName, std::stringstream& ss)
{
    int row = cellName[0] - 'A' + 1;
    int col = atoi(cellName + 1);
    rows[row].setCellAtInd(col, ss.str());
}

void Table::setErrorValue(const char* cellName)
{
    int row = cellName[0] - 'A' + 1;
    int col = atoi(cellName + 1);
    rows[row].setCellAtInd(col, "#VALUE!");
}

bool Table::isCllNameValid(const char* cellName) const
{
    if (strlen(cellName) < 2 || cellName[0] < 'A' || cellName[0] > 'Z') 
        return false;

    int row = cellName[0] - 'A' + 1;
    int col = atoi(cellName + 1);

    if (col == 0 || row <= 0 || row >= (int)initialTableRows || col >= (int)initialTableCols) 
        return false;

    return true;
}

bool Table::initializeEmptyTable() 
{
    for (size_t row = 0; row < initialTableRows; row++)         
    {
        rows[row].reinit(initialTableCols);                     // to fix the columns in each row
        if (row >= 1)                                           // set the header col
        {
            char rowHeader[2] = { char('A' + row - 1), '\0' };
            rows[row].setCellAtInd(0, rowHeader);
        }
    }

    for (size_t col = 1; col < initialTableCols; col++)         // set the header row
        rows[0].setCellAtInd(col, std::to_string(col));

	return true;    
}

bool Table::initializeTableFromFile(const char* table_file) 
{
    if (!table_file) 
        return false;

    std::ifstream ifs(table_file);
    if (!ifs.is_open()) 
    {
        std::cout << "Error while opening table file!" << std::endl;
        return false;
    }

    initializeEmptyTable();

    for (size_t i = 1; i < initialTableRows; i++) 
    {
        char buff[MAX_SYMBOLS];
        ifs.getline(buff, MAX_SYMBOLS);
        rows[i].parseRowFromFile(buff);
    }

    ifs.close();
	return true;
}

void Table::resetTable() 
{
    for (size_t i = 0; i < initialTableRows; i++)
        rows[i].reinit(initialTableCols);
}

bool Table::loadConfig(const char* config_file) 
{
    if (!config_file) 
		return false;

    std::ifstream file(config_file);
    if (!file.is_open()) 
    {
        std::cout << "Error while opening configuration file!" << std::endl;
        return false;
    }

    char property[BUFFER_SIZE];
    char value[BUFFER_SIZE];
    while (file.getline(property, BUFFER_SIZE, DELIMITER) && file.getline(value, BUFFER_SIZE)) 
    {
        if (strcmp(property, "initialAlignment") == 0) 
        {
            if (strcmp(value, "left") == 0)
                initialAlignment = Alignment::LEFT;
            else if (strcmp(value, "center") == 0)
                initialAlignment = Alignment::CENTER;
            else if (strcmp(value, "right") == 0)
                initialAlignment = Alignment::RIGHT;
            else 
            {
                std::cout << "ABORTING! " << property << DELIMITER << value << SPACE << DASH << " Invalid value!\n";
				return false;
            }
            continue;
        }
        else if (strcmp(property, "clearConsoleAfterCommand") == 0) 
        {
            if (strcmp(value, "true") == 0)
                clearConsoleAfterCommand = true;
            else if (strcmp(value, "false") == 0)
                clearConsoleAfterCommand = false;
            else 
            {
                std::cout << "ABORTING! " << property << DELIMITER << value << SPACE << DASH << " Invalid value!\n";
                return false;
            }
            continue;
        }
        else if (strcmp(property, "autoFit") == 0) 
        {
            if (strcmp(value, "true") == 0)
                autoFit = true;
            else if (strcmp(value, "false") == 0)
                autoFit = false;
            else
            {
                std::cout << "ABORTING! " << property << DELIMITER << value << SPACE << DASH << " Invalid value!\n";
                return false;
            }
            continue;
        }

        int val = atoi(value);
        if (val <= 0)
        {
            std::cout << "ABORTING! " << property << DELIMITER << value << SPACE << DASH << " Invalid value!\n";
            return false;
        }

        if (strcmp(property, "initialTableRows") == 0)
			initialTableRows = val + 1;                         // +1 for the header row
        else if (strcmp(property, "initialTableCols") == 0)
			initialTableCols = val + 1;                         // +1 for the header col
        else if (strcmp(property, "maxTableRows") == 0)
            maxTableRows = val + 1;
        else if (strcmp(property, "maxTableCols") == 0)
            maxTableCols = val + 1;
        else if (strcmp(property, "visibleCellSymbols") == 0)
            visibleCellSymbols = val;
        else
        {
            std::cout << "ABORTING! " << property << DELIMITER << value << SPACE << DASH << " Missing property!\n";
            return false;
        }
    }

    file.close();
    return true;
}

bool Table::saveTableToFile(const char* filename) const 
{
    if (!filename) 
        return false;

    std::ofstream ofs(filename);
    if (!ofs.is_open()) 
    {
        std::cout << "Error while opening file for writing!" << std::endl;
        return false;
    }

    for (size_t i = 1; i < initialTableRows; i++)
        rows[i].saveRowToFile(ofs);

    ofs.close();
	return true;
}

Cell* Table::getCellFromName(const char* name) {
    if (!name || strlen(name) < 2)
        return nullptr;

    char rowChar = name[0];
    if (rowChar < 'A' || rowChar > 'Z')
        return nullptr;

    int row = rowChar - 'A' + 1;
    int col = atoi(name + 1);

    if (row <= 0 || col <= 0 || row >= (int)initialTableRows || col >= (int)initialTableCols)
        return nullptr;

    return rows[row].getCellAtInd(col);
}
