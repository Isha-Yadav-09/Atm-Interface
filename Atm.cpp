#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <cstring> 

using namespace std;

struct Account {
    int accountNumber;
    string hashedPin;
    double balance;
};

string hashPin(const string& pin) {
    return "hashed_" + pin;
}

vector<Account> loadAccounts(const string& filename) {
    ifstream file(filename, ios::binary);
    vector<Account> accounts;

    if (!file.is_open()) {
        return accounts;
    }

    int count;
    file.read((char*)&count, sizeof(int));
    accounts.resize(count);
    file.read((char*)accounts.data(), sizeof(Account) * count);

    file.close();
    return accounts;
}

void saveAccounts(const vector<Account>& accounts, const string& filename) {
    ofstream file(filename, ios::binary);

    if (!file.is_open()) {
        return;
    }

    int count = accounts.size();
    file.write((char*)&count, sizeof(int));
    file.write((char*)accounts.data(), sizeof(Account) * count);

    file.close();
}

Account* findAccount(vector<Account>& accounts, int accountNumber) {
    for (auto& account : accounts) {
        if (account.accountNumber == accountNumber) {
            return &account;
        }
    }
    return nullptr;
}

bool validatePin(const string& enteredPin, const string& hashedPin) {
    return hashPin(enteredPin) == hashedPin;
}

int getChoice() {
    int choice;
    cout << "\n** ATM Menu **" << endl;
    cout << "1. View Balance" << endl;
    cout << "2. Withdraw Cash" << endl;
    cout << "3. Deposit Cash" << endl;
    cout << "4. Change PIN" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    return choice;
}

double getValidAmount() {
    double amount;
    while (!(cin >> amount) || amount <= 0) {
        cout << "Invalid amount. Please enter a positive value: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return amount;
}

int main() {
    const string accountsFile = "accounts.dat";

    int accountNumber;
    string enteredPin;
    vector<Account> accounts = loadAccounts(accountsFile);
    Account* currentAccount = nullptr;

    cout << "\nWelcome to the ATM!" << endl;

    cout << "Enter your account number: ";
    cin >> accountNumber;

    currentAccount = findAccount(accounts, accountNumber);

    if (currentAccount == nullptr) {
        cout << "This is your first login. Please set your PIN." << endl;
        do {
            cout << "Enter your desired 4-digit PIN: ";
            cin >> enteredPin;
        } while (enteredPin.length() != 4);

        Account newAccount = {accountNumber, hashPin(enteredPin), 0.0};
        accounts.push_back(newAccount);
        saveAccounts(accounts, accountsFile);
        currentAccount = &accounts.back();
        cout << "Your PIN has been set. Please remember it for future logins." << endl;
    }

    do {
        cout << "\nEnter your PIN: ";
        cin >> enteredPin;

        if (validatePin(enteredPin, currentAccount->hashedPin)) {
            break;
        } else {
            cout << "Invalid PIN. Please try again." << endl;
        }
    } while (true);

    int choice;
    do {
        choice = getChoice();
        switch (choice) {
            case 1:
                cout << "\nYour current balance is: Rs." << currentAccount->balance << endl;
                break;
            case 2: {
                double withdrawAmount;
                cout << "\nEnter amount to withdraw: Rs.";
                withdrawAmount = getValidAmount();
                if (withdrawAmount <= currentAccount->balance) {
                    currentAccount->balance -= withdrawAmount;
                    cout << "\nRs." << withdrawAmount << " withdrawn successfully." << endl;
                } else {
                    cout << "\nInsufficient funds. Withdrawal failed." << endl;
                }
                break;
            }
            case 3: {
                double depositAmount;
                cout << "\nEnter amount to deposit: Rs.";
                depositAmount = getValidAmount();
                currentAccount->balance += depositAmount;
                cout << "\nRs." << depositAmount << " deposited successfully." << endl;
                break;
            }
            case 4:
                cout << "\nEnter your current PIN for verification: ";
                cin >> enteredPin;
                if (validatePin(enteredPin, currentAccount->hashedPin)) {
                    do {
                        cout << "Enter your new PIN (4 digits): ";
                        cin >> enteredPin;
                    } while (enteredPin.length() != 4);
                    currentAccount->hashedPin = hashPin(enteredPin);
                    saveAccounts(accounts, accountsFile);
                    cout << "PIN changed successfully." << endl;
                } else {
                    cout << "Invalid PIN. Change failed." << endl;
                }
                break;
            case 5:
                cout << "\nThank you for using the ATM!" << endl;
                saveAccounts(accounts, accountsFile);
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 5);

    return 0;
}
