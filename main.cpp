#include "BankingSystem.h"
#include "UIHelper.h"
#include "Security.h"
#include <iostream>
#include <limits>
#include <string>
#include <vector>

void clearBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int getIntInput(const std::string& prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            clearBuffer();
            return val;
        } else {
            clearBuffer();
            UI::printColored("Invalid input! Please enter a valid number.", UI::NEON_PINK);
        }
    }
}

double getDoubleInput(const std::string& prompt) {
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            clearBuffer();
            return val;
        } else {
            clearBuffer();
            UI::printColored("Invalid amount! Please enter a valid numeric value.", UI::NEON_PINK);
        }
    }
}

std::string getStringInput(const std::string& prompt) {
    std::string s;
    std::cout << prompt;
    std::getline(std::cin, s);
    return s;
}

void pressEnterToContinue() {
    std::cout << "\n" << UI::rgbColor(UI::GRAY) << "Press Enter to continue..." << UI::RESET;
    std::cin.get();
}

void runCalculators() {
    while (true) {
        UI::clearScreen();
        UI::drawBoxHeader("FINANCIAL ANALYTICAL SUITE", UI::NEON_PURPLE, UI::NEON_CYAN);
        UI::printColored(" 1. Compound Interest Calculator", UI::WHITE);
        UI::printColored(" 2. Loan Eligibility Checker", UI::WHITE);
        UI::printColored(" 3. Return to Portal", UI::NEON_PINK);
        UI::drawBoxFooter(UI::NEON_PURPLE);

        int choice = getIntInput("Select option (1-3): ");
        if (choice == 3) break;

        if (choice == 1) {
            UI::clearScreen();
            UI::drawBoxHeader("COMPOUND INTEREST CALCULATOR", UI::NEON_CYAN, UI::NEON_GREEN);
            double principal = getDoubleInput("Enter principal amount ($): ");
            double rate = getDoubleInput("Enter annual interest rate (%): ");
            double years = getDoubleInput("Enter investment term (years): ");
            int freq = getIntInput("Compounding frequency (1=Annually, 4=Quarterly, 12=Monthly): ");
            
            if (principal < 0 || rate < 0 || years < 0 || freq <= 0) {
                UI::printColored("Error: Invalid calculation arguments.", UI::NEON_PINK);
            } else {
                double total = BankingSystem::calculateCompoundInterest(principal, rate, years, freq);
                double interest = total - principal;
                
                UI::drawCardDivider(UI::NEON_CYAN);
                UI::drawCardLine("Principal Amount:", UI::formatCurrency(principal), UI::NEON_CYAN, UI::WHITE, UI::NEON_GREEN);
                UI::drawCardLine("Total Interest Earned:", UI::formatCurrency(interest), UI::NEON_CYAN, UI::WHITE, UI::NEON_YELLOW);
                UI::drawCardLine("Maturity Value:", UI::formatCurrency(total), UI::NEON_CYAN, UI::WHITE, UI::NEON_CYAN);
            }
            UI::drawBoxFooter(UI::NEON_CYAN);
            pressEnterToContinue();
        } else if (choice == 2) {
            UI::clearScreen();
            UI::drawBoxHeader("LOAN ELIGIBILITY CHECKER", UI::NEON_CYAN, UI::NEON_GREEN);
            double income = getDoubleInput("Enter monthly gross income ($): ");
            double debt = getDoubleInput("Enter current monthly debt EMIs ($): ");
            double amount = getDoubleInput("Enter requested loan amount ($): ");
            
            if (income <= 0 || debt < 0 || amount <= 0) {
                UI::printColored("Error: Input values must be greater than zero.", UI::NEON_PINK);
            } else {
                double maxLoan = 0;
                std::string reason;
                bool eligible = BankingSystem::checkLoanEligibility(income, debt, amount, maxLoan, reason);
                
                UI::drawCardDivider(UI::NEON_CYAN);
                if (eligible) {
                    UI::printColored(" STATUS: APPROVED", UI::NEON_GREEN);
                    UI::drawCardLine("Maximum Loan Eligibility:", UI::formatCurrency(maxLoan), UI::NEON_CYAN, UI::WHITE, UI::NEON_GREEN);
                } else {
                    UI::printColored(" STATUS: REJECTED", UI::NEON_PINK);
                    UI::drawCardLine("Maximum Loan Capacity:", UI::formatCurrency(maxLoan), UI::NEON_CYAN, UI::WHITE, UI::NEON_YELLOW);
                }
                UI::printColored("\n Reason: " + reason, UI::WHITE);
            }
            UI::drawBoxFooter(UI::NEON_CYAN);
            pressEnterToContinue();
        }
    }
}

