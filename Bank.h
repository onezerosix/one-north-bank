// =============================================================================
// File: Bank.h
// =============================================================================
// Description:
//      This header file hosts the Bank class for dealing with random access 
//      files for accounts.
// =============================================================================

#ifndef BANK_H
#define BANK_H

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include "Account.h"
#include "Utilities.h"
//#include "Cipher.h" // TODO
using namespace std;

// === Bank ====================================================================
// This class represents the entire bank.
// =============================================================================
class Bank {
private:
    static const int MAX_RECORDS = 100; // TODO: move to be param of contructor
    bitset<MAX_RECORDS> available_ids;// TODO: to_ullong

    string ra_fname;
    fstream ra_file;

    int reserveNextAvailableId();
    void releaseId(Account account);
    void updateFile(Account account, bool update_available_ids = false, 
        bool account_closed = false);
    int calculateOffset(Account account, bool include_available_ids = true);

public:
    Bank(string ra_file_name);

    Account* login();
    Account* createAccount();
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
//      ra_file_name [IN]           -- name of the ra file
//
// No Output.
// =============================================================================
Bank::Bank(string ra_file_name) {
    ra_fname = ra_file_name;
    available_ids.set();

    if (fstream(ra_fname)) { // file already exists
        // TODO: not validating the input here
        ra_file.open(ra_fname, ios::in | ios::binary);
        // if (ra_file.fail()) {
        //     cout << "Error opening file\nExiting\n";
        //     exit(-10); // TODO: change to something better than -10
        // }
        ra_file.read((char*)&available_ids, sizeof(available_ids));
        ra_file.close();
        return;
    }

    ra_file.open(ra_fname, ios::out | ios::binary);
    if (ra_file.fail()) {
        cout << "Error opening file\nExiting\n";
        exit(-10); // TODO: change to something better than -10
    }

    // add the available ids set at beginning of the file
    ra_file.write((char*)&available_ids, sizeof(available_ids));

    // initialize the raf with dummy records
    Account account;
    for (int i = 0; i < MAX_RECORDS; i++) {
        ra_file.write((char*)&account, sizeof(account));
    }
    ra_file.close();
}

// ==== Bank::reserveNextAvailableId ===========================================
// This function gets the next available id & sets it to unavailable.
//
// Input: None
//
// Output:
//      int representing the id that was just reserved
// =============================================================================
int Bank::reserveNextAvailableId() {
    if (available_ids == 0) {
        // TODO: error?
    }

    int id = (int)log2(available_ids.to_ullong());
    available_ids.set(id, false);

    return (id + 1) * 10;
}

// ==== Bank::releaseId ========================================================
// This function sets the id of the account that is being closed to available so
// it can be used for another account.
//
// Input:
//      account [IN]                -- the account that is being closed
//
// Output: None
// =============================================================================
void Bank::releaseId(Account account) { // TODO: check that id is valid?
    available_ids.set(account.id/10 - 1, true);
}

// ==== Bank::updateFile =======================================================
// This function updates the random access file. It's essentially like updating
// a record in the database. It should be the final step of a transaction.
//
// Input:
//      account [IN]                    -- the account that had a transaction 
//                                          performed
//      update_available_ids [OPT IN]   -- optional: boolean indicating whether 
//                                          there a the set of available_ids
//                                          needs to be updated in the RAF.
//                                          defaults to false
//      account_closed [OPT IN]         -- optional: boolean indicating whether
//                                          the account was closed. defaults to
//                                          false
//
// Output: None
// =============================================================================
void Bank::updateFile(Account account, bool update_available_ids /*= false*/,
    bool account_closed /*= false*/) {
    ra_file.open(ra_fname, ios::in | ios::binary);

    // TODO: check if open failed?

    if (update_available_ids) {
        ra_file.write((char*)&available_ids, sizeof(available_ids));
    }
    else {
        ra_file.seekp(sizeof(available_ids), ios::beg);
    }

    // TODO: check that id is valid?
    int byte_offset = calculateOffset(account, false);
    if (account_closed) {
        account.reset();
    }
    
    ra_file.seekp(byte_offset, ios::cur);
    ra_file.write((char*)&account, sizeof(account));
    ra_file.close();
}

// === Bank::calculateOffset ===================================================
// This function calculates where an account should be in the random access
// file.
//
// Input:
//      account [IN]                    -- the account that needs to be located
//      include_available_ids [OPT IN]  -- optional: boolean indicating whether 
//                                          the set at located at the beginning 
//                                          of the RAF should be accounted for 
//                                          in the offset. defaults to true
//
// Output:
//      int representing the offset of the account in bytes
// =============================================================================
int Bank::calculateOffset(Account account, bool include_available_ids /*= true*/
    ) {
    int offset = (account.id/10 - 1) * sizeof(Account);
    if (include_available_ids) {
        offset += sizeof(available_ids);
    }
    return offset;
}

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
Account* Bank::login() {
    int id;
    if (!get(id, "Enter your id: ")) {
        // TODO: failed input error
        return nullptr;
    }
    else if (id < 10 || id > MAX_RECORDS * 10 || id % 10 != 0) {
        // TODO: invalid id error
        return nullptr;
    }
    Account login = { id };

    string name;
    if (!get(name, "Enter your user name: ")) {
        // TODO; failed input error
        return nullptr;
    }
    else if (!login.setName(name)){
        // TODO: error
        return nullptr;
    }

    // TODO: move calculation to dif func?
    int byte_offset = calculateOffset(login);

    Account* account = new Account();
    ra_file.open(ra_fname, ios::in | ios::binary);
    ra_file.seekp(byte_offset, ios::beg);
    ra_file.read((char*)account, sizeof(Account));
    ra_file.close();

    if (strcmp(login.name, account->name) != 0) {
        // TODO: invalid error
        return nullptr;
    }

    displayBalance(*account);
    return account;
}

// ==== Bank::createAccount ====================================================
// This function creates a new account if there is room for one.
//
// Input: None
//
// Output:
//      pointer to the account that was created, otherwise nullptr
// =============================================================================
Account* Bank::createAccount() {
    int id = reserveNextAvailableId();

    Account* account = new Account(id);

    string name; // TODO: same as login above
    if (!get(name, "Enter your user name: ")) {
        // TODO; failed input error
        return nullptr;
    }
    else if (!account->setName(name)){
        // TODO: error
        return nullptr;
    }

    float initial_balance = 0.0;
    if (!get(initial_balance, "Enter opening deposit amount: ")) {
        // TODO; failed input error
        return nullptr;
    }
    account->deposit(initial_balance);

    updateFile(*account, true);
    
    displayBalance(*account);
    return account;
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
bool Bank::closeAccount(Account &account) {
    cout << "This is the account you are about to close:\n" // display record
        << account.id << " " << account.name << endl;
    displayBalance(account);

    char confirm;
    if (!get(confirm, 'n',
        "Are you sure you want to close this account (y/n)? ")) {
        cout << "Error with input\n";
    }
    
    if (confirm == 'y') {
        updateFile(account, true, true);
        account.reset();
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
void Bank::displayBalance(Account account) {
    cout << "Your balance is: $" << setprecision(2) << fixed
        << account.balance << endl;
}

// === Bank::withdraw ==========================================================
// This function withdraws money from an account.
//
// Input:
//      account [IN/OUT]         -- the account to withdraw from
//
// No Output.
// =============================================================================
void Bank::withdraw(Account &account) {
    float amount;

    if (!get(amount, "How much would you like to withdraw? ")) {
        cout << "Error with input\n"; // TODO: what to display?
        return;
    }

    if (account.withdraw(amount)) {
        updateFile(account);
        displayBalance(account);
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
void Bank::deposit(Account &account) {
    float amount;

    if (!get(amount, "How much would you like to deposit? ")) {
        cout << "Error with input\n"; // TODO: what to display?
        return;
    }

    if (account.deposit(amount)) {
        updateFile(account);
        displayBalance(account);
    }
}

#endif // BANK_H