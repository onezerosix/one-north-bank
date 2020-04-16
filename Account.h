// =============================================================================
// File: Account.h
// =============================================================================
// Description:
//      This header file hosts the Account class.
// =============================================================================

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string.h>

// === Account =================================================================
// This class represents one account.
// =============================================================================
class Account {
    friend class Bank;

    static const int MAX_NAME_SIZE = 100;

    int id;
    char user_name[MAX_NAME_SIZE]; // account holder's name
    float balance;

    // === Account =============================================================
    // This is the constructor for the Account struct.
    //
    // Input: None
    //
    // Output: None
    // =========================================================================
    // TODO: can we make constructor private?
    Account() {
        reset();
    }

    Account(int id) {
        // TODO: set..
    }

    // === initialize ==========================================================
    // This function sets the variables to {0, "UNKNOWN", 0.0}.
    //
    // Input: None
    //
    // Output: None
    // =========================================================================
    void reset() {
        id = 0;
        strcpy(user_name, "UNKNOWN");
        balance = 0.0;
    }

    // TODO: deposit & withdraw
    // TODO: close account
    // TODO: setName

}; // end of Account class

#endif // ACCOUNT_H

/*
int calculateOffset(int id) {
    if (id < 10 || id > MAX_RECORDS || id % 10 != 0) {
        cout << "Invalid id\n"; // TODO: exit?
        return -1;
    }
    else {
        return (id/10 - 1) * sizeof(Account);
    }
}

void withdraw() {
    // TODO: the display..
    // display record before change and prompt for the amount to be added/sub
    cout << "This is your account:\n"
        << account.id << " " << account.name
        << " " << account.balance << endl;

    if (!get(int amount, 0, "How much would you like to withdraw? ")) { // TODO: this allowed?
        cout << "Error with input\n"; // TODO: what to display?
    }
    else if (account.balance < amount) {
        // TODO can't withdraw that amount
    }
    else {
        account.balance -= amount;
        cout << "This is your new balance: " << setprecision(2) << fixed 
            << account.balance << endl;

        updateFile(account);
    }
}

void closeAccount() {
    char confirm;

    cout << "This is your account you are about to close:\n" // display record
        << account.id << " " << account.name << " "
        << setprecision(2) << fixed << account.balance << endl
        << "Are you sure you want to close this account (y/n)? "; // confirm

    if (!Get(confirm))
        cout << "Error with input\n";

    else if (confirm == 'y') {
        account.reset(); // replace record with dummy record
        ra_file.seekp(byte_offset, ios::beg);
        ra_file.write((char*)&account, sizeof(account));
    }

    ra_file.close();
    return;
}

bool openAccount() {
    // TODO: errors?
    if (!account.setName()) {
        return NULL;
    }
    if (!account.deposit()) {
        return NULL;
    }

    // TODO: display id to user
}
*/