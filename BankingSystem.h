#ifndef BANKING_SYSTEM_H
#define BANKING_SYSTEM_H

#pragma once
#include "Models.h"
#include "Database.h"
#include "Security.h"
#include "UIHelper.h"
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

inline std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* ltm = std::localtime(&now);
    std::stringstream ss;
    ss << std::put_time(ltm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

class BankingSystem {
private:
    std::vector<Models::Customer> customers;
    std::vector<Models::Account> accounts;
    std::vector<Models::Transaction> transactions;

    Models::Customer currentCustomer;
    bool loggedIn;

    std::string generateCustomerId() {
        int maxId = 1000;
        for (const auto& c : customers) {
            if (c.customerId.length() > 1 && c.customerId[0] == 'C') {
                try {
                    int num = std::stoi(c.customerId.substr(1));
                    if (num > maxId) maxId = num;
                } catch (...) {}
            }
        }
        return "C" + std::to_string(maxId + 1);
    }

    std::string generateAccountId() {
        long long maxAccNum = 1000000000;
        for (const auto& a : accounts) {
            try {
                long long num = std::stoll(a.accountId);
                if (num > maxAccNum) maxAccNum = num;
            } catch (...) {}
        }
        return std::to_string(maxAccNum + 1);
    }

    std::string generateTransactionId() {
        long long maxTxId = 500000;
        for (const auto& t : transactions) {
            if (t.transactionId.length() > 2 && t.transactionId.substr(0, 2) == "TX") {
                try {
                    long long num = std::stoll(t.transactionId.substr(2));
                    if (num > maxTxId) maxTxId = num;
                } catch (...) {}
            }
        }
        return "TX" + std::to_string(maxTxId + 1);
    }

public:
    BankingSystem() {
        customers = Database::loadCustomers();
        accounts = Database::loadAccounts();
        transactions = Database::loadTransactions();
        loggedIn = false;
    }

    void saveData() {
        Database::saveCustomers(customers);
        Database::saveAccounts(accounts);
        Database::saveTransactions(transactions);
    }

    bool isLoggedIn() const { return loggedIn; }
    Models::Customer getCurrentCustomer() const { return currentCustomer; }

    bool registerCustomer(std::string name, std::string email, std::string phone, std::string password, Models::Customer& outCustomer) {
        if (name.empty() || email.empty() || phone.empty() || password.empty()) return false;
        
        for (const auto& c : customers) {
            if (c.email == email) return false;
        }

        outCustomer.customerId = generateCustomerId();
        outCustomer.name = name;
        outCustomer.email = email;
        outCustomer.phone = phone;
        outCustomer.passwordHash = Security::hashPassword(password);
        outCustomer.isAdmin = false;

        customers.push_back(outCustomer);
        saveData();
        return true;
    }

    bool loginCustomer(std::string email, std::string password) {
        std::string hashed = Security::hashPassword(password);
        for (const auto& c : customers) {
            if (c.email == email && c.passwordHash == hashed) {
                currentCustomer = c;
                loggedIn = true;
                return true;
            }
        }
        return false;
    }

    void logout() {
        loggedIn = false;
        currentCustomer = Models::Customer();
    }

    bool createAccount(std::string customerId, std::string type, double initialDeposit, std::string pin, Models::Account& outAccount) {
        if (initialDeposit < 0.0 || pin.length() != 4) return false;

        outAccount.accountId = generateAccountId();
        outAccount.customerId = customerId;
        outAccount.type = type;
        outAccount.balance = initialDeposit;
        outAccount.pinHash = Security::hashPassword(pin);
        outAccount.isActive = true;

        if (type == "SAVINGS") {
            outAccount.interestRate = 4.0;
            outAccount.fdMaturityDate = "-";
        } else if (type == "CHECKING") {
            outAccount.interestRate = 0.0;
            outAccount.fdMaturityDate = "-";
        } else if (type == "FIXED_DEPOSIT") {
            outAccount.interestRate = 7.5;
            std::time_t now = std::time(nullptr);
            std::tm* ltm = std::localtime(&now);
            ltm->tm_year += 1;
            std::time_t oneYearLater = std::mktime(ltm);
            std::tm* mtm = std::localtime(&oneYearLater);
            std::stringstream ss;
            ss << std::put_time(mtm, "%Y-%m-%d");
            outAccount.fdMaturityDate = ss.str();
        } else {
            return false;
        }

        accounts.push_back(outAccount);

        Models::Transaction t;
        t.transactionId = generateTransactionId();
        t.fromAccountId = (type == "FIXED_DEPOSIT" ? "SAVINGS_TRANSFER" : "CASH_COUNTER");
        t.toAccountId = outAccount.accountId;
        t.amount = initialDeposit;
        t.type = (type == "FIXED_DEPOSIT" ? "FD_CREATED" : "DEPOSIT");
        t.timestamp = getCurrentTimestamp();
        transactions.push_back(t);

        saveData();
        return true;
    }

    bool deposit(std::string accountId, double amount) {
        if (amount <= 0) return false;
        for (auto& a : accounts) {
            if (a.accountId == accountId) {
                if (!a.isActive) return false;
                a.balance += amount;

                Models::Transaction t;
                t.transactionId = generateTransactionId();
                t.fromAccountId = "CASH_COUNTER";
                t.toAccountId = accountId;
                t.amount = amount;
                t.type = "DEPOSIT";
                t.timestamp = getCurrentTimestamp();
                transactions.push_back(t);

                saveData();
                return true;
            }
        }
        return false;
    }

    bool withdraw(std::string accountId, double amount, std::string pin) {
        if (amount <= 0) return false;
        std::string hashedPin = Security::hashPassword(pin);
        for (auto& a : accounts) {
            if (a.accountId == accountId) {
                if (!a.isActive) return false;
                if (a.pinHash != hashedPin) return false;
                if (a.balance < amount) return false;

                a.balance -= amount;

                Models::Transaction t;
                t.transactionId = generateTransactionId();
                t.fromAccountId = accountId;
                t.toAccountId = "CASH_DISPENSER";
                t.amount = amount;
                t.type = "WITHDRAWAL";
                t.timestamp = getCurrentTimestamp();
                transactions.push_back(t);

                saveData();
                return true;
            }
        }
        return false;
    }

    bool transfer(std::string fromAccountId, std::string toAccountId, double amount, std::string pin) {
        if (amount <= 0 || fromAccountId == toAccountId) return false;
        std::string hashedPin = Security::hashPassword(pin);

        auto senderIt = accounts.end();
        auto receiverIt = accounts.end();

        for (auto it = accounts.begin(); it != accounts.end(); ++it) {
            if (it->accountId == fromAccountId) senderIt = it;
            if (it->accountId == toAccountId) receiverIt = it;
        }

        if (senderIt == accounts.end() || receiverIt == accounts.end()) return false;
        if (!senderIt->isActive || !receiverIt->isActive) return false;
        if (senderIt->pinHash != hashedPin) return false;
        if (senderIt->balance < amount) return false;

        senderIt->balance -= amount;
        receiverIt->balance += amount;

        Models::Transaction t;
        t.transactionId = generateTransactionId();
        t.fromAccountId = fromAccountId;
        t.toAccountId = toAccountId;
        t.amount = amount;
        t.type = "TRANSFER";
        t.timestamp = getCurrentTimestamp();
        transactions.push_back(t);

        saveData();
        return true;
    }

    std::vector<Models::Account> getCustomerAccounts(std::string customerId) const {
        std::vector<Models::Account> results;
        for (const auto& a : accounts) {
            if (a.customerId == customerId) {
                results.push_back(a);
            }
        }
        return results;
    }

    std::vector<Models::Transaction> getAccountTransactions(std::string accountId) const {
        std::vector<Models::Transaction> results;
        for (const auto& t : transactions) {
            if (t.fromAccountId == accountId || t.toAccountId == accountId) {
                results.push_back(t);
            }
        }
        std::reverse(results.begin(), results.end());
        return results;
    }

    std::vector<Models::Customer> getAllCustomers() const { return customers; }
    std::vector<Models::Account> getAllAccounts() const { return accounts; }
    std::vector<Models::Transaction> getAllTransactions() const { return transactions; }

    bool toggleAccountStatus(std::string accountId) {
        for (auto& a : accounts) {
            if (a.accountId == accountId) {
                a.isActive = !a.isActive;
                saveData();
                return true;
            }
        }
        return false;
    }

    bool updateCustomerProfile(std::string customerId, std::string name, std::string email, std::string phone) {
        for (auto& c : customers) {
            if (c.customerId == customerId) {
                c.name = name;
                c.email = email;
                c.phone = phone;
                saveData();
                return true;
            }
        }
        return false;
    }

    bool addInterestToSavings() {
        bool added = false;
        for (auto& a : accounts) {
            if (a.isActive && a.type == "SAVINGS" && a.balance > 0) {
                double interest = a.balance * (a.interestRate / 12.0 / 100.0);
                if (interest > 0.01) {
                    a.balance += interest;

                    Models::Transaction t;
                    t.transactionId = generateTransactionId();
                    t.fromAccountId = "RESERVE_BANK";
                    t.toAccountId = a.accountId;
                    t.amount = interest;
                    t.type = "INTEREST";
                    t.timestamp = getCurrentTimestamp();
                    transactions.push_back(t);
                    added = true;
                }
            }
        }
        if (added) {
            saveData();
        }
        return added;
    }

    static double calculateCompoundInterest(double principal, double rate, double time, int n) {
        return principal * std::pow(1.0 + (rate / 100.0 / n), n * time);
    }

    static bool checkLoanEligibility(double monthlyIncome, double existingDebt, double loanAmount, double& maxEligibleLoan, std::string& reason) {
        double dti = (existingDebt / monthlyIncome) * 100.0;
        if (dti > 45.0) {
            reason = "Debt-to-Income (DTI) ratio is too high (" + std::to_string(static_cast<int>(dti)) + "%). Maximum allowed is 45%.";
            maxEligibleLoan = 0.0;
            return false;
        }

        double freeIncome = monthlyIncome - existingDebt;
        double maxAllowedMonthlyEMI = freeIncome * 0.35;
        
        double monthlyRate = 8.0 / 12.0 / 100.0;
        int months = 60;
        
        double factor = (monthlyRate * std::pow(1.0 + monthlyRate, months)) / (std::pow(1.0 + monthlyRate, months) - 1.0);
        maxEligibleLoan = maxAllowedMonthlyEMI / factor;

        if (loanAmount > maxEligibleLoan) {
            reason = "Requested loan exceeds maximum eligible capacity. Maximum capacity is $" + std::to_string(static_cast<int>(maxEligibleLoan)) + ".";
            return false;
        }

        reason = "Congratulations! You are eligible for the requested loan.";
        return true;
    }
};

#endif
