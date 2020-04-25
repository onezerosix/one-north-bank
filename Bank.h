// =============================================================================
// File: Bank.h
// =============================================================================
// Description:
//      This header file hosts the Bank class.
// =============================================================================

#ifndef BANK_H
#define BANK_H

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include <memory>
#include <limits>
#include "Account.h"
#include "Utilities.h"
#include "RandomAccessFile.h"
//#include "Cipher.h" // TODO
using namespace std; // TODO: remove since this file is a header

// === Bank ====================================================================
// This class represents the bank.
// =============================================================================
class Bank {
private:
    // === Account =================================================================
    // This class represents one account.
    // =============================================================================
    class Account : public RandomAccessFileRecord {
    public:

        static const int MAX_NAME_SIZE = 100;

        int id;
        char name[MAX_NAME_SIZE]; // account holder's name (null terminated)
        float balance;

        Account();
        Account(int id);

        void reset();
        bool setName(string new_name);
        bool deposit(float amount);
        bool withdraw(float amount);

        int getId() override;

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

    RandomAccessFile raf;
    unique_ptr<Bank::Account> current_account; // TODO: validate logged in?
    // TODO: logout function?

public:
    Bank(string ra_file_name);

    bool login();
    bool createAccount();
    bool closeAccount();

    void displayBalance();
    void withdraw();
    void deposit();
};

// === Account::Account ========================================================
// This is the constructor for the Account class.
//
// Input: None
//
// Output: None
// =============================================================================
Bank::Account::Account() {
    reset();
}

// === Account::Account ========================================================
// This is the second constructor for the Account class.
//
// Input:
//      id [IN]                 -- the id to give the account
//
// Output: None
// =============================================================================
Bank::Account::Account(int id) {
    reset();
    this->id = id;
}


// === Account::reset ==========================================================
// This function sets the variables to {0, "UNKNOWN", 0.0}.
//
// Input: None
//
// Output: None
// =============================================================================
void Bank::Account::reset() {
    id = 0;
    strcpy(name, "UNKNOWN");
    balance = 0.0;
}

// === Account::getId ==========================================================
// This function get the id of the account.
//
// Input: None
//
// Output:
//      the id of the account
// =============================================================================
int Bank::Account::getId() { return this->id; }

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
bool Bank::Account::setName(string new_name) {
    // TODO: santize
    if (new_name.length() + 1 > MAX_NAME_SIZE) {
        cout << "Name is too long by " << new_name.length() + 1 - MAX_NAME_SIZE
            << " characters\n";
        return false;
    }

    strcpy(name, new_name.c_str());
    return true;
}

// === Account::deposit ========================================================
// This function deposits an amount to an account.
//
// Input:
//      amount [IN]             -- the amount to deposit
//
// Output:
//      true if the deposit was successful, otherwise false
// =============================================================================
bool Bank::Account::deposit(float amount) { // TODO: change to double
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

// === Account::withdraw =======================================================
// This function withdraws an amount from an account.
//
// Input:
//      amount [IN]             -- the amount to withdraw
//
// Output:
//      true if the withdrawal was successful, otherwise false
// =============================================================================
bool Bank::Account::withdraw(float amount) {
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
    Bank::Account login(id);

    string name;
    if (!get(name, "Enter your name: ")) {
        cout << "Failed to get name";
        return false;
    }
    if (!login.setName(name)){
        return false;
    }

    current_account = unique_ptr<Bank::Account>(new Bank::Account());
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
    current_account = unique_ptr<Bank::Account>(new Bank::Account(id));

    string name; // TODO: same as login above
    if (!get(name, "Enter your name: ")) {
        cout << "Failed to get name\n";
        return false;
    }
    if (!current_account->setName(name)){
        return false;
    }

    float initial_balance = 0.0;
    if (!get(initial_balance, "Enter opening deposit amount: ")) {
        cout << "Failed to get deposit amount\n";
        return false;
    }
    current_account->deposit(initial_balance);

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

// === Bank::withdraw ==========================================================
// This function withdraws money from an account.
//
// Input:
//      account [IN/OUT]         -- the account to withdraw from
//
// No Output.
// =============================================================================
void Bank::withdraw() {
    float amount;

    if (!get(amount, "How much would you like to withdraw? ")) {
        cout << "Error with input\n"; // TODO: what to display?
        return;
    }

    if (current_account->withdraw(amount)) {
        raf.updateRecord(current_account.get());
        displayBalance();
    }
}

// === Bank::deposit ===========================================================
// This function deposits money to an account.
//
// Input:
//      account [IN/OUT]         -- the account to despoit to
//
// No Output.
// =============================================================================
void Bank::deposit() {
    float amount;

    if (!get(amount, "How much would you like to deposit? ")) {
        cout << "Error with input\n"; // TODO: what to display?
        return;
    }

    if (current_account->deposit(amount)) {
        raf.updateRecord(current_account.get());
        displayBalance();
    }
}

#endif // BANK_H