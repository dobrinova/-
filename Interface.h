#pragma once
#include "Table.h"
#include "Constants.h"
#include <iostream>
#include <sstream>

void runCommands(Table& table) 
{
    char command[BUFFER_SIZE];

    while (true) 
    {
        std::cout << "\n> ";
        std::cin.getline(command, BUFFER_SIZE);

        if (strcmp(command, "exit") == 0)
            break;
        else if (strcmp(command, "display") == 0)
            table.display();

        table.executeCommand(command);
    }
}

void startInterface() 
{
    std::cout << "\n> ";

    char input[BUFFER_SIZE];
    std::cin.getline(input, BUFFER_SIZE);

    std::stringstream ss(input);
    char command[POS_SIZE];
    ss >> command;

    if (strcmp(command, "open") == 0) 
    {
        char tableName[MAX_SYMBOLS], configName[MAX_SYMBOLS];
        ss >> tableName >> configName;

        if (strlen(tableName) == 0 || strlen(configName) == 0) 
        {
            std::cout << "Missing table or config file name!\n";
            return;
        }

        Table table(configName, tableName);
        table.display();
        runCommands(table);
    }
    else if (strcmp(command, "new") == 0) 
    {
        char configName[MAX_SYMBOLS];
        ss >> configName;

        if (strlen(configName) == 0) {
            std::cout << "Missing config file name!\n";
            return;
        }

        Table table(configName);
        table.display();
        runCommands(table);
    }
    else 
    {
        std::cout << "Invalid command!\n";
    }
}

