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
#include "utility.h"
#include "ral.h"
#include "Bank.h"

using namespace std;
using namespace ral;

// === Bank ====================================================================
// This class represents the bank.
// =============================================================================
class Bank {
public:
    // === Bank::Bank ==============================================================
    // This is the constructor for the Bank class.
    //
    // Input:
    //      ra_file_name [IN]           -- name of the ra file
    //
    // No Output.
    // =============================================================================
    Bank(string ra_file_name);

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
    bool login();

    // ==== Bank::createAccount ====================================================
    // This function creates a new account if there is room for one.
    //
    // Input: None
    //
    // Output:
    //      pointer to the account that was created, otherwise nullptr
    // =============================================================================
    bool createAccount();

    // === Bank::closeAccount ======================================================
    // This functions closes an account and deletes a record from the raf.
    //
    // Input:
    //      account [IN/OUT]         -- the account that is being closed
    //
    // Output:
    //      true if succeeded in closing the account, otherwise false
    // =============================================================================
    bool closeAccount();

    // === Bank::displayAccount ====================================================
    // This function displays the balance of an account.
    //
    // Input:
    //      account [IN]             -- the account to display the balance of
    //
    // No Output.
    // =============================================================================
    void displayBalance();

    // === Bank::withdraw ==========================================================
    // This function withdraws money from an account.
    //
    // Input:
    //      account [IN/OUT]         -- the account to withdraw from
    //
    // No Output.
    // =============================================================================
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
        Account();

        // === Account::reset ==========================================================
        // This function sets the variables to {0, "UNKNOWN", 0.0}.
        //
        // Input: None
        //
        // Output: None
        // =============================================================================
        void reset();

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
        bool setName();

        // === Account::deposit ========================================================
        // This function deposits an amount to an account.
        //
        // Input:
        //      amount [IN]             -- the amount to deposit
        //
        // Output:
        //      true if the deposit was successful, otherwise false
        // =============================================================================
        bool deposit(string promptMsg = "How much would you like to deposit? ");

        // === Account::withdraw =======================================================
        // This function withdraws an amount from an account.
        //
        // Input:
        //      amount [IN]             -- the amount to withdraw
        //
        // Output:
        //      true if the withdrawal was successful, otherwise false
        // =============================================================================
        bool withdraw();

        // === Account::getId ==========================================================
        // This function get the id of the account.
        //
        // Input: None
        //
        // Output:
        //      the id of the account
        // =============================================================================
        int getId() override;

        size_t getSize() override;

        bool serialize(stringstream &ss) override;
        bool deserialize(stringstream &ss) override;
    };

    File raf;
    unique_ptr<Bank::Account> current_account; // TODO: validate logged in?
    // TODO: logout function?
};

#endif // BANK_H