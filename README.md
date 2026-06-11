# 💳 Futuristic C++ Console Banking System

A state-of-the-art C++ Terminal-based banking portal designed with **neon/gradient visual themes**, **typing effects**, **custom animations**, and secure cryptographic implementations. The application features multi-profile sign-in, transaction PIN controls, and full text-based database file storage.

---

## ✨ Key Features & Visual Aesthetics

### 🌈 Visual & Typography Effects
- **True Color Gradients**: Terminal-wide 24-bit RGB gradient titles.
- **Neon Color Themes**: Distinct aesthetic colors for different console segments (Cyan, Pink, Green, Purple, and Yellow).
- **Animations**: Typing animations for menus and console dialogs, combined with a smooth-filling loading progress bar.
- **Grids & UI Containers**: High-fidelity double-lined box models (`║`, `═`, `╔`, `╗`, `╚`, `╝`) for rendering bank statements and dashboards.

### 🔒 Security Framework
- **Password Protection**: Plain-text passwords are encrypted using a customized header-only **SHA-256** implementation before file writes.
- **Terminal Masking**: Secure character-by-character masking for login password and transaction PIN entries.
- **Transaction Safety**: Withdrawals and fund transfers require explicit verification of the 4-digit PIN.

### 🏦 Core Banking Logic
- **Admin Dashboard**:
  - Provision and register new customer files.
  - Open new Savings, Checking, or Fixed Deposit accounts.
  - Global accounts manager (Freeze/Unfreeze).
  - Distribute monthly Savings Interest yields (4% p.a.).
  - Real-time global transaction audit logs.
- **Customer Portal**:
  - View multi-account balances and status cards.
  - Perform Deposits, secure ATM Withdrawals, and Intra-Bank Transfers.
  - Review transaction logs with color-coded tags.
  - Self-update profile details.
- **Analytical Suite**:
  - Compound Interest Calculator (supporting Annually, Quarterly, and Monthly compound rates).
  - Loan Eligibility Checker (analyzes Debt-to-Income ratios).

---

## 🛠️ Getting Started

### Prerequisites
- A C++17 compatible compiler (e.g. `clang++` or `g++`).
- A modern terminal supporting standard ANSI/UTF-8 escapes (Terminal.app, iTerm2, or VS Code terminal).

### Compiling
Use your C++ compiler in the workspace directory:
```bash
clang++ -std=c++17 main.cpp -o banking_system
```
*Alternatively, if using GCC:*
```bash
g++ -std=c++17 main.cpp -o banking_system
```

### Running the Application
```bash
./banking_system
```

---

## 🔑 Administrative Sign-In Credentials
On fresh launches, the database automatically pre-provisions an Admin account for testing.
* **Email**: `admin@bank.com`
* **Password**: `admin`

---

## 📂 File Organization & Data Storage
Database values are written dynamically in pipe-delimited CSV formats:
- `customers.txt`: Houses user identities, emails, telephone numbers, roles, and hashed credentials.
- `accounts.txt`: Tracks account numbers, types, balances, interest rates, and status flags.
- `transactions.txt`: Keeps history of all deposits, withdrawals, transfers, and interest payouts.

---

## ✍️ Author & Developer
This project was developed by **Shaurya Tiwari**.
