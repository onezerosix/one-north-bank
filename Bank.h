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
    //static Account dummy_record(); // TODO: static?
    RandomAccessFile raf;
    unique_ptr<Account> current_account; // TODO: validate logged in?
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

// === Bank::Bank ==============================================================
// This is the constructor for the Bank class.
//
// Input:
//      ra_file_name [IN]           -- name of the ra file
//
// No Output.
// =============================================================================
Bank::Bank(string ra_file_name) : raf(ra_file_name, 
    unique_ptr<Account>(new Account())) { }

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
    Account login(id);

    string name;
    if (!get(name, "Enter your name: ")) {
        cout << "Failed to get name";
        return false;
    }
    if (!login.setName(name)){
        return false;
    }

    current_account = unique_ptr<Account>(new Account());
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
    current_account = unique_ptr<Account>(new Account(id));

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