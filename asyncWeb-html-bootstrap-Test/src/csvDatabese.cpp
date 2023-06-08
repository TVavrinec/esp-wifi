#include "csvDatabese.hpp"


// public:
csvDatabese::csvDatabese(const char *filename) {
    _filename = filename;
    if (!LittleFS.begin())
    {
        Serial.println("An error occurred while mounting LittleFS");
        return;
    }
    File file = LittleFS.open(filename, "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
    }

    String line;
    // Read data
    while (file.available()) {
        line = file.readStringUntil('\n');
        _data.push_back(parsLine(line));
    }
}

/*
    * @brief Get cell from record by name
    * 
    * @param name - name of record
    * @param cellName - name of Column
    * @return String - cell
*/
String csvDatabese::getRecordCell(const char *name, const char *columnName) {
    std::lock_guard<std::mutex> lock(_mutex__);
    
    int cellNubmer = -1;
    for (int i = 0; i < _data[0].size(); i++)
    {
        if (_data[0][i] == columnName) {
            cellNubmer = i;
            break;
        }
    }
    if (cellNubmer == -1) {
        Serial.printf("Error: Wrong column name %s, in csvDatabaze.cpp on %d line\n", columnName, __LINE__);
        return "";
    }
    String record;
    String nameStr = String(name);
    for (int i = 0; i < _data.size(); i++)
    {
        if (_data[i][0] == nameStr) {
            record = _data[i][cellNubmer];
            break;
        }
    }
    return record;
}

/*
    * @brief Get all records name
    * 
    * @return std::vector<String> - vector of records name
*/
std::vector<String> csvDatabese::getRecordsName() {
    std::lock_guard<std::mutex> lock(_mutex__);

    std::vector<String> recordsName;
    for (int i = 0; i < _data.size(); i++)
    {
        recordsName.push_back(_data[i][0]);
    }
    return recordsName;
}

/*
    * @brief Print database
*/
void csvDatabese::printDatabase() {
    std::lock_guard<std::mutex> lock(_mutex__);

    for (int i = 0; i < _data.size(); i++)
    {
        for (int j = 0; j < _data[i].size(); j++)
        {
            Serial.printf("%20.20s |",_data[i][j].c_str());
        }
        Serial.printf("\n");
    }
}

/*
    * @brief Add record to database
    * 
    * @param record - vector of cells
*/
void csvDatabese::addRecord(std::vector<String> record) {
    std::lock_guard<std::mutex> lock(_mutex__);

    for (int a = 0; a < _data.size(); a++)
    {
        if (_data[a][0] == record[0]) {
            return;
        }
    }
    _data.push_back(record);
    writeData();
}

/*
    * @brief Change record in database
    * 
    * @param name - name of record
    * @param record - vector of cells
*/
void csvDatabese::changeRecord(const char *name, std::vector<String> record) {
    std::lock_guard<std::mutex> lock(_mutex__);

    String nameStr = String(name);
    for (int i = 0; i < _data.size(); i++)
    {
        if (_data[i][0] == nameStr) {
            _data[i] = record;
            break;
        }
    }
    writeData();
}

/*
    * @brief Delete record from database
    * 
    * @param name - name of record
*/
void csvDatabese::deleteRecord(const char *name) {
    std::lock_guard<std::mutex> lock(_mutex__);

    String nameStr = String(name);
    for (int i = 0; i < _data.size(); i++)
    {
        if (_data[i][0] == nameStr) {
            _data.erase(_data.begin() + i);
            break;
        }
    }
    writeData();
}

/*
    * @brief Get record by name
    * 
    * @param name - name of record
    * @return std::vector<String> - vector of cells
*/
std::vector<String> csvDatabese::getRecord(const char *name) {
    std::lock_guard<std::mutex> lock(_mutex__);

    String nameStr = String(name);
    for (int i = 0; i < _data.size(); i++)
    {
        if (_data[i][0] == nameStr) {
            return _data[i];
        }
    }
    // Serial.printf("Error: Wrong name %s, in csvDatabaze.cpp on %d line\n", name, __LINE__);
    return std::vector<String>();
}

/*
 * @brief Get record by number in database
 * 
 * @param number - number of record
 * @return std::vector<String> - vector of cells
*/
std::vector<String> csvDatabese::getRecord(int number) {
    std::lock_guard<std::mutex> lock(_mutex__);

    if (number < 0 || number >= _data.size()) {
        Serial.printf("Warning: Wrong number %d, in csvDatabaze.cpp on %d line\n", number, __LINE__);
        return std::vector<String>();
    }
    return _data[number];
}

// private:

/*
    * @brief Parse line in csv format
    * 
    * @param line - line from csv file
    * @return std::vector<String> - vector of cells
*/
std::vector<String> csvDatabese::parsLine(String line) {
    std::vector<String> data_line;
    line.trim();
    line.replace(" ", "");
    int cellEnd = line.indexOf(',');
    int lastCellEnd = 0;
    if (cellEnd == -1)
    {
        data_line.push_back(line);
        return data_line;
    }
    
    while (cellEnd > 0) {
        data_line.push_back(line.substring(lastCellEnd, cellEnd));
        lastCellEnd = cellEnd + 1;
        cellEnd = line.indexOf(',', lastCellEnd);
        if (cellEnd == -1) {
            data_line.push_back(line.substring(lastCellEnd));
        }        
    }
    return data_line;
}

/*
    * @brief Write data to file
    * 
    * @return bool - true if success
*/
bool csvDatabese::writeData() {
    File file = LittleFS.open(_filename, "w");
    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return false;
    }
    for (int i = 0; i < _data.size(); i++)
    {
        for (int j = 0; j < _data[i].size(); j++)
        {
            file.print(_data[i][j]);
            if (j != _data[i].size() - 1) {
                file.print(",");
            }
        }
        file.print("\n");
    }
    file.close();
    return true;
}
