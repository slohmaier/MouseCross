# MouseCross - Mac App Store Certificate Setup Guide

## ✅ Certificate Signing Requests Created

Your Certificate Signing Requests (CSRs) have been created and saved to:
**`~/Documents/Apple Developer Account/MouseCross-Certificates/`**

## 📋 Required Certificates for Mac App Store

You need **TWO** certificates for Mac App Store submission:

### 1. **Mac App Distribution**
- **Purpose**: Signs the app bundle (.app file)
- **Apple Portal Name**: "Mac App Distribution"
- **CSR File**: `MouseCross_App_Distribution.csr`
- **Private Key**: `MouseCross_App_Distribution.key` (keep secure!)

### 2. **Mac Installer Distribution**
- **Purpose**: Signs the installer package (.pkg file)
- **Apple Portal Name**: "Mac Installer Distribution"
- **CSR File**: `MouseCross_Installer_Distribution.csr`
- **Private Key**: `MouseCross_Installer_Distribution.key` (keep secure!)

## 🔧 Step-by-Step Certificate Creation

### Step 1: Access Apple Developer Portal
1. Go to: https://developer.apple.com/account/resources/certificates
2. Sign in with your Apple Developer account
3. Click the **"+"** button to create a new certificate

### Step 2: Create Mac App Distribution Certificate
1. **Select Certificate Type**: Choose **"Mac App Distribution"**
2. **Upload CSR**: Select `MouseCross_App_Distribution.csr` from your Documents folder
3. **Download**: Save the certificate (`.cer` file) to your Downloads
4. **Install**: Double-click the downloaded `.cer` file to install in Keychain Access

### Step 3: Create Mac Installer Distribution Certificate
1. **Select Certificate Type**: Choose **"Mac Installer Distribution"**
2. **Upload CSR**: Select `MouseCross_Installer_Distribution.csr` from your Documents folder
3. **Download**: Save the certificate (`.cer` file) to your Downloads
4. **Install**: Double-click the downloaded `.cer` file to install in Keychain Access

### Step 4: Verify Installation
Run the verification script to check your certificates:
```bash
~/Documents/"Apple Developer Account"/MouseCross-Certificates/verify_certificates.sh
```

## 🎯 Correct Certificate Types in Apple Developer Portal

Based on the options you provided, here are the **exact** certificates you need:

✅ **"Mac App Distribution"**
- Description: "This certificate is used to code sign your app and configure a Distribution Provisioning Profile for submission to the Mac App Store Connect."

✅ **"Mac Installer Distribution"**
- Description: "This certificate is used to sign your app's Installer Package for submission to the Mac App Store Connect."

❌ **DO NOT USE**:
- "Apple Distribution" (for iOS/multi-platform)
- "Developer ID Application" (for distribution outside Mac App Store)
- "Developer ID Installer" (for distribution outside Mac App Store)
- "Mac Development" (for development only)

## 🔍 Troubleshooting Common Issues

### Issue: "Certificate not found in keychain"
**Solution**:
1. Check Keychain Access (Applications → Utilities → Keychain Access)
2. Look in "My Certificates" section
3. Ensure certificates show "Mac Developer: Stefan Lohmaier"
4. If missing, re-download and install certificates

### Issue: "Private key not found"
**Solution**:
1. The private key (.key file) must stay in the same location
2. Check file permissions: `ls -la ~/Documents/"Apple Developer Account"/MouseCross-Certificates/`
3. Private keys should be `-rw-------` (600 permissions)

### Issue: "Certificate appears but signing fails"
**Solution**:
1. Check that both certificate AND private key are present
2. The certificate and private key must be linked in Keychain Access
3. Try logging out and back into Keychain Access

## 🛡️ Security Best Practices

### Private Key Protection
- **Never share** the `.key` files
- **Backup securely** to encrypted storage
- **Restricted permissions** already set (600)

### Certificate Management
- Certificates expire after 1 year
- You'll need to renew before expiration
- Keep track of expiration dates

### Team Management
- Only team members with "Admin" or "Account Holder" role can create certificates
- Each team can have limited number of distribution certificates

## 🚀 After Certificate Installation

Once certificates are installed, you can:

### Test Certificate Installation
```bash
# From MouseCross deployment/macos directory
~/Documents/"Apple Developer Account"/MouseCross-Certificates/verify_certificates.sh
```

### Run App Store Submission
```bash
# Build the app for App Store
./build_appstore.sh

# Submit to App Store (with certificates installed)
./submit_appstore.sh
```

## 📱 Expected Certificate Names

After installation, your certificates should appear in Keychain Access as:
- **"Mac Developer: Stefan Lohmaier (TEAM_ID)"** - for app signing
- **"Mac Developer: Stefan Lohmaier (TEAM_ID)"** - for installer signing

Where `TEAM_ID` is your 10-character Apple Developer Team ID.

## 🔄 Certificate Renewal Process

When certificates expire (after 1 year):
1. Create new CSRs using the same script
2. Follow the same creation process
3. Replace old certificates in Keychain Access
4. Update any automated scripts with new certificate details

## 📞 Support Resources

- **Apple Developer Portal**: https://developer.apple.com/account/resources/certificates
- **Keychain Access**: Applications → Utilities → Keychain Access
- **Certificate Verification**: Run `verify_certificates.sh` script
- **MouseCross Documentation**: See main submission guide

---

## ✅ Quick Checklist

- [ ] Access Apple Developer Portal
- [ ] Create "Mac App Distribution" certificate with `MouseCross_App_Distribution.csr`
- [ ] Create "Mac Installer Distribution" certificate with `MouseCross_Installer_Distribution.csr`
- [ ] Download both `.cer` files
- [ ] Install both certificates (double-click)
- [ ] Run verification script to confirm installation
- [ ] Test with `./submit_appstore.sh --sign-only`

**Ready for App Store submission once all certificates are installed!** 🎉