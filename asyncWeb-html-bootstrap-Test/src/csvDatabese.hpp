#pragma once
#include <mutex>

#include <FS.h>
#include <LittleFS.h>
#include <iostream>
#include <vector>
#include <string>

class csvDatabese {
private:
    std::mutex _mutex__;

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

    void changeRecord(const char *name, std::vector<String> record);

    void deleteRecord(const char *name);

    int getNumberOfRecords(){
        std::lock_guard<std::mutex> lock(_mutex__);
        return _data.size();
    }
    int getNumberOfColumns(){
        std::lock_guard<std::mutex> lock(_mutex__);
        return _data[0].size();
    }

    String getRecordCell(const char *name, const char *columnName);

    std::vector<String> getRecord(const char *name);
    std::vector<String> getRecord(int index);

    std::vector<String> getRecordsName();
};