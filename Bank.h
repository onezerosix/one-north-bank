// =============================================================================
// File: Bank.h
// =============================================================================
// Description:
//      This header file hosts the Bank class for dealing with random access 
//      files for accounts.
// =============================================================================

#ifndef BANK_H
#define BANK_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include "Account.h"
//#include "Cipher.h" // TODO
using namespace std;

// === Bank ====================================================================
// This class represents the entire bank.
// =============================================================================
// TODO: access to Account's stuff
class Bank {
private:
    static const int MAX_RECORDS = 100; // TODO: move to be param of contructor
    bitset<MAX_RECORDS> available_ids;// TODO: to_ullong

    string ra_fname;
    fstream ra_file;

    int reserveNextAvailableId();
    bool releaseId(int id);
    void updateFile(Account account);

public:
    Bank(string ra_file_name);

    Account login();
    Account createAccount();
    bool closeAccount(Account &account);

    void displayBalance(Account account);
    void withdraw(Account &account);
    void deposit(Account &account);
};

// === Bank::Bank ==============================================================
// This is the constructor for the Bank class. It creates and initializes the
// raf with n = MAX_RECORDS dummy records if needed or loads the existing raf.
//
// Input:
//      ra_file_name [IN]           - name of the ra file
//
// No Output.
// =============================================================================
Bank::Bank(string ra_file_name) {
    ra_fname = ra_file_name;
    available_ids.set();

    if (fstream(ra_fname)) {
        // TODO: sanitize?
        // get available_ids
        return; // file already exists
    }

    ra_file.open(ra_fname, ios::out | ios::binary);
    if (ra_file.fail()) {
        cout << "Error opening file\nExiting\n";
        exit(-10); // TODO: not -10
    }

    // add the available records set to beginning of the file
    ra_file.write((char*)&available_ids, sizeof(available_ids));

    // initialize the raf with dummy records
    Account account;
    for (int i = 0; i < MAX_RECORDS; i++) {
        ra_file.write((char*)&account, sizeof(account));
    }
    ra_file.close();
}

// =============================================================================
// =============================================================================
int reserveNextAvailableId() {
    if (available_ids == 0) {
        // TODO: error?
    }

    int id = (int)log2(available_ids) + 1;
    available_ids.set(id, false);

    return id * 10;
}

// =============================================================================
// =============================================================================
bool releaseId(int id) {
    available_ids.set(id/10, true);
}

// =============================================================================
// =============================================================================
void Bank::updateFile(Account account, bool updateAvailableIds = false) {
    int byte_offset = (account.id/10 - 1) * sizeof(Account); // TODO: check that id is valid
    
    ra_file.open(ra_fname, ios::in || ios::binary);

    if (updateAvailableIds) {
        ra_file.write((char*)&available_ids, sizeof(account));
    }

    ra_file.seekp(byte_offset, ios::beg);
    ra_file.write((char*)&account, sizeof(account));
    ra_file.close();
}

// =============================================================================
// =============================================================================
Account Bank::login() {
    get(int id, -1, "Enter your id: ");
    get(string name, -1, "Enter your name: ");
    // TODO: santize name & validate id

    // TODO: move calculation to dif func?
    int byte_offset = (id/10 - 1) * sizeof(Account); // TODO: check that id is valid

    Account account;
    ra_file.open(ra_fname, ios::in || ios::binary);
    ra_file.seekp(byte_offset, ios::beg);
    ra_file.read((char*)&account, sizeof(Account));
    ra_file.close();

    // TODO: validate name matches & successful at getting an account

    return account;
}

// =============================================================================
// =============================================================================
// TODO: this
Account Bank::createAccount() {
    int id = reserveNextAvailableId();

    Account account = { id };
    if (account.openAccount) {
        updateFile(account, true);
        return account;
    }
    
    return NULL;
}

// === Bank::closeAccount ======================================================
// This functions closes an account and deletes a record from the raf.
//
// Input:
//      account [IN/OUT]         -- the account that is being closed
//
// Output:
//      true if succeeded in closing the account, otherwise false.
// =============================================================================
bool Bank::closeAccount(Account &account) {
    bool successful;
    if (successful = account.close()) {
        updateFile(account, true);
    }
    return successful;
}

// === Bank::displayAccount ===================================================
// This function displays the information of 1 record.
//
// Input:
//      account [OUT]            -- the account to display the balance of
//
// No Output.
// ============================================================================
void Bank::displayBalance(Account account) {
    //account.displayBalance(); // TODO
    cout << "Your balance is: $" << setprecision(2) << fixed
        << account.balance << endl;
}

// === Bank::withdraw =========================================================
// This function withdraws money from an account.
//
// Input:
//      account [IN/OUT]         -- the account to withdraw from
//
// No Output.
// ============================================================================
void Bank::withdraw(Account &account) {
    if (account.withdraw()) {
        updateFile(account);
    }
}

// === Bank::deposit =========================================================
// This function deposits money to an account.
//
// Input:
//      account [IN/OUT]         -- the account to despoit to
//
// No Output.
// ============================================================================
void Bank::deposit(Account &account) {
    if (account.deposit()) {
        updateFile(account);
    }
}