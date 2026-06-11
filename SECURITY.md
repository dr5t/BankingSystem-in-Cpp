# Security Policy

This security policy outlines the safety configurations, data protection mechanisms, and vulnerability reporting procedures established for the **Advanced Banking Suite**, maintained by **Shaurya Tiwari**.

---

## 🔒 Data Security Standards

The codebase implements strict controls to protect bank data and client credentials:
1. **Password Protection**: Plain-text passwords are never stored in database files. Every password and transaction PIN is salted and hashed using **SHA-256** encryption before writes.
2. **Masked Console Input**: POSIX termios configurations are adjusted during login to prevent passwords and transaction PINs from echoing on the screen. Inputs are visually replaced with masking symbols (`*`).
3. **Field Validation**: Validation regex and character parsing are used on emails, phone numbers, and decimal numbers to block script injection or buffer corruption.

---

## 📈 Supported Versions

Security updates are actively maintained for the following versions:

| Version | Supported | Notes |
| ------- | --------- | ----- |
| 1.0.x   | Yes       | Active maintenance branch by Shaurya Tiwari |
| < 1.0.0 | No        | Legacy prototypes |

---

## 🚨 Reporting a Vulnerability

If you discover a security vulnerability or data exposure risk, please report it to Shaurya Tiwari:
1. Go to the GitHub repository: **[dr5t/BankingSystem](https://github.com/dr5t/BankingSystem)**.
2. Create a confidential issue or contact the developer directly at your registered email address.
3. Provide details on the vulnerability, steps to reproduce, and possible impact.
4. We aim to acknowledge receipt within 48 hours and release an official patch within 7 business days.
