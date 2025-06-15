#pragma once
#include "Row.h"
#include "Alignment.h"
#include "Constants.h"      // Constraints* .... ops
#include <vector>

class Table {
private:
    size_t initialTableRows;
    size_t initialTableCols;
    size_t maxTableRows;
    size_t maxTableCols;
    bool autoFit;
    size_t visibleCellSymbols;
    Alignment initialAlignment;
    bool clearConsoleAfterCommand;

    Row rows[MAX_ROWS];

    void resetTable();
    bool loadConfig(const char* config_file);
    bool initializeEmptyTable();
    bool initializeTableFromFile(const char* table_file);
    void displayLine() const;
    void displayLine(const size_t* maxLenPerCol) const;

    void doInsert(const char* cellName, std::stringstream& ss);
	void doDelete(const char* cellName);
    void doReference(const char* targetCell, const char* refCell);
    std::vector<int> getNumericValues(std::stringstream& ss);
    
    int countCommas(std::stringstream& ss);
    void doSUM(const char* cellName, std::stringstream& ss);
	void doAVERAGE(const char* cellName, std::stringstream& ss);
	void doMAX(const char* cellName, std::stringstream& ss);
	void doMIN(const char* cellName, std::stringstream& ss);
	void doLEN(const char* cellName, std::stringstream& ss);
	void doCONCAT(const char* cellName, std::stringstream& ss);
	void doSUBSTR(const char* cellName, std::stringstream& ss);
	void doCOUNT(const char* cellName, std::stringstream& ss);

	bool isRange(std::stringstream& ss);
    void setNumericValue(const char* cellName, int value);
    void setStringValue(const char* cellName, std::stringstream& ss);
    void setErrorValue(const char* cellName);
	bool isCllNameValid(const char* cellName) const;
public:
    Table(const char* config_file);
    Table(const char* config_file, const char* table_file);

    bool saveTableToFile(const char* filename) const;
    void display() const;

    void executeCommand(const char* command);
    Cell* getCellFromName(const char* name);

};