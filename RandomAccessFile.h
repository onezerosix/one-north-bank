// =============================================================================
// File: ral.h
// =============================================================================
// Description:
//      This header file hosts the random access library (ral) namespace. ///// which contains the
//      File class definition that controls a random access file, and the
//      abstract Record class that is used for the records in the random access file.
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
// TODO: validating/santizing input

namespace ral {
    using namesspace std;

    // === Record ==============================================================
    // This abstract class is the base for records in the random access file.
    // =========================================================================
    class Record {
    public:
        // === getId ===========================================================
        // Parameters: None
        //
        // Return val:
        //      int representing the id the of the record
        // =====================================================================
        virtual int getId() = 0;

        // === getSize =========================================================
        // Parmeters: None
        //
        // Return val:
        //      size_t equal to the size that a serialized record would be (in
        //      bytes)
        // =====================================================================
        virtual size_t getSize() = 0;

        // === serialize =======================================================
        // Parmeters:
        //      ss [REF]                -- stream where the serialized record
        //                                  will be written to
        //
        // Return val:
        //      true if able to serialize, otherwise false
        // =====================================================================
        virtual bool serialize(stringstream &ss) = 0;

        // === deserialize =====================================================
        // Parmeters:
        //      ss [REF]                -- stream where a serialized record will
        //                                  be read from
        //
        // Return val:
        //      true if able to deserialize, otherwise false
        // =====================================================================
        virtual bool deserialize(stringstream &ss) = 0;
    };

    // === File ================================================================
    // This class controls a random access file of 100 records.
    // =========================================================================
    class File {
    private:
        static const int MAX_RECORDS = 100; // TODO: move to be adjustable
        bitset<MAX_RECORDS> available_ids;
        unique_ptr<Record> dummy_record;
        size_t record_size;

        string file_name;
        fstream file;

        bool reserveId(int id);
        void releaseId(int id);
        void updateFile(int id, Record* record,
            bool update_available_ids = false);
        int calculateOffset(int id, bool include_available_ids = true);

    public:
        // === File ============================================================
        // This is the constructor. It creates a new raf or loads the existing
        // raf.
        // =========================================================================
        // Input:
        //      file_name [VAL]         -- name of the ra file
        //      dummy_record [REF]      -- a dummy record to be given to this
        //                                  class
        //
        // No Output.
        // =============================================================================
        File(string file_name, unique_ptr<Record> dummy_record);

        int getNextAvailableId();
        bool createRecord(Record* record);
        bool deleteRecord(Record* record);
        bool getRecord(int id, Record* record);
        void updateRecord(Record* record);
    };



RandomAccessFile::RandomAccessFile(string file_name, 
    unique_ptr<RandomAccessFileRecord> dummy_record) {
    this->file_name = file_name;
    this->dummy_record = move(dummy_record);
    record_size = this->dummy_record->getSize();

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
    stringstream ss;
    if (!this->dummy_record->serialize(ss)) {
        cout << "Error with serializing record\nExiting\n";
        file.close();
        exit(-10); // TODO: change to something better?
    }
    if (ss.tellp() != record_size) {
        file.close();
        cout << "Error serializing records\nExiting\n";
        exit(-10); // TODO: change to something better?
    }
    if (!ss) {
        file.close(); // TODO: file.close repeatedly...
        cout << "Error serializing status\nExiting\n";
        exit(-10); // TODO: change to something better?
    }

    char record[record_size];
    ss.seekg(0, ios::beg);
    ss.read(record, record_size);
    for (int i = 0; i < MAX_RECORDS; i++) {
        file.write(record, record_size);
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
// TODO: bool return?
void RandomAccessFile::updateFile(int id, RandomAccessFileRecord* record,
    bool update_available_ids /*= false*/) {
    file.open(file_name, ios::out | ios::in | ios::binary);

    if (file.fail()) {
        cout << "Opening file failed\n";
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

    stringstream ss; // TODO: same as constructor..
    if (!record->serialize(ss)) {
        cout << "Error with serializing record\nExiting\n";
        file.close();
        exit(-10); // TODO: change to something better?
    }
    if (ss.tellp() != record_size) {
        file.close();
        cout << "Error serializing records\nExiting\n";
        exit(-10); // TODO: change to something better?
    }
    if (!ss) {
        file.close(); // TODO: file.close repeatedly...
        cout << "Error serializing status\nExiting\n";
        exit(-10); // TODO: change to something better?
    }

    char serialized_record[record_size];
    ss.seekg(0, ios::beg);
    ss.read(serialized_record, record_size);
    file.write(serialized_record, record_size);
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

    int offset = (id/10 - 1) * record_size;
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
        //cout << "Invalid id\n";
        return false;
    } // TODO: move this validation

    int byte_offset = calculateOffset(id);

    //unique_ptr<T> record(new T());
    file.open(file_name, ios::in | ios::binary);
    file.seekg(byte_offset, ios::beg);

    char serialized_record[record_size];
    file.read(serialized_record, record_size);
    file.close();

    stringstream ss;
    ss.write(serialized_record, record_size);
    if (!record->deserialize(ss)) {
        cout << "Error with deserializing\n";
        return false;
    }
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
}

#endif // RANDOMACCESSFILE_H