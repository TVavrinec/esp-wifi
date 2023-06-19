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
    csvDatabese(const csvDatabese& other) = delete;
    csvDatabese& operator=(const csvDatabese& other) = delete;
    csvDatabese(csvDatabese &&) = delete;
    csvDatabese& operator=(csvDatabese &&) = delete;

    void printDatabase();

    void addRecord(std::vector<String> record);

    void changeRecord(String name, std::vector<String> record);
    void changeRecord(String name, String columnName, String value);
    void changeRecord(int index, std::vector<String> record);
    void changeRecord(int index, String columnName, String value);

    void deleteRecord(const char *name);

    int getNumberOfRecords(){
        return _data.size();
    }
    int getNumberOfColumns(){
        return _data[0].size();
    }

    String getRecordCell(String name, String columnName);
    String getRecordCell(int index, String columnName);

    std::vector<String> getRecord(const char *name);
    std::vector<String> getRecord(int index);

    std::vector<String> getRecordsName();
};