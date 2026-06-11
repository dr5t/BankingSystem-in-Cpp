#ifndef MODELS_H
#define MODELS_H

#pragma once
#include <string>
#include <sstream>
#include <vector>

namespace Models {
    struct Transaction {
        std::string transactionId;
        std::string fromAccountId;
        std::string toAccountId;
        double amount;
        std::string type;
        std::string timestamp;

        std::string serialize() const {
            std::stringstream ss;
            ss << transactionId << "|" << fromAccountId << "|" << toAccountId << "|" 
               << amount << "|" << type << "|" << timestamp;
            return ss.str();
        }

        static Transaction deserialize(const std::string& line) {
            std::stringstream ss(line);
            std::string item;
            Transaction t;
            std::vector<std::string> tokens;
            while (std::getline(ss, item, '|')) {
                tokens.push_back(item);
            }
            if (tokens.size() >= 6) {
                t.transactionId = tokens[0];
                t.fromAccountId = tokens[1];
                t.toAccountId = tokens[2];
                t.amount = std::stod(tokens[3]);
                t.type = tokens[4];
                t.timestamp = tokens[5];
            }
            return t;
        }
    };

    struct Account {
        std::string accountId;
        std::string customerId;
        std::string type;
        double balance;
        std::string pinHash;
        bool isActive;
        double interestRate;
        std::string fdMaturityDate;

        std::string serialize() const {
            std::stringstream ss;
            ss << accountId << "|" << customerId << "|" << type << "|" << balance << "|"
               << pinHash << "|" << (isActive ? "1" : "0") << "|" << interestRate << "|" << fdMaturityDate;
            return ss.str();
        }

        static Account deserialize(const std::string& line) {
            std::stringstream ss(line);
            std::string item;
            Account a;
            std::vector<std::string> tokens;
            while (std::getline(ss, item, '|')) {
                tokens.push_back(item);
            }
            if (tokens.size() >= 8) {
                a.accountId = tokens[0];
                a.customerId = tokens[1];
                a.type = tokens[2];
                a.balance = std::stod(tokens[3]);
                a.pinHash = tokens[4];
                a.isActive = (tokens[5] == "1");
                a.interestRate = std::stod(tokens[6]);
                a.fdMaturityDate = tokens[7];
            }
            return a;
        }
    };

    struct Customer {
        std::string customerId;
        std::string name;
        std::string email;
        std::string phone;
        std::string passwordHash;
        bool isAdmin;

        std::string serialize() const {
            std::stringstream ss;
            ss << customerId << "|" << name << "|" << email << "|" << phone << "|"
               << passwordHash << "|" << (isAdmin ? "1" : "0");
            return ss.str();
        }

        static Customer deserialize(const std::string& line) {
            std::stringstream ss(line);
            std::string item;
            Customer c;
            std::vector<std::string> tokens;
            while (std::getline(ss, item, '|')) {
                tokens.push_back(item);
            }
            if (tokens.size() >= 6) {
                c.customerId = tokens[0];
                c.name = tokens[1];
                c.email = tokens[2];
                c.phone = tokens[3];
                c.passwordHash = tokens[4];
                c.isAdmin = (tokens[5] == "1");
            }
            return c;
        }
    };
}

#endif
