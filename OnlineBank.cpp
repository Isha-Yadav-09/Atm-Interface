#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>

using namespace std;

class Account {
public:
    int accountNumber;
    string name;
    double balance;
    int pin;  // New member variable for storing PIN

    // Constructor for creating a new account
    Account(int accountNumber, const string &name, int pin, double initialDeposit = 0.0) {
        this->accountNumber = accountNumber;
        this->name = name;
        this->pin = pin;
        this->balance = initialDeposit;
    }

    // Function to deposit funds
    void deposit(double amount) {
        balance += amount;
        cout << "Deposit successful! New balance: $" << balance << endl;
    }

    // Function to withdraw funds (with input validation)
    bool withdraw(double amount) {
        if (amount > balance) {
            cout << "Insufficient funds. Withdrawal failed." << endl;
            return false;
        }
        balance -= amount;
        cout << "Withdrawal successful! New balance: $" << balance << endl;
        return true;
    }

    // Save account to file
    void saveToFile(ofstream &outFile) const {
        outFile << accountNumber << '\n' << name << '\n' << pin << '\n' << balance << '\n';
    }

    // Load account from file
    static Account loadFromFile(ifstream &inFile) {
        int accNumber;
        string accName;
        int accPin;
        double accBalance;
        inFile >> accNumber;
        inFile.ignore();
        getline(inFile, accName);
        inFile >> accPin >> accBalance;
        return Account(accNumber, accName, accPin, accBalance);
    }

    // Function to change PIN
    void changePin(int newPin) {
        pin = newPin;
        cout << "PIN changed successfully!" << endl;
    }

    // Function to validate PIN
    bool validatePin(int inputPin) const {
        return pin == inputPin;
    }
};

vector<Account> accounts; // Vector to store all accounts

// Function to validate user input (account number)
int validateAccountNumber() {
    int accountNumber;
    while (!(cin >> accountNumber)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a valid account number: ";
    }
    return accountNumber;
}

// Function to validate user input (amount)
double validateAmount() {
    double amount;
    while (!(cin >> amount) || amount < 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a positive amount: ";
    }
    return amount;
}

// Function to validate user input (PIN)
int validatePin() {
    int pin;
    while (!(cin >> pin) || pin < 1000 || pin > 9999) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a 4-digit PIN: ";
    }
    return pin;
}

// Function to find an account by account number
Account* findAccount(int accountNumber) {
    for (auto &account : accounts) {
        if (account.accountNumber == accountNumber) {
            return &account;
        }
    }
    return nullptr;
}

// Function to load accounts from file
void loadAccounts(const string &filename) {
    ifstream inFile(filename);
    if (inFile.is_open()) {
        while (inFile.peek() != EOF) {
            accounts.push_back(Account::loadFromFile(inFile));
        }
        inFile.close();
        cout << "Accounts loaded successfully.\n";
    } else {
        cout << "No existing account file found. Starting fresh.\n";
    }
}

// Function to save accounts to file
void saveAccounts(const string &filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        for (const auto &account : accounts) {
            account.saveToFile(outFile);
        }
        outFile.close();
        cout << "Accounts saved successfully.\n";
    } else {
        cerr << "Failed to save accounts.\n";
    }
}

int main() {
    const string filename = "accounts.txt";
    loadAccounts(filename);

    int choice, accountNumber, pin;
    double amount;
    string name;

    while (true) {
        cout << "\nWelcome to Online Banking System!" << endl;
        cout << "1. Create Account" << endl;
        cout << "2. Deposit Funds" << endl;
        cout << "3. Withdraw Funds" << endl;
        cout << "4. Transfer Funds" << endl;
        cout << "5. Change PIN" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter your choice: ";

        choice = validateAccountNumber();

        switch (choice) {
            case 1: {
                cout << "Enter your name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter a 4-digit PIN: ";
                pin = validatePin();
                int newAccountNumber = accounts.size() + 1; // Generate unique account number
                accounts.emplace_back(newAccountNumber, name, pin);
                cout << "Account created successfully! Your account number is: " << newAccountNumber << endl;
                break;
            }
            case 2: {
                cout << "Enter your account number: ";
                accountNumber = validateAccountNumber();
                Account *account = findAccount(accountNumber);
                if (account) {
                    cout << "Enter your PIN: ";
                    pin = validatePin();
                    if (account->validatePin(pin)) {
                        cout << "Enter amount to deposit: ";
                        amount = validateAmount();
                        account->deposit(amount);
                    } else {
                        cout << "Incorrect PIN." << endl;
                    }
                } else {
                    cout << "Account not found." << endl;
                }
                break;
            }
            case 3: {
                cout << "Enter your account number: ";
                accountNumber = validateAccountNumber();
                Account *account = findAccount(accountNumber);
                if (account) {
                    cout << "Enter your PIN: ";
                    pin = validatePin();
                    if (account->validatePin(pin)) {
                        cout << "Enter amount to withdraw: ";
                        amount = validateAmount();
                        account->withdraw(amount);
                    } else {
                        cout << "Incorrect PIN." << endl;
                    }
                } else {
                    cout << "Account not found." << endl;
                }
                break;
            }
            case 4: {
                cout << "Enter your account number (source): ";
                int sourceAccountNumber = validateAccountNumber();
                Account *sourceAccount = findAccount(sourceAccountNumber);
                if (!sourceAccount) {
                    cout << "Source account not found." << endl;
                    break;
                }

                cout << "Enter your PIN: ";
                pin = validatePin();
                if (!sourceAccount->validatePin(pin)) {
                    cout << "Incorrect PIN." << endl;
                    break;
                }

                cout << "Enter destination account number: ";
                int destinationAccountNumber = validateAccountNumber();
                Account *destinationAccount = findAccount(destinationAccountNumber);
                if (!destinationAccount) {
                    cout << "Destination account not found." << endl;
                    break;
                }

                cout << "Enter transfer amount: ";
                amount = validateAmount();

                // Check if source account has sufficient funds
                if (sourceAccount->balance < amount) {
                    cout << "Insufficient funds in source account. Transfer failed." << endl;
                    break;
                }

                // Perform transfer (debit source, credit destination)
                sourceAccount->withdraw(amount);
                destinationAccount->deposit(amount);
                cout << "Transfer successful!" << endl;
                break;
            }
            case 5: {
                cout << "Enter your account number: ";
                accountNumber = validateAccountNumber();
                Account *account = findAccount(accountNumber);
                if (account) {
                    cout << "Enter your current PIN: ";
                    int currentPin = validatePin();
                    if (account->validatePin(currentPin)) {
                        cout << "Enter new PIN: ";
                        int newPin = validatePin();
                        account->changePin(newPin);
                    } else {
                        cout << "Incorrect PIN." << endl;
                    }
                } else {
                    cout << "Account not found." << endl;
                }
                break;
            }
            case 6:
                saveAccounts(filename);
                cout << "Exiting Online Banking System." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}