void renderCustomerDashboard(const BankingSystem& sys) {
    auto accounts = sys.getCustomerAccounts(sys.getCurrentCustomer().customerId);
    UI::drawBoxHeader("ACCOUNT DASHBOARD SUMMARY", UI::NEON_CYAN, UI::NEON_GREEN, 70);
    
    if (accounts.empty()) {
        UI::drawCardLine("No active accounts found.", "Go to option 1 to open one.", UI::NEON_CYAN, UI::NEON_PINK, UI::WHITE, 70);
    } else {
        double totalBalance = 0;
        for (const auto& acc : accounts) {
            std::string typeStr = acc.type;
            if (!acc.isActive) typeStr += " (FROZEN)";
            std::string balStr = UI::formatCurrency(acc.balance);
            UI::drawCardLine("Acc " + acc.accountId + " [" + typeStr + "]", balStr, UI::NEON_CYAN, UI::WHITE, (acc.isActive ? UI::NEON_GREEN : UI::GRAY), 70);
            if (acc.isActive) totalBalance += acc.balance;
        }
        UI::drawCardDivider(UI::NEON_CYAN, 70);
        UI::drawCardLine("TOTAL LIQUIDITY:", UI::formatCurrency(totalBalance), UI::NEON_CYAN, UI::NEON_YELLOW, UI::NEON_CYAN, 70);
    }
    UI::drawBoxFooter(UI::NEON_CYAN, 70);
}

void runCustomerPortal(BankingSystem& sys) {
    while (true) {
        UI::clearScreen();
        UI::logo();
        UI::printGradient("Welcome back, " + sys.getCurrentCustomer().name, UI::NEON_CYAN, UI::NEON_PINK);
        std::cout << "\n";
        
        renderCustomerDashboard(sys);
        std::cout << "\n";

        UI::drawBoxHeader("CUSTOMER TRANSACTION MENU", UI::NEON_PINK, UI::NEON_CYAN, 70);
        UI::printColored(" 1. Open New Bank Account (Savings/Checking/FD)", UI::WHITE);
        UI::printColored(" 2. Deposit Funds", UI::WHITE);
        UI::printColored(" 3. Withdraw Funds", UI::WHITE);
        UI::printColored(" 4. Transfer Funds (Intra-Bank)", UI::WHITE);
        UI::printColored(" 5. View Account Details Card", UI::WHITE);
        UI::printColored(" 6. Transaction History Logs", UI::WHITE);
        UI::printColored(" 7. Update Contact Profiles", UI::WHITE);
        UI::printColored(" 8. Financial Analytical Suite (Calculators)", UI::WHITE);
        UI::printColored(" 9. Safe Logout", UI::NEON_PINK);
        UI::drawBoxFooter(UI::NEON_PINK, 70);

        int choice = getIntInput("Select action (1-9): ");
        if (choice == 9) {
            sys.logout();
            UI::showLoadingBar("Logging out securely...", UI::NEON_PINK);
            break;
        }

        switch (choice) {
            case 1: {
                UI::clearScreen();
                UI::drawBoxHeader("OPEN NEW ACCOUNT PORTAL", UI::NEON_CYAN, UI::NEON_GREEN, 60);
                UI::printColored("Available Account Types:", UI::NEON_YELLOW);
                UI::printColored(" - SAVINGS      (4.0% Compound Interest)", UI::WHITE);
                UI::printColored(" - CHECKING     (Standard Checking Account)", UI::WHITE);
                UI::printColored(" - FIXED_DEPOSIT (7.5% Interest, 1-Year Term)", UI::WHITE);
                UI::drawCardDivider(UI::NEON_CYAN, 60);
                
                std::string type = getStringInput("Enter account type: ");
                std::transform(type.begin(), type.end(), type.begin(), ::toupper);
                
                if (type != "SAVINGS" && type != "CHECKING" && type != "FIXED_DEPOSIT") {
                    UI::printColored("Error: Invalid account selection.", UI::NEON_PINK);
                    UI::drawBoxFooter(UI::NEON_CYAN, 60);
                    pressEnterToContinue();
                    break;
                }

                double depositAmt = getDoubleInput("Enter initial deposit amount ($): ");
                if (depositAmt < 0) {
                    UI::printColored("Error: Initial deposit cannot be negative.", UI::NEON_PINK);
                    UI::drawBoxFooter(UI::NEON_CYAN, 60);
                    pressEnterToContinue();
                    break;
                }

                std::string pin = getStringInput("Set a 4-digit Transaction PIN: ");
                if (pin.length() != 4 || !std::all_of(pin.begin(), pin.end(), ::isdigit)) {
                    UI::printColored("Error: PIN must be exactly 4 digits.", UI::NEON_PINK);
                    UI::drawBoxFooter(UI::NEON_CYAN, 60);
                    pressEnterToContinue();
                    break;
                }

                Models::Account outAcc;
                if (sys.createAccount(sys.getCurrentCustomer().customerId, type, depositAmt, pin, outAcc)) {
                    UI::showLoadingBar("Provisioning account credentials...", UI::NEON_GREEN);
                    UI::printColored("Account Created Successfully!", UI::NEON_GREEN);
                    UI::drawCardLine("Account Number:", outAcc.accountId, UI::NEON_CYAN, UI::WHITE, UI::NEON_CYAN, 60);
                    UI::drawCardLine("Type:", outAcc.type, UI::NEON_CYAN, UI::WHITE, UI::NEON_YELLOW, 60);
                    UI::drawCardLine("Maturity Date:", outAcc.fdMaturityDate, UI::NEON_CYAN, UI::WHITE, UI::NEON_GREEN, 60);
                } else {
                    UI::printColored("Error: Failed to register account.", UI::NEON_PINK);
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 60);
                pressEnterToContinue();
                break;
            }
            case 2: {
                UI::clearScreen();
                UI::drawBoxHeader("CASH DEPOSIT SERVICE", UI::NEON_CYAN, UI::NEON_GREEN, 60);
                std::string accId = getStringInput("Enter Target Account Number: ");
                double amount = getDoubleInput("Enter Deposit Amount ($): ");
                
                UI::showLoadingBar("Processing cash deposit...", UI::NEON_CYAN);
                if (sys.deposit(accId, amount)) {
                    UI::printColored("Transaction Completed! Cash deposited successfully.", UI::NEON_GREEN);
                } else {
                    UI::printColored("Error: Transaction failed. Check account number and status.", UI::NEON_PINK);
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 60);
                pressEnterToContinue();
                break;
            }
            case 3: {
                UI::clearScreen();
                UI::drawBoxHeader("SECURE ATM WITHDRAWAL", UI::NEON_CYAN, UI::NEON_GREEN, 60);
                std::string accId = getStringInput("Enter Account Number: ");
                double amount = getDoubleInput("Enter Withdrawal Amount ($): ");
                std::cout << "Enter 4-digit PIN: ";
                std::string pin = Security::getMaskedInput();

                UI::showLoadingBar("Validating secure transaction...", UI::NEON_CYAN);
                if (sys.withdraw(accId, amount, pin)) {
                    UI::printColored("Transaction Completed! Cash dispensed successfully.", UI::NEON_GREEN);
                } else {
                    UI::printColored("Error: Transaction rejected. Invalid credentials or insufficient funds.", UI::NEON_PINK);
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 60);
                pressEnterToContinue();
                break;
            }
            case 4: {
                UI::clearScreen();
                UI::drawBoxHeader("INTRA-BANK TRANSFER INITIATOR", UI::NEON_CYAN, UI::NEON_GREEN, 60);
                std::string fromAcc = getStringInput("Enter Source Account Number: ");
                std::string toAcc = getStringInput("Enter Destination Account Number: ");
                double amount = getDoubleInput("Enter Transfer Amount ($): ");
                std::cout << "Enter 4-digit PIN: ";
                std::string pin = Security::getMaskedInput();

                UI::showLoadingBar("routing wire transfer...", UI::NEON_CYAN);
                if (sys.transfer(fromAcc, toAcc, amount, pin)) {
                    UI::printColored("Transfer Completed! Funds routed successfully.", UI::NEON_GREEN);
                } else {
                    UI::printColored("Error: Wire transfer failed. Check details, PIN and balances.", UI::NEON_PINK);
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 60);
                pressEnterToContinue();
                break;
            }
            case 5: {
                UI::clearScreen();
                UI::drawBoxHeader("DETAILED ACCOUNT INFORMATION", UI::NEON_CYAN, UI::NEON_GREEN, 65);
                std::string accId = getStringInput("Enter Account Number to view: ");
                auto accounts = sys.getCustomerAccounts(sys.getCurrentCustomer().customerId);
                
                auto it = std::find_if(accounts.begin(), accounts.end(), [&](const Models::Account& a){
                    return a.accountId == accId;
                });

                if (it != accounts.end()) {
                    UI::drawCardDivider(UI::NEON_CYAN, 65);
                    UI::drawCardLine("Account ID:", it->accountId, UI::NEON_CYAN, UI::WHITE, UI::NEON_CYAN, 65);
                    UI::drawCardLine("Holder Customer ID:", it->customerId, UI::NEON_CYAN, UI::WHITE, UI::WHITE, 65);
                    UI::drawCardLine("Product Type:", it->type, UI::NEON_CYAN, UI::WHITE, UI::NEON_YELLOW, 65);
                    UI::drawCardLine("Current Balance:", UI::formatCurrency(it->balance), UI::NEON_CYAN, UI::WHITE, UI::NEON_GREEN, 65);
                    UI::drawCardLine("Applied Interest Rate:", std::to_string(it->interestRate) + "% p.a.", UI::NEON_CYAN, UI::WHITE, UI::NEON_CYAN, 65);
                    UI::drawCardLine("Product Maturity Date:", it->fdMaturityDate, UI::NEON_CYAN, UI::WHITE, UI::WHITE, 65);
                    UI::drawCardLine("Operational Status:", (it->isActive ? "ACTIVE" : "FROZEN"), UI::NEON_CYAN, UI::WHITE, (it->isActive ? UI::NEON_GREEN : UI::NEON_PINK), 65);
                } else {
                    UI::printColored("Error: Account not found or unauthorized access.", UI::NEON_PINK);
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 65);
                pressEnterToContinue();
                break;
            }
            case 6: {
                UI::clearScreen();
                UI::drawBoxHeader("TRANSACTION HISTORICAL LOGS", UI::NEON_CYAN, UI::NEON_GREEN, 75);
                std::string accId = getStringInput("Enter Account Number: ");
                
                auto customerAccounts = sys.getCustomerAccounts(sys.getCurrentCustomer().customerId);
                auto authCheck = std::find_if(customerAccounts.begin(), customerAccounts.end(), [&](const Models::Account& a){
                    return a.accountId == accId;
                });

                if (authCheck != customerAccounts.end()) {
                    auto txs = sys.getAccountTransactions(accId);
                    if (txs.empty()) {
                        UI::printColored(" No transactions recorded for this account.", UI::NEON_YELLOW);
                    } else {
                        std::cout << UI::rgbColor(UI::NEON_CYAN) 
                                  << " │ " << std::left << std::setw(10) << "TX ID"
                                  << " │ " << std::left << std::setw(12) << "TYPE"
                                  << " │ " << std::left << std::setw(12) << "FROM"
                                  << " │ " << std::left << std::setw(12) << "TO"
                                  << " │ " << std::left << std::setw(10) << "AMOUNT"
                                  << " │ " << std::left << std::setw(20) << "TIMESTAMP"
                                  << " │\n" << UI::RESET;
                        UI::drawCardDivider(UI::NEON_CYAN, 75);

                        for (const auto& tx : txs) {
                            UI::Color txColor = UI::WHITE;
                            if (tx.type == "DEPOSIT") txColor = UI::NEON_GREEN;
                            else if (tx.type == "WITHDRAWAL") txColor = UI::NEON_PINK;
                            else if (tx.type == "TRANSFER") txColor = UI::NEON_YELLOW;
                            else if (tx.type == "FD_CREATED") txColor = UI::NEON_CYAN;
                            
                            std::cout << UI::rgbColor(UI::NEON_CYAN) << " │ " << UI::RESET
                                      << UI::rgbColor(txColor) << std::left << std::setw(10) << tx.transactionId << UI::RESET
                                      << UI::rgbColor(UI::NEON_CYAN) << " │ " << UI::RESET
                                      << UI::rgbColor(txColor) << std::left << std::setw(12) << tx.type << UI::RESET
                                      << UI::rgbColor(UI::NEON_CYAN) << " │ " << UI::RESET
                                      << std::left << std::setw(12) << (tx.fromAccountId.length() > 10 ? tx.fromAccountId.substr(0,10) : tx.fromAccountId)
                                      << UI::rgbColor(UI::NEON_CYAN) << " │ " << UI::RESET
                                      << std::left << std::setw(12) << (tx.toAccountId.length() > 10 ? tx.toAccountId.substr(0,10) : tx.toAccountId)
                                      << UI::rgbColor(UI::NEON_CYAN) << " │ " << UI::RESET
                                      << UI::rgbColor(txColor) << std::left << std::setw(10) << UI::formatCurrency(tx.amount) << UI::RESET
                                      << UI::rgbColor(UI::NEON_CYAN) << " │ " << UI::RESET
                                      << std::left << std::setw(20) << tx.timestamp
                                      << UI::rgbColor(UI::NEON_CYAN) << " │\n" << UI::RESET;
                        }
                    }
                } else {
                    UI::printColored("Error: Account not found or access denied.", UI::NEON_PINK);
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 75);
                pressEnterToContinue();
                break;
            }
            case 7: {
                UI::clearScreen();
                UI::drawBoxHeader("UPDATE CONTACT INFORMATION", UI::NEON_CYAN, UI::NEON_GREEN, 60);
                std::string newName = getStringInput("Enter New Name: ");
                std::string newEmail = getStringInput("Enter New Email: ");
                std::string newPhone = getStringInput("Enter New Phone: ");

                if (newName.empty() || !Security::isValidEmail(newEmail) || !Security::isValidPhone(newPhone)) {
                    UI::printColored("Error: Profile fields contain invalid email/phone structure.", UI::NEON_PINK);
                } else {
                    UI::showLoadingBar("Applying profile updates...", UI::NEON_CYAN);
                    if (sys.updateCustomerProfile(sys.getCurrentCustomer().customerId, newName, newEmail, newPhone)) {
                        UI::printColored("Profile updated successfully!", UI::NEON_GREEN);
                    } else {
                        UI::printColored("Error: Failed to save changes.", UI::NEON_PINK);
                    }
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 60);
                pressEnterToContinue();
                break;
            }
            case 8: {
                runCalculators();
                break;
            }
        }
    }
}

void runAdminPortal(BankingSystem& sys) {
    while (true) {
        UI::clearScreen();
        UI::logo();
        UI::printGradient("ADMINISTRATIVE BACKEND CONTROLLER", UI::NEON_PURPLE, UI::NEON_CYAN);
        std::cout << "\n";

        UI::drawBoxHeader("MANAGEMENT MENU OPTIONS", UI::NEON_PURPLE, UI::NEON_CYAN, 65);
        UI::printColored(" 1. Create/Register New Customer File", UI::WHITE);
        UI::printColored(" 2. Provision New Bank Account", UI::WHITE);
        UI::printColored(" 3. View All Registered Customer Records", UI::WHITE);
        UI::printColored(" 4. View All Active & Frozen Accounts", UI::WHITE);
        UI::printColored(" 5. Toggle Account Operations (Freeze/Unfreeze)", UI::WHITE);
        UI::printColored(" 6. Apply Accrued Monthly Savings Interest", UI::WHITE);
        UI::printColored(" 7. View Global Audit Transaction Log", UI::WHITE);
        UI::printColored(" 8. Administrative Logout", UI::NEON_PINK);
        UI::drawBoxFooter(UI::NEON_PURPLE, 65);

        int choice = getIntInput("Select administrative option (1-8): ");
        if (choice == 8) {
            sys.logout();
            UI::showLoadingBar("Terminating admin session...", UI::NEON_PINK);
            break;
        }

        switch (choice) {
            case 1: {
                UI::clearScreen();
                UI::drawBoxHeader("REGISTER NEW CUSTOMER PROFILE", UI::NEON_CYAN, UI::NEON_GREEN, 60);
                std::string name = getStringInput("Enter full name: ");
                std::string email = getStringInput("Enter email address: ");
                std::string phone = getStringInput("Enter phone number: ");
                std::string pw = getStringInput("Set account access password: ");

                if (name.empty() || !Security::isValidEmail(email) || !Security::isValidPhone(phone) || pw.empty()) {
                    UI::printColored("Error: Customer fields contain invalid configurations.", UI::NEON_PINK);
                } else {
                    Models::Customer newCust;
                    if (sys.registerCustomer(name, email, phone, pw, newCust)) {
                        UI::showLoadingBar("Writing registry metadata...", UI::NEON_GREEN);
                        UI::printColored("Customer Registration Completed!", UI::NEON_GREEN);
                        UI::drawCardLine("Customer ID Assigned:", newCust.customerId, UI::NEON_CYAN, UI::WHITE, UI::NEON_CYAN, 60);
                        UI::drawCardLine("Assigned Email:", newCust.email, UI::NEON_CYAN, UI::WHITE, UI::WHITE, 60);
                    } else {
                        UI::printColored("Error: Registration failed. Email might already exist.", UI::NEON_PINK);
                    }
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 60);
                pressEnterToContinue();
                break;
            }
            case 2: {
                UI::clearScreen();
                UI::drawBoxHeader("PROVISION CUSTOMER ACCOUNT", UI::NEON_CYAN, UI::NEON_GREEN, 60);
                std::string custId = getStringInput("Enter Target Customer ID: ");
                std::string type = getStringInput("Enter Account Type (SAVINGS/CHECKING/FIXED_DEPOSIT): ");
                std::transform(type.begin(), type.end(), type.begin(), ::toupper);
                
                if (type != "SAVINGS" && type != "CHECKING" && type != "FIXED_DEPOSIT") {
                    UI::printColored("Error: Invalid account selection.", UI::NEON_PINK);
                    UI::drawBoxFooter(UI::NEON_CYAN, 60);
                    pressEnterToContinue();
                    break;
                }

                double initialDep = getDoubleInput("Enter Initial Deposit ($): ");
                std::string pin = getStringInput("Set 4-digit Transaction PIN: ");

                if (initialDep < 0 || pin.length() != 4 || !std::all_of(pin.begin(), pin.end(), ::isdigit)) {
                    UI::printColored("Error: Invalid inputs for account creation.", UI::NEON_PINK);
                } else {
                    Models::Account outAcc;
                    if (sys.createAccount(custId, type, initialDep, pin, outAcc)) {
                        UI::printColored("Account created successfully!", UI::NEON_GREEN);
                        UI::drawCardLine("Assigned Acc Number:", outAcc.accountId, UI::NEON_CYAN, UI::WHITE, UI::NEON_CYAN, 60);
                    } else {
                        UI::printColored("Error: Account creation rejected.", UI::NEON_PINK);
                    }
                }
                UI::drawBoxFooter(UI::NEON_CYAN, 60);
                pressEnterToContinue();
                break;
            }
            case 3: {
                UI::clearScreen();
                UI::drawBoxHeader("REGISTERED CUSTOMERS REGISTRY", UI::NEON_PURPLE, UI::NEON_CYAN, 70);
                auto custs = sys.getAllCustomers();
                for (const auto& c : custs) {
                    std::string info = c.name + " (" + c.email + ")";
                    UI::drawCardLine(c.customerId + (c.isAdmin ? " [ADMIN]" : ""), info, UI::NEON_PURPLE, UI::WHITE, UI::NEON_CYAN, 70);
                }
                UI::drawBoxFooter(UI::NEON_PURPLE, 70);
                pressEnterToContinue();
                break;
            }
            case 4: {
                UI::clearScreen();
                UI::drawBoxHeader("GLOBAL BANK ACCOUNT REGISTRY", UI::NEON_PURPLE, UI::NEON_CYAN, 70);
                auto accs = sys.getAllAccounts();
                for (const auto& a : accs) {
                    std::string statusStr = (a.isActive ? "ACTIVE" : "FROZEN");
                    std::string info = a.type + " - Bal: " + UI::formatCurrency(a.balance) + " - (" + statusStr + ")";
                    UI::drawCardLine("Acc " + a.accountId + " (Cust " + a.customerId + ")", info, UI::NEON_PURPLE, UI::WHITE, (a.isActive ? UI::NEON_GREEN : UI::NEON_PINK), 70);
                }
                UI::drawBoxFooter(UI::NEON_PURPLE, 70);
                pressEnterToContinue();
                break;
            }
            case 5: {
                UI::clearScreen();
                UI::drawBoxHeader("FREEZE/UNFREEZE CONTROLLER", UI::NEON_PURPLE, UI::NEON_CYAN, 60);
                std::string accId = getStringInput("Enter Target Account Number: ");
                UI::showLoadingBar("Altering system permission maps...", UI::NEON_PURPLE);
                if (sys.toggleAccountStatus(accId)) {
                    UI::printColored("Operational status toggled successfully!", UI::NEON_GREEN);
                } else {
                    UI::printColored("Error: Account record not found.", UI::NEON_PINK);
                }
                UI::drawBoxFooter(UI::NEON_PURPLE, 60);
                pressEnterToContinue();
                break;
            }
            case 6: {
                UI::clearScreen();
                UI::drawBoxHeader("SAVINGS INTEREST DISPERSAL UNIT", UI::NEON_PURPLE, UI::NEON_CYAN, 60);
                UI::showLoadingBar("Accruing savings accounts interest yields...", UI::NEON_GREEN);
                if (sys.addInterestToSavings()) {
                    UI::printColored("Interest dispersed successfully to all active Savings accounts!", UI::NEON_GREEN);
                } else {
                    UI::printColored("Interest dispersal run: No interest paid. (No active savings balances found).", UI::NEON_YELLOW);
                }
                UI::drawBoxFooter(UI::NEON_PURPLE, 60);
                pressEnterToContinue();
                break;
            }
            case 7: {
                UI::clearScreen();
                UI::drawBoxHeader("GLOBAL TRANSACTION AUDIT HISTORY", UI::NEON_PURPLE, UI::NEON_CYAN, 75);
                auto txs = sys.getAllTransactions();
                if (txs.empty()) {
                    UI::printColored("No historical transactions recorded in database.", UI::NEON_YELLOW);
                } else {
                    std::cout << UI::rgbColor(UI::NEON_PURPLE) 
                              << " │ " << std::left << std::setw(10) << "TX ID"
                              << " │ " << std::left << std::setw(12) << "TYPE"
                              << " │ " << std::left << std::setw(12) << "FROM"
                              << " │ " << std::left << std::setw(12) << "TO"
                              << " │ " << std::left << std::setw(10) << "AMOUNT"
                              << " │ " << std::left << std::setw(20) << "TIMESTAMP"
                              << " │\n" << UI::RESET;
                    UI::drawCardDivider(UI::NEON_PURPLE, 75);

                    for (const auto& tx : txs) {
                        UI::Color txColor = UI::WHITE;
                        if (tx.type == "DEPOSIT") txColor = UI::NEON_GREEN;
                        else if (tx.type == "WITHDRAWAL") txColor = UI::NEON_PINK;
                        else if (tx.type == "TRANSFER") txColor = UI::NEON_YELLOW;
                        else if (tx.type == "FD_CREATED") txColor = UI::NEON_CYAN;

                        std::cout << UI::rgbColor(UI::NEON_PURPLE) << " │ " << UI::RESET
                                  << UI::rgbColor(txColor) << std::left << std::setw(10) << tx.transactionId << UI::RESET
                                  << UI::rgbColor(UI::NEON_PURPLE) << " │ " << UI::RESET
                                  << UI::rgbColor(txColor) << std::left << std::setw(12) << tx.type << UI::RESET
                                  << UI::rgbColor(UI::NEON_PURPLE) << " │ " << UI::RESET
                                  << std::left << std::setw(12) << (tx.fromAccountId.length() > 10 ? tx.fromAccountId.substr(0,10) : tx.fromAccountId)
                                  << UI::rgbColor(UI::NEON_PURPLE) << " │ " << UI::RESET
                                  << std::left << std::setw(12) << (tx.toAccountId.length() > 10 ? tx.toAccountId.substr(0,10) : tx.toAccountId)
                                  << UI::rgbColor(UI::NEON_PURPLE) << " │ " << UI::RESET
                                  << UI::rgbColor(txColor) << std::left << std::setw(10) << UI::formatCurrency(tx.amount) << UI::RESET
                                  << UI::rgbColor(UI::NEON_PURPLE) << " │ " << UI::RESET
                                  << std::left << std::setw(20) << tx.timestamp
                                  << UI::rgbColor(UI::NEON_PURPLE) << " │\n" << UI::RESET;
                    }
                }
                UI::drawBoxFooter(UI::NEON_PURPLE, 75);
                pressEnterToContinue();
                break;
            }
        }
    }
}

int main() {
    BankingSystem sys;
    
    while (true) {
        UI::clearScreen();
        UI::logo();
        
        UI::drawBoxHeader("CENTRAL TERMINAL INTERFACE", UI::NEON_CYAN, UI::NEON_CYAN);
        UI::printColored(" 1. Sign In to Profile", UI::WHITE);
        UI::printColored(" 2. Register New Customer Account", UI::WHITE);
        UI::printColored(" 3. Terminate Connection", UI::NEON_PINK);
        UI::drawBoxFooter(UI::NEON_CYAN);

        int choice = getIntInput("Select channel (1-3): ");
        if (choice == 3) {
            UI::clearScreen();
            UI::typeText("Closing session. Secure mainframe connection terminated.", UI::NEON_PINK, 15);
            break;
        }

        if (choice == 1) {
            UI::clearScreen();
            UI::drawBoxHeader("SIGN IN MODULE", UI::NEON_CYAN, UI::NEON_GREEN);
            std::string email = getStringInput("Enter account email: ");
            std::cout << "Enter account password: ";
            std::string pw = Security::getMaskedInput();

            UI::showLoadingBar("Authenticating digital signature...", UI::NEON_CYAN);
            if (sys.loginCustomer(email, pw)) {
                UI::printColored("Login Approved. Opening console...", UI::NEON_GREEN);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                
                if (sys.getCurrentCustomer().isAdmin) {
                    runAdminPortal(sys);
                } else {
                    runCustomerPortal(sys);
                }
            } else {
                UI::printColored("Login Rejected. Check email address and password.", UI::NEON_PINK);
                UI::drawBoxFooter(UI::NEON_CYAN);
                pressEnterToContinue();
            }
        } else if (choice == 2) {
            UI::clearScreen();
            UI::drawBoxHeader("REGISTER PROFILE MODULE", UI::NEON_CYAN, UI::NEON_GREEN);
            std::string name = getStringInput("Enter your full name: ");
            std::string email = getStringInput("Enter your email address: ");
            std::string phone = getStringInput("Enter your phone number: ");
            std::string pw = getStringInput("Create account password: ");

            if (name.empty() || !Security::isValidEmail(email) || !Security::isValidPhone(phone) || pw.empty()) {
                UI::printColored("Registration failed: Invalid fields or passwords.", UI::NEON_PINK);
                UI::drawBoxFooter(UI::NEON_CYAN);
                pressEnterToContinue();
                continue;
            }

            Models::Customer outCust;
            UI::showLoadingBar("Writing registry metadata...", UI::NEON_CYAN);
            if (sys.registerCustomer(name, email, phone, pw, outCust)) {
                UI::printColored("Registration successful! You may now sign in.", UI::NEON_GREEN);
                UI::drawCardLine("Customer ID Assigned:", outCust.customerId, UI::NEON_CYAN, UI::WHITE, UI::NEON_CYAN);
            } else {
                UI::printColored("Registration failed. Email address may already be in use.", UI::NEON_PINK);
            }
            UI::drawBoxFooter(UI::NEON_CYAN);
            pressEnterToContinue();
        }
    }
    return 0;
}
