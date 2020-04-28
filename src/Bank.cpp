// =============================================================================
// File: Bank.cpp
// =============================================================================
// Description:
//      This file implements the Bank class.
// =============================================================================
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include "utility.h"
#include "Bank.h"

using namespace utility;

Bank::Account::Account() {
    reset();
}

void Bank::Account::reset() {
    id = 0;
    strcpy(name, "UNKNOWN");
    balance = 0.0;
}

bool Bank::Account::setName() {
    // TODO: santize
    string name;
    if (!get(name, "Enter your name: ")) {
        cout << "Failed to get name";
        return false;
    }

    if (name.length() + 1 > MAX_NAME_SIZE) {
        cout << "Name is too long by " << name.length() + 1 - MAX_NAME_SIZE
            << " characters\n";
        return false;
    }

    strcpy(this->name, name.c_str());
    return true;
}

bool Bank::Account::deposit(string promptMsg /*= "How much would you like to deposit? "*/) { // TODO: change to double
    float amount;

    if (!get(amount, promptMsg)) {
        cout << "Error with input\n"; // TODO: what to display?
        return false;
    }

    if (amount <= 0.0) {
        cout << "Cannot deposit $0.00 or less\n";
        return false;
    }
    if (amount > numeric_limits<float>::max() - balance) {
        cout << "Too much money! Can only deposit $" << setprecision(2) << fixed
            << numeric_limits<float>::max() - balance
            << " more. Please open another account.\n";
        return false;
    }

    balance += amount; // TODO: rounding to max when gets too big
    return true;
}

bool Bank::Account::withdraw() {
    float amount;

    if (!get(amount, "How much would you like to withdraw? ")) {
        cout << "Error with input\n"; // TODO: what to display?
        return false;
    }

    if (amount <= 0.0) {
        cout << "Cannot withdraw $0.00 or less\n";
        return false;
    }

    if (balance < amount) {
        cout << "Cannot withdraw more than your balance";
        return false;
    }

    balance -= amount;
    return true;
}

int Bank::Account::getId() {
    return this->id;
}

size_t Bank::Account::getSize() {
    return sizeof(id) + sizeof(name) + sizeof(balance);
}

// TODO: make better
bool Bank::Account::serialize(stringstream &ss) {
    if (!ss) {
        cout << "serialize: buffer error\n";
        return false;
    }

    if (ss.tellp() != 0) {
        cout << "serialize: buffer status error\n";
    }

    ss.write((char*)&id, sizeof(id));
    ss.write((char*)name, sizeof(name));
    ss.write((char*)&balance, sizeof(balance));
    //ss.read(str, getSize());

    return true;
}

bool Bank::Account::deserialize(stringstream &ss) {
    if (!ss) {
        cout << "deserialize: buffer error\n";
        return false;
    }

    if (ss.tellp() != getSize() || ss.tellg() != 0) {
        cout << "deserialize: buffer status error\n";
        return false;
    }

    //ss.write((char*)str, getSize());
    ss.read((char*)&id, sizeof(id));
    ss.read((char*)name, sizeof(name));
    ss.read((char*)&balance, sizeof(balance));

    return true;
}

Bank::Bank(string ra_file_name) : raf(ra_file_name, 
    unique_ptr<Bank::Account>(new Bank::Account())) { }

bool Bank::login() {
    int id;
    if (!get(id, "Enter your id: ")) {
        cout << "Failed to get id\n";
        return false;
    }
    Bank::Account login;
    login.id = id;

    if (!login.setName()){
        return false;
    }

    current_account = unique_ptr<Bank::Account>(new Account());
    if (!raf.getRecord(id, current_account.get())) {
        cout << "Invalid login\n";
        return false;
    }

    if (strcmp(login.name, current_account->name) != 0) {
        cout << "Invalid login\n";
        return false;
    }

    displayBalance();
    return true;
}

bool Bank::createAccount() {
    int id = raf.getNextAvailableId();
    if (id == -1) {
        // TODO: display msg here instead of from raf
        return false;
    }
    current_account = unique_ptr<Bank::Account>(new Bank::Account());
    current_account->id = id;

    if (!current_account->setName()){
        return false;
    }

    current_account->deposit("Enter opening deposit amount: ");

    if (!raf.createRecord(current_account.get())) {
        cout << "Failed to create account\n";
        return false;
    }
    
    cout << "Your id is: " << current_account->id << endl;
    displayBalance();
    return true;
}

bool Bank::closeAccount() {
    cout << "This is the account you are about to close:\n" // display record
        << current_account->id << " " << current_account->name << endl;
    displayBalance();

    char confirm;
    if (!get(confirm, 'n',
        "Are you sure you want to close this account (y/n)? ")) {
        cout << "Error with input\n";
    }

    bool closed = false;
    if (confirm == 'y') {
        closed = raf.deleteRecord(current_account.get());
    }

    if (closed) {
        current_account->reset();
        return true;
    }

    cout << "Your account was not closed\n";
    return false;
}

void Bank::displayBalance() {
    cout << "Your balance is: $" << setprecision(2) << fixed
        << current_account->balance << endl;
}

void Bank::adjustBalance(bool is_deposit) {
    bool failed = true;

    if (is_deposit) {
        failed = current_account->deposit();
    }
    else {
        failed = current_account->withdraw();
    }

    if (failed) {
        raf.updateRecord(current_account.get());
        displayBalance();
    }
}