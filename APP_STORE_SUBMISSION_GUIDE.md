# MouseCross - Mac App Store Submission Guide

## ✅ Deployment Status
**Status**: Ready for App Store submission!
- **App Bundle**: `dist/MouseCross.app` (fully bundled with Qt frameworks)
- **Installer Package**: `dist/MouseCross-0.1.0-appstore.pkg` (23MB)
- **LGPL Compliance**: ✅ Complete with bundled Qt frameworks

## Prerequisites for App Store Submission

### 1. Apple Developer Account
- **Apple Developer Program membership** ($99/year)
- **App Store Connect access** with admin/app manager role
- **Development Team ID** configured in Xcode

### 2. Code Signing Requirements
You'll need these certificates from Apple Developer:
- **Mac App Store distribution certificate**
- **Mac Installer Distribution certificate**
- **App Store provisioning profile** for MouseCross

### 3. App Store Connect App Record
Create your app record at [App Store Connect](https://appstoreconnect.apple.com):
- **Bundle ID**: `de.slohmaier.mousecross` (or your chosen identifier)
- **App Name**: "MouseCross"
- **Category**: Utilities or Accessibility
- **Price**: Free (or your chosen price tier)

## Step-by-Step Submission Process

### Phase 1: Code Signing

#### 1.1 Install Distribution Certificates
```bash
# Download certificates from Apple Developer portal
# Install them in Keychain Access by double-clicking
```

#### 1.2 Sign the App Bundle
```bash
cd /Users/stefan/git/MouseCross

# Sign all Qt frameworks first
codesign --force --verify --verbose --sign "3rd Party Mac Developer Application: [Your Name]" \
  dist/MouseCross.app/Contents/Frameworks/*.framework

# Sign all plugins
find dist/MouseCross.app/Contents/PlugIns -name "*.dylib" -exec \
  codesign --force --verify --verbose --sign "3rd Party Mac Developer Application: [Your Name]" {} \;

# Sign the main executable
codesign --force --verify --verbose --sign "3rd Party Mac Developer Application: [Your Name]" \
  dist/MouseCross.app/Contents/MacOS/MouseCross

# Sign the app bundle with entitlements
codesign --force --verify --verbose --sign "3rd Party Mac Developer Application: [Your Name]" \
  --entitlements deployment/macos/MouseCross.entitlements \
  dist/MouseCross.app
```

#### 1.3 Create Signed Installer
```bash
# Create the final signed installer package
productbuild --component dist/MouseCross.app /Applications \
  --sign "3rd Party Mac Developer Installer: [Your Name]" \
  MouseCross-AppStore-Final.pkg
```

### Phase 2: App Store Metadata

#### 2.1 Required App Information
**App Description (English)**:
```
MouseCross helps visually impaired users locate their mouse cursor with a customizable crosshair overlay.

Features:
• Customizable crosshair with multiple shapes (Circle, Arrow, Cross, Diamond)
• Adjustable size, color, and opacity settings
• Inverted mode for visibility on any background
• System tray integration with easy toggle controls
• Accessible design with keyboard shortcuts
• Runs efficiently in the background

Perfect for users with visual impairments, large displays, or anyone who frequently loses track of their cursor. MouseCross is free, open-source software licensed under LGPL v3.0.
```

**Keywords**: `accessibility, mouse, cursor, crosshair, visual, impaired, overlay, utilities`

**Support URL**: `https://slohmaier.de/mousecross`

**Privacy Policy URL**: `https://slohmaier.de/mousecross/privacy` (create if needed)

#### 2.2 App Screenshots
Required sizes for macOS:
- **1280 x 800** (primary)
- **1440 x 900**
- **2560 x 1600** (Retina)
- **2880 x 1800** (Retina)

Create screenshots showing:
1. MouseCross settings dialog
2. Crosshair overlay in action
3. System tray integration
4. Different crosshair shapes/styles

#### 2.3 App Categories
- **Primary Category**: Utilities
- **Secondary Category**: Accessibility (if available)

### Phase 3: Technical Information

#### 3.1 App Review Information
**Review Notes**:
```
MouseCross is an accessibility application that helps visually impaired users locate their mouse cursor.

TESTING INSTRUCTIONS:
1. Launch MouseCross from Applications folder
2. The app runs in the system tray (menu bar)
3. Click the MouseCross icon in menu bar to access settings
4. Enable crosshair overlay - you'll see crosshair following mouse cursor
5. Test different crosshair shapes and settings in preferences
6. Verify system tray menu functions (toggle, settings, quit)

ACCESSIBILITY FEATURES:
- Crosshair overlay helps locate mouse cursor
- Keyboard shortcuts for quick toggle
- High contrast inverted mode
- Customizable size and colors

The app requires no special permissions and runs entirely in user space.
```

**Demo Account**: Not needed (no login required)

#### 3.2 Version Information
- **Version**: `0.1.0`
- **Build Number**: `1` (increment for each submission)
- **Copyright**: `© 2025 Stefan Lohmaier`
- **Trade Representative Contact**: Your contact information

#### 3.3 App Privacy
**Data Collection**: None
**Privacy Practices**:
- "No data collected" - MouseCross runs locally and doesn't collect, store, or transmit user data
- No analytics, advertising, or tracking

### Phase 4: Upload to App Store Connect

#### 4.1 Using Application Loader (Xcode)
```bash
# Option 1: Use Xcode's Application Loader
xcrun altool --upload-app --type osx --file MouseCross-AppStore-Final.pkg \
  --username [your-apple-id] --password [app-specific-password]
```

#### 4.2 Using Transporter App
1. Download **Transporter** from Mac App Store
2. Sign in with Apple Developer credentials
3. Drag `MouseCross-AppStore-Final.pkg` to Transporter
4. Click "Deliver" to upload

### Phase 5: App Store Connect Configuration

#### 5.1 Build Assignment
1. Go to **App Store Connect** → **My Apps** → **MouseCross**
2. Navigate to **App Store** tab
3. Select your uploaded build under "Build"
4. Add **Export Compliance** information:
   - "Does not use encryption" (standard selection)

#### 5.2 Release Options
Choose release method:
- **Automatic**: Release immediately after approval
- **Manual**: You manually release after approval
- **Scheduled**: Release on specific date after approval

#### 5.3 Pricing and Availability
- **Price**: Free
- **Availability**: All countries/regions
- **Educational Discount**: Not applicable for free apps

### Phase 6: Submit for Review

#### 6.1 Final Review Checklist
- [ ] App bundle properly signed
- [ ] All metadata completed
- [ ] Screenshots uploaded (all required sizes)
- [ ] Privacy information complete
- [ ] App description accurate and compelling
- [ ] Build successfully uploaded and processed
- [ ] Export compliance answered
- [ ] Release preferences set

#### 6.2 Submit
1. Click **"Submit for Review"**
2. Review submission summary
3. Confirm submission

**Expected Review Time**: 2-7 days for initial review

## Common Issues and Solutions

### Code Signing Issues
**Problem**: "Code signature invalid"
**Solution**: Ensure all frameworks and plugins are signed before signing the main app bundle

### Upload Issues
**Problem**: "Invalid bundle structure"
**Solution**: Verify Qt frameworks are properly bundled in `Contents/Frameworks/`

### Review Rejection: Functionality
**Problem**: "App doesn't function as expected"
**Solution**: Provide clear testing instructions in review notes

### Review Rejection: Metadata
**Problem**: "Description doesn't match functionality"
**Solution**: Ensure description accurately reflects app features

## Post-Approval Tasks

### 1. Monitor Analytics
- Track downloads via App Store Connect
- Monitor user ratings and reviews
- Check crash reports (if any)

### 2. Updates Process
For future updates:
1. Increment version number in CMakeLists.txt
2. Rebuild with `./build_appstore.sh`
3. Code sign new build
4. Upload new version to App Store Connect
5. Update "What's New" description
6. Submit update for review

### 3. LGPL Compliance
**Important**: As MouseCross uses LGPL-licensed Qt, you must:
- Reference your website in app description: "Source code available at slohmaier.de/mousecross"
- Maintain LGPL compliance documentation on your website
- Provide rebuild instructions (already created in LGPL_REBUILD_GUIDE.md)

## Support Resources

- **App Store Connect**: https://appstoreconnect.apple.com
- **Apple Developer Documentation**: https://developer.apple.com/app-store/review/guidelines/
- **MouseCross Website**: https://slohmaier.de/mousecross
- **Source Code**: https://github.com/slohmaier/MouseCross

## Current Status Summary

✅ **App Build**: Ready (`dist/MouseCross-0.1.0-appstore.pkg`)
✅ **LGPL Compliance**: Complete with bundled Qt frameworks
✅ **Documentation**: Complete rebuild guides provided
⏳ **Next Steps**: Code signing and App Store Connect submission

---
*Last Updated: September 14, 2025*
*MouseCross is ready for Mac App Store submission!*