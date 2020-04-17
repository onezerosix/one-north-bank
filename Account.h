// =============================================================================
// File: Account.h
// =============================================================================
// Description:
//      This header file hosts the Account class.
// =============================================================================

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <cstring>
#include <iostream>
using namespace std;

// === Account =================================================================
// This class represents one account.
// =============================================================================
class Account {
private:
    friend class Bank; // TODO: shouldn't be allowd to directly set variables

    static const int MAX_NAME_SIZE = 100;

    int id;
    char name[MAX_NAME_SIZE]; // account holder's name
    float balance;

    Account();
    Account(int id);
    // ~Account(); // TODO: this due to Bank using new?

    void reset();
    bool setName(string new_name);
    bool deposit(float amount);
    bool withdraw(float amount);
};

// === Account::Account ========================================================
// This is the constructor for the Account class.
//
// Input: None
//
// Output: None
// =============================================================================
Account::Account() {
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
Account::Account(int id) {
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
void Account::reset() {
    id = 0;
    strcpy(name, "UNKNOWN");
    balance = 0.0;
}

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
bool Account::setName(string new_name) {
    // TODO: santize
    // TODO: check length
    // TODO: set the name
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
bool Account::deposit(float amount) {
    // TODO: check for overflow
    
    balance += amount;
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
bool Account::withdraw(float amount) {
    if (amount == 0.0) {
        cout << "Cannot withdraw $0.00\n"; // TODO: handling the errors..
        return false;
    }

    if (balance < amount) {
        cout << "Cannot withdraw more than your balance";
        return false;
    }

    balance -= amount;
    return true;
}

#endif // ACCOUNT_H