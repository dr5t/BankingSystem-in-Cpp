# Security Policy

## Supported Versions

The following versions of the Banking System are currently supported with security updates:

| Version | Supported |
| ------- | --------- |
| 1.0.x   | Yes       |
| < 1.0.0 | No        |

## Security Implementation Highlights

To ensure secure console operations and robust file handling, this application incorporates:
- **Hashing**: All passwords and transaction PINs are processed using a secure C++ implementation of the **SHA-256** cryptographic hashing algorithm before storage in files.
- **PIN Masking**: Transaction PINs and sign-in passwords entered in the console are masked using terminal control functions (disabling terminal echo) so they are never visible on screen.
- **Strict Validations**: Input sanitation prevents directory traversals, line injection into database files, and invalid formats for email and telephone entries.

## Reporting a Vulnerability

If you discover a security vulnerability in this project, please report it immediately:
1. Open an issue on this repository labeled `security`, or contact the maintainer directly.
2. Provide a clear description of the vulnerability, steps to reproduce, and potential impact.
3. We will acknowledge receipt within 48 hours and coordinate a patch within 7 business days.
