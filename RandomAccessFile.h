// =============================================================================
// File: RandomAccessFile.h
// =============================================================================
// Description:
//      This header file hosts the utility functions for random access files.
// =============================================================================

#ifndef RANDOMACCESSFILE_H
#define RANDOMACCESSFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <math.h>
#include <memory>
//#include "Cipher.h" // TODO
using namespace std;

// === RandomAccessFile ========================================================
// This class represents the random access file.
// =============================================================================
class RandomAccessFile {
public:
    class RandomAccessFileRecord;

    RandomAccessFile(string file_name,
        unique_ptr<RandomAccessFileRecord> dummy_record);

    int getNextAvailableId();
    bool createRecord(RandomAccessFileRecord* record);
    bool deleteRecord(RandomAccessFileRecord* record);
    bool getRecord(int id, RandomAccessFileRecord* record);
    //void RandomAccessFile<T>::getRecord(int id, unique_ptr<T> &record) {
    void updateRecord(RandomAccessFileRecord* record);
private:
    static const int MAX_RECORDS = 100; // TODO: move to be param of contructor?
    bitset<MAX_RECORDS> available_ids;
    unique_ptr<RandomAccessFileRecord> dummy_record;

    string file_name;
    fstream file;

    bool reserveId(int id);
    void releaseId(int id);
    void updateFile(int id, RandomAccessFileRecord* record,
        bool update_available_ids = false);
    int calculateOffset(int id, bool include_available_ids = true);
};

// === RandomAccessFile:RandomAccessFileRecord =================================
// This class is the base for records in the random access file.
// =============================================================================
class RandomAccessFile::RandomAccessFileRecord {
public:
    // === getId ===============================================================
    // This function should be overridden in derived classes. It gets the id of
    // the record.
    //
    // Input: None
    //
    // Output:
    //      int representing the id the of the record
    // =========================================================================
    virtual int getId() { return -1; }
};

// === RandomAccessFile::RandomAccessFile ======================================
// This is the constructor for the RandomAccessFile class. It creates and
// initializes the raf with n = MAX_RECORDS dummy records if needed or loads
// the existing raf.
//
// Input:
//      file_name [IN]                  -- name of the ra file
//      dummy_record [IN]               -- the dummy record to be given to the
//                                          RAF class
//
// No Output.
// =============================================================================
RandomAccessFile::RandomAccessFile(string file_name, 
    unique_ptr<RandomAccessFileRecord> dummy_record) {
    this->file_name = file_name;
    this->dummy_record = move(dummy_record);

    if (fstream(file_name)) { // file already exists
        // TODO: not validating the input here
        file.open(file_name, ios::in | ios::binary);
        file.read((char*)&available_ids, sizeof(available_ids));
        file.close();
        return;
    }

    available_ids.set();
    file.open(file_name, ios::out | ios::binary);
    if (file.fail()) {
        cout << "Error opening file\nExiting\n";
        exit(-10); // TODO: change to something better than -10
    }

    // add the available ids set at beginning of the file
    file.write((char*)&available_ids, sizeof(available_ids));

    // initialize the raf with dummy records
    for (int i = 0; i < MAX_RECORDS; i++) {
        file.write((char*)dummy_record.get(), sizeof(*dummy_record)); // TODO: may not work
    }
    file.close();
}

// ==== RandomAccessFile::reserveId ============================================
// This function sets an id to unavailable.
//
// Input:
//      id [IN]                         -- id to be reserved
//
// Output:
//      true if id was able to be reserved, otherwise false
// =============================================================================
bool RandomAccessFile::reserveId(int id) {
    id = id/10 -1;
    if (!available_ids[id]) {
        return false;
    }

    available_ids.set(id, false);
    return true;
}

// ==== RandomAccessFile::releaseId ============================================
// This function sets the id of the record that is being deleted to available so
// it can be used for another record.
//
// Input:
//      id [IN]                         -- the id that needs to be released
//
// Output: None
// =============================================================================
void RandomAccessFile::releaseId(int id) {
    available_ids.set(id/10 - 1, true);
}

// ==== RandomAccessFile::getNextAvailableId ===================================
// This function gets the next available id.
//
// Input: None
//
// Output:
//      int representing the next id if one was available, otherwise, -1
// =============================================================================
int RandomAccessFile::getNextAvailableId() {
    if (available_ids.none()) {
        cout << "No available ids\n";
        return -1;
    }

    int id;
    for (id = 0; id < MAX_RECORDS; id++) {
        if (available_ids[id]) {
            available_ids.set(id, false);
            break;
        }
    }

    return (id + 1) * 10;
}

// ==== RandomAccessFile::updateFile ===========================================
// This function updates the random access file. It's essentially like updating
// a record in the database. It should be the final step of a transaction.
//
// Input:
//      id [IN]                         -- id of the record to be updated
//      record [IN]                     -- pointer to the record that had a 
//                                          transaction  performed
//      update_available_ids [OPT IN]   -- optional: boolean indicating whether 
//                                          there a the set of available_ids
//                                          needs to be updated in the RAF.
//                                          defaults to false
//
// Output: None
// =============================================================================
void RandomAccessFile::updateFile(int id, RandomAccessFileRecord* record,
    bool update_available_ids /*= false*/) {
    file.open(file_name, ios::out | ios::binary);

    if (file.fail()) {
        cout << "Openning file failed\n";
        exit(-10); // TODO: code/msg better than -10?
    }

    if (update_available_ids) {
        file.write((char*)&available_ids, sizeof(available_ids));
    }
    else {
        file.seekp(sizeof(available_ids), ios::beg);
    }

    int byte_offset = calculateOffset(id, false);
    
    file.seekp(byte_offset, ios::cur);
    file.write((char*)record, sizeof(*record));
    file.close();
}

// === RandomAccessFile::calculateOffset =======================================
// This function calculates where a record should be in the random access file.
//
// Input:
//      id [IN]                         -- the id of the record
//      include_available_ids [OPT IN]  -- optional: boolean indicating whether 
//                                          the set at located at the beginning 
//                                          of the RAF should be accounted for 
//                                          in the offset. defaults to true
//
// Output:
//      int representing the offset of the record in bytes
// =============================================================================
int RandomAccessFile::calculateOffset(int id,
    bool include_available_ids /*= true*/) {

    int offset = (id/10 - 1) * sizeof(*dummy_record);
    if (include_available_ids) {
        offset += sizeof(available_ids);
    }
    return offset;
}

// ==== RandomAccessFile::createRecord =========================================
// This function adds a new record to the RAF if there is room for one.
//
// Input:
//      record [IN]                     -- pointer to the record to be added
//
// Output:
//      true if the record was added, otherwise false
// =============================================================================
bool RandomAccessFile::createRecord(RandomAccessFileRecord* record) {
    // TODO: validate record->getId
    int id = record->getId();

    if (!reserveId(id)) {
        return false; // TODO: print msg?
    }

    updateFile(id, record, true);

    return true;
}

// === RandomAccessFile::deleteRecord ==========================================
// This functions deletes a record from the raf if it's up to date.
//
// Input:
//      record [IN]             -- the record that is being deleted
//
// Output:
//      true if succeeded in deleting the record, otherwise false
// =============================================================================
bool RandomAccessFile::deleteRecord(RandomAccessFileRecord* record) { // TODO: password?
    // TODO: validate record->getId
    int id = record->getId();

    // TODO: this..later
    // if (getRecord(id) != record) {
    //     return false;
    // }

    releaseId(id);
    updateFile(id, dummy_record.get(), true);

    return true;
}

// ==== RandomAccessFile::getRecord ============================================
// This function attempts to get a record from the RAF.
//
// Input:
//      id [IN]                         -- id of the record to get
//      record [OUT]                    -- pointer to where the record should
//                                          get written out to
//
// Output:
//      true if able to get the record, otherwise false
// =============================================================================
bool RandomAccessFile::getRecord(int id, RandomAccessFileRecord* record) {
    if (id < 10 || id > MAX_RECORDS * 10 || id % 10 != 0) {
        cout << "Invalid id\n";
        return false;
    } // TODO: move this validation

    int byte_offset = calculateOffset(id);

    //unique_ptr<T> record(new T());
    file.open(file_name, ios::in | ios::binary);
    file.seekp(byte_offset, ios::beg);
    file.read((char*)record, sizeof(*record));
    file.close();

    return true;
}

// ==== RandomAccessFile::updateRecord =========================================
// This function updates a record.
//
// Input:
//      record [IN]                     -- the record that is being updated
//
// Output: None
// =============================================================================
void RandomAccessFile::updateRecord(RandomAccessFileRecord* record) {
    // TODO: validate id?
    int id = record->getId();
    updateFile(id, record);
}

#endif // RANDOMACCESSFILE_H