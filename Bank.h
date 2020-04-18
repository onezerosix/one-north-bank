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
using namespace std;

// === Bank ====================================================================
// This class represents the bank.
// =============================================================================
class Bank {
private:
    RandomAccessFile<Account> raf;

public:
    Bank(string ra_file_name);

    unique_ptr<Account> login();
    unique_ptr<Account> createAccount();
    bool closeAccount(Account &account);

    void displayBalance(Account account);
    void withdraw(Account &account);
    void deposit(Account &account);
};

// === Bank::Bank ==============================================================
// This is the constructor for the Bank class.
//
// Input:
//      ra_file_name [IN]           -- name of the ra file
//
// No Output.
// =============================================================================
Bank::Bank(string ra_file_name) : raf(ra_file_name, Account()) { }

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
unique_ptr<Account> Bank::login() {
    int id;
    if (!get(id, "Enter your id: ")) {
        cout << "Failed to get id\n";
        return nullptr;
    }
    Account login = { id };

    string name;
    if (!get(name, "Enter your name: ")) {
        cout << "Failed to get name";
        return nullptr;
    }
    if (!login.setName(name)){
        return nullptr;
    }

    unique_ptr<Account> account = raf.getRecord(id);
    if (account == nullptr) {
        return account;
    }

    cout << strlen(login.name) << endl << strlen(account->name) << endl;
    cout << login.name << endl << account->name << endl;
    if (strcmp(login.name, account->name) != 0) {
        cout << "Invalid login\n";
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
unique_ptr<Account> Bank::createAccount() {
    unique_ptr<Account> account(new Account());

    string name; // TODO: same as login above
    if (!get(name, "Enter your name: ")) {
        cout << "Failed to get name";
        return nullptr;
    }
    if (!account->setName(name)){
        return nullptr;
    }

    float initial_balance = 0.0;
    if (!get(initial_balance, "Enter opening deposit amount: ")) {
        // TODO; failed input error
        return nullptr;
    }
    account->deposit(initial_balance);

    int id = raf.createRecord(*account);
    // TODO: shouldn't set id here
    account->id = id;
    
    cout << "Your id is: " << account->id << endl;
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

    bool closed = false;
    if (confirm == 'y') {
        closed = raf.deleteRecord(account.id, account);
        account.reset();
    }

    if (closed) {
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
        raf.updateRecord(account.id, account);
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
        raf.updateRecord(account.id, account);
        displayBalance(account);
    }
}

#endif // BANK_H