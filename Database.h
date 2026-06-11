#ifndef DATABASE_H
#define DATABASE_H

#pragma once
#include "Models.h"
#include "Security.h"
#include <vector>
#include <fstream>
#include <iostream>

namespace Database {
    const std::string CUSTOMER_FILE = "customers.txt";
    const std::string ACCOUNT_FILE = "accounts.txt";
    const std::string TRANSACTION_FILE = "transactions.txt";

    inline void saveCustomers(const std::vector<Models::Customer>& customers) {
        std::ofstream outfile(CUSTOMER_FILE, std::ios::trunc);
        if (!outfile.is_open()) return;
        for (const auto& c : customers) {
            outfile << c.serialize() << "\n";
        }
    }

    inline std::vector<Models::Customer> loadCustomers() {
        std::vector<Models::Customer> customers;
        std::ifstream infile(CUSTOMER_FILE);
        if (!infile.is_open()) {
            Models::Customer admin;
            admin.customerId = "C1000";
            admin.name = "Admin Portal";
            admin.email = "admin@bank.com";
            admin.phone = "+1000000000";
            admin.passwordHash = Security::hashPassword("admin");
            admin.isAdmin = true;
            customers.push_back(admin);
            saveCustomers(customers);
            return customers;
        }

        std::string line;
        while (std::getline(infile, line)) {
            if (!line.empty()) {
                customers.push_back(Models::Customer::deserialize(line));
            }
        }

        if (customers.empty()) {
            Models::Customer admin;
            admin.customerId = "C1000";
            admin.name = "Admin Portal";
            admin.email = "admin@bank.com";
            admin.phone = "+1000000000";
            admin.passwordHash = Security::hashPassword("admin");
            admin.isAdmin = true;
            customers.push_back(admin);
            saveCustomers(customers);
        }
        return customers;
    }

    inline void saveAccounts(const std::vector<Models::Account>& accounts) {
        std::ofstream outfile(ACCOUNT_FILE, std::ios::trunc);
        if (!outfile.is_open()) return;
        for (const auto& a : accounts) {
            outfile << a.serialize() << "\n";
        }
    }

    inline std::vector<Models::Account> loadAccounts() {
        std::vector<Models::Account> accounts;
        std::ifstream infile(ACCOUNT_FILE);
        if (!infile.is_open()) return accounts;
        std::string line;
        while (std::getline(infile, line)) {
            if (!line.empty()) {
                accounts.push_back(Models::Account::deserialize(line));
            }
        }
        return accounts;
    }

    inline void saveTransactions(const std::vector<Models::Transaction>& transactions) {
        std::ofstream outfile(TRANSACTION_FILE, std::ios::trunc);
        if (!outfile.is_open()) return;
        for (const auto& t : transactions) {
            outfile << t.serialize() << "\n";
        }
    }

    inline std::vector<Models::Transaction> loadTransactions() {
        std::vector<Models::Transaction> transactions;
        std::ifstream infile(TRANSACTION_FILE);
        if (!infile.is_open()) return transactions;
        std::string line;
        while (std::getline(infile, line)) {
            if (!line.empty()) {
                transactions.push_back(Models::Transaction::deserialize(line));
            }
        }
        return transactions;
    }
}

#endif
