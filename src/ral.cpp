// =============================================================================
// File: ral.cpp
// =============================================================================
// Description:
//      This file is the implementation of the ral namespace.
// =============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include "ral.h"
//#include "Cipher.h" // TODO
// TODO: validating/santizing input

using namespace ral;

File::File(string file_name, 
    unique_ptr<Record> dummy_record) {
    this->file_name = file_name + FILE_EXTENSION;
    this->dummy_record = move(dummy_record);
    record_size = this->dummy_record->getSize();

    if (fstream(this->file_name)) { // file already exists
        // TODO: not validating the input here
        file.open(this->file_name, ios::in | ios::binary);
        file.read((char*)&available_ids, sizeof(available_ids));
        file.close();
        return;
    }

    available_ids.set();
    file.open(this->file_name, ios::out | ios::binary);
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

bool File::reserveId(int id) {
    id = id/10 -1;
    if (!available_ids[id]) {
        return false;
    }

    available_ids.set(id, false);
    return true;
}

void File::releaseId(int id) {
    available_ids.set(id/10 - 1, true);
}

int File::getNextAvailableId() {
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

void File::updateFile(int id, Record* record,
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

int File::calculateOffset(int id,
    bool include_available_ids /*= true*/) {

    int offset = (id/10 - 1) * record_size;
    if (include_available_ids) {
        offset += sizeof(available_ids);
    }
    return offset;
}

bool File::createRecord(Record* record) {
    // TODO: validate record->getId
    int id = record->getId();

    if (!reserveId(id)) {
        return false; // TODO: print msg?
    }

    updateFile(id, record, true);

    return true;
}

bool File::deleteRecord(Record* record) { // TODO: password?
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

bool File::getRecord(int id, Record* record) {
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

void File::updateRecord(Record* record) {
    // TODO: validate id?
    int id = record->getId();
    updateFile(id, record);
}