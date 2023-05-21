#pragma once

#include <FS.h>
#include <LittleFS.h>
#include <iostream>
#include <vector>
#include <string>

class csvDatabese {
private:
    const char *_filename;
    std::vector<std::vector<String>> _data;

    std::vector<String> parsLine(String line);

    bool writeData();

public:
    csvDatabese(const char *_filename);

    void printDatabase();

    void addRecord(std::vector<String> record);

    void deleteRecord(const char *name);

    int getNumberOfRecords(){return _data.size();}
    int getNumberOfColumns(){return _data[0].size();}

    String getRecordCell(const char *name, const char *columnName);

    std::vector<String> getRecord(const char *name);
    std::vector<String> getRecord(int index);

    std::vector<String> getRecordsName();
};