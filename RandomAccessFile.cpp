// =============================================================================
// File: RandomAccessorFile.h
// =============================================================================
// Description:
//      This header file hosts the utility functions for random access files.
// =============================================================================

#ifndef RANDOMACCESSORFILE_H
#define RANDOMACCESSORFILE_H

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <math.h>
#include <memory>
//#include "Cipher.h" // TODO
using namespace std;

// === RandomAccessorFile ======================================================
// This class represents the random access file.
// =============================================================================
template <class T>
class RandomAccessorFile {
private:
    static const int MAX_RECORDS = 100; // TODO: move to be param of contructor?
    bitset<MAX_RECORDS> available_ids;
    T dummy_record;
    
    string file_name;
    fstream file;

    int reserveNextAvailableId();
    void releaseId(int id);
    void updateFile(int id, T record, bool update_available_ids = false);
    int calculateOffset(int id, bool include_available_ids = true);

public:
    RandomAccessorFile(string file_name, T dummy_record);

    int createRecord(T record);
    bool deleteRecord(int id, T record);
    unique_ptr<T> getRecord(int id);
    void updateRecord(int id, T record);
};

// === RandomAccessorFile::RandomAccessorFile ==================================
// This is the constructor for the RandomAccessorFile class. It creates and
// initializes the raf with n = MAX_RECORDS dummy records if needed or loads
// the existing raf.
//
// Input:
//      file_name [IN]                  -- name of the ra file
//
// No Output.
// =============================================================================
template<class T>
RandomAccessorFile<T>::RandomAccessorFile(string file_name, T dummy_record) {
    this->file_name = file_name;
    this->dummy_record = dummy_record;

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
        file.write((char*)&dummy_record, sizeof(dummy_record));
    }
    file.close();
}

// ==== RandomAccessorFile::reserveNextAvailableId =============================
// This function gets the next available id & sets it to unavailable.
//
// Input: None
//
// Output:
//      int representing the id that was just reserved if one was available,
//      otherwise, -1
// =============================================================================
template <class T>
int RandomAccessorFile<T>::reserveNextAvailableId() {
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

// ==== RandomAccessorFile::releaseId ==========================================
// This function sets the id of the record that is being deleted to available so
// it can be used for another record.
//
// Input:
//      id [IN]                -- the id that needs to be released
//
// Output: None
// =============================================================================
template <class T>
void RandomAccessorFile<T>::releaseId(int id) {
    available_ids.set(id/10 - 1, true);
}

// ==== RandomAccessorFile::updateFile =========================================
// This function updates the random access file. It's essentially like updating
// a record in the database. It should be the final step of a transaction.
//
// Input:
//      id [IN]                         -- the id of the record being updated
//      record [IN]                     -- the record that had a transaction 
//                                          performed
//      update_available_ids [OPT IN]   -- optional: boolean indicating whether 
//                                          there a the set of available_ids
//                                          needs to be updated in the RAF.
//                                          defaults to false
//
// Output: None
// =============================================================================
template <class T>
void RandomAccessorFile<T>::updateFile(int id, T record,
    bool update_available_ids /*= false*/) {
    file.open(file_name, ios::in | ios::binary);

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

    int byte_offset = calculateOffset(record, false);
    
    file.seekp(byte_offset, ios::cur);
    file.write((char*)&record, sizeof(record));
    file.close();
}

// === RandomAccessorFile::calculateOffset =====================================
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
template <class T>
int RandomAccessorFile<T>::calculateOffset(int id,
    bool include_available_ids /*= true*/) {
    // TODO: validate id?
    int offset = (id/10 - 1) * sizeof(T);
    if (include_available_ids) {
        offset += sizeof(available_ids);
    }
    return offset;
}

// ==== RandomAccessorFile::createRecord =======================================
// This function adds a new record to the RAF if there is room for one.
//
// Input:
//      record [IN]             -- the record to be added
//
// Output:
//      The id of the new record if there was room for one, otherwise -1.
// =============================================================================
template <class T>
int RandomAccessorFile<T>::createRecord(T record) {
    int id = reserveNextAvailableId();

    if (id != -1) {
        updateFile(id, record, true);
    }

    return id;
}

// === RandomAccessorFile::deleteRecord ========================================
// This functions deletes a record from the raf if it's up to date.
//
// Input:
//      id [IN]                 -- the id of the record being deleted
//      record [IN]             -- the record that is being deleted
//
// Output:
//      true if succeeded in deleting the record, otherwise false
// =============================================================================
template <class T>
bool RandomAccessorFile<T>::deleteRecord(int id, T record) { // TODO: password?
    // TODO: validate id?
    unique_ptr<T> record_in_raf = getRecord(id);

    // TODO: this
    // if (record_in_raf != record) {
    //     return false;
    // }

    releaseId(id);
    updateFile(id, dummy_record, true);

    return true;
}

// ==== RandomAccessorFile::getRecord ==========================================
// This function attempts to get a record from the RAF.
//
// Input: None
//
// Output:
//      pointer to record if it was able to be located, otherwise nullptr
// =============================================================================
template <class T>
unique_ptr<T> RandomAccessorFile<T>::getRecord(int id) {
    int byte_offset = calculateOffset(id);

    unique_ptr<T> record(new T());
    file.open(file_name, ios::in | ios::binary);
    file.seekp(byte_offset, ios::beg);
    file.read((char*)record.get(), sizeof(T));
    file.close();

    return record;
}

// ==== RandomAccessorFile::updateRecord =======================================
// This function updates a record.
//
// Input:
//      id [IN]                         -- the id of the record being updated
//      record [IN]                     -- the record that is being updated
//
// Output: None
// =============================================================================
template <class T>
void RandomAccessorFile<T>::updateRecord(int id, T record) {
    // TODO: validate id?
    updateFile(id, record);
}

#endif // RANDOMACCESSORFILE_H