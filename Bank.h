// =============================================================================
// File: Bank.h
// =============================================================================
// Description:
//      This header file hosts the Bank class.
// =============================================================================

#ifndef BANK_H
#define BANK_H

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include <memory>
#include <limits>
#include "Utilities.h"
#include "RandomAccessFile.h"
//#include "Cipher.h" // TODO
using namespace std; // TODO: remove since this file is a header
using namespace ral;

// === Bank ====================================================================
// This class represents the bank.
// =============================================================================
class Bank {
public:
    Bank(string ra_file_name);

    bool login();
    bool createAccount();
    bool closeAccount();

    void displayBalance();
    void adjustBalance(bool is_deposit);

private:
    // === Account =================================================================
    // This class represents one account.
    // =============================================================================
    class Account : public Record {
    public:

        static const int MAX_NAME_SIZE = 100;

        int id;
        char name[MAX_NAME_SIZE]; // account holder's name (null terminated)
        float balance;

        // === Account::Account ========================================================
        // This is the constructor for the Account class.
        //
        // Input: None
        //
        // Output: None
        // =============================================================================
        Account() { reset(); }

        // === Account::reset ==========================================================
        // This function sets the variables to {0, "UNKNOWN", 0.0}.
        //
        // Input: None
        //
        // Output: None
        // =============================================================================
        void reset() {
            id = 0;
            strcpy(name, "UNKNOWN");
            balance = 0.0;
        }

        bool setName();
        bool deposit(string promptMsg = "How much would you like to deposit? ");
        bool withdraw();

        // === Account::getId ==========================================================
        // This function get the id of the account.
        //
        // Input: None
        //
        // Output:
        //      the id of the account
        // =============================================================================
        int getId() override { return this->id; }

        size_t getSize() override { return sizeof(id) + sizeof(name) + sizeof(balance); }

        // TODO: make better
        bool serialize(stringstream &ss) override {
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
        bool deserialize(stringstream &ss) override {
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
    };

    File raf;
    unique_ptr<Bank::Account> current_account; // TODO: validate logged in?
    // TODO: logout function?
};

// TODO; docs
// === Account::setName ========================================================
// This function sets the name of the account after santiziing it & checking the
// length.
//
// Input:
//      name [IN]               -- what to set the account name to
//
// Output:
//      true if name was valid & account was updated, otherwise false
// =============================================================================
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

// TODO: change
// === Account::deposit ========================================================
// This function deposits an amount to an account.
//
// Input:
//      amount [IN]             -- the amount to deposit
//
// Output:
//      true if the deposit was successful, otherwise false
// =============================================================================
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

// TODO: update
// === Account::withdraw =======================================================
// This function withdraws an amount from an account.
//
// Input:
//      amount [IN]             -- the amount to withdraw
//
// Output:
//      true if the withdrawal was successful, otherwise false
// =============================================================================
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

// === Bank::Bank ==============================================================
// This is the constructor for the Bank class.
//
// Input:
//      ra_file_name [IN]           -- name of the ra file
//
// No Output.
// =============================================================================
Bank::Bank(string ra_file_name) : raf(ra_file_name, 
    unique_ptr<Bank::Account>(new Bank::Account())) { }

// ==== Bank::login ============================================================
// This function logs a user into the bank by attempting to locate the user's
// account.
//
// Input: None
//
// Output:
//      pointer to account of the user if it was able to be located, otherwise
//      nullptr
// =============================================================================
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

// ==== Bank::createAccount ====================================================
// This function creates a new account if there is room for one.
//
// Input: None
//
// Output:
//      pointer to the account that was created, otherwise nullptr
// =============================================================================
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

// === Bank::closeAccount ======================================================
// This functions closes an account and deletes a record from the raf.
//
// Input:
//      account [IN/OUT]         -- the account that is being closed
//
// Output:
//      true if succeeded in closing the account, otherwise false
// =============================================================================
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

// === Bank::displayAccount ====================================================
// This function displays the balance of an account.
//
// Input:
//      account [IN]             -- the account to display the balance of
//
// No Output.
// =============================================================================
void Bank::displayBalance() {
    cout << "Your balance is: $" << setprecision(2) << fixed
        << current_account->balance << endl;
}

// TODO: update
// === Bank::withdraw ==========================================================
// This function withdraws money from an account.
//
// Input:
//      account [IN/OUT]         -- the account to withdraw from
//
// No Output.
// =============================================================================
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

#endif // BANK_H