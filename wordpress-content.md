# MouseCross - Visual Mouse Locator for Accessibility

![MouseCross Icon](app_icon.png)

**MouseCross** is a specialized crosshair overlay application designed to help visually impaired users with tunnel vision locate their mouse cursor on the screen. The application provides an always-on-top crosshair that extends from the cursor to all screen edges, making it instantly visible regardless of background color or complexity.

## 🎯 Key Features

- **Full-Screen Crosshair Lines**: Lines extend from mouse cursor to all screen edges (top, bottom, left, right)
- **Intelligent Inverted Mode**: Dual-layer lines (black outer, white inner) ensure visibility on any background
- **Gradient Thickness**: Lines get progressively thicker toward screen edges for enhanced visibility
- **UI Scale Awareness**: Automatically adjusts line thickness based on Windows/macOS display scaling
- **System Tray Integration**: Runs unobtrusively in background with easy toggle functionality
- **Customizable Hotkey**: Set any key combination for instant crosshair toggle (default: Ctrl+Alt+C)
- **Auto-Start Support**: Optional startup with Windows/macOS
- **Cross-Platform**: Native support for Windows and macOS

## 🔧 Customization Options

The comprehensive settings dialog allows users to fine-tune the crosshair appearance:

### Appearance Settings
- **Line Width**: Base thickness (3-10 pixels, default: 4px)
- **Offset from Cursor**: Gap size around cursor (0-100 pixels, default: 49px)  
- **Thickness Growth**: How much lines thicken toward edges (1.0x-10.0x, default: 3.0x)
- **Color**: Customizable line color (default: white)
- **Opacity**: Transparency level (10-100%, default: 80%)
- **Direction Arrows**: Small arrows on inner line pointing to center (default: enabled)

### Behavior Settings
- **Auto-Start**: Launch with system startup
- **Activate on Start**: Show crosshair immediately when app launches (default: enabled)
- **Custom Hotkey**: Set personalized toggle shortcut (default: Ctrl+Alt+Shift+C)

## 🎯 Target Audience

MouseCross is specifically designed for:
- **Users with Tunnel Vision**: Provides instant cursor location across entire screen
- **Low Vision Users**: High-contrast crosshair visible on any background
- **Users with Visual Processing Difficulties**: Clear, consistent visual reference
- **Assistive Technology Users**: Complements screen readers and magnifiers
- **Gaming Accessibility**: Enhanced cursor visibility for games and applications

## 💻 System Requirements

- **Windows**: Windows 10 or later
- **macOS**: macOS 10.14 (Mojave) or later
- **Development**: Qt 6.2+, CMake 3.16+, C++17 compiler

## 🛠 Technical Features

- **Native Qt6 Implementation**: Modern, efficient cross-platform framework
- **High DPI Support**: Automatically scales with display settings
- **Multiple Monitor Support**: Works across all connected displays
- **Minimal Resource Usage**: Lightweight system tray application
- **No Installation Required**: Portable executable

## 🚀 Getting Started

1. **Download** MouseCross from the official website
2. **Launch** the application - it will appear in your system tray
3. **Toggle Crosshair**: Double-click tray icon or use Ctrl+Alt+Shift+C
4. **Customize**: Right-click tray icon → Settings for full customization
5. **Auto-Start**: Enable in settings to start with your computer

## 📸 Screenshots

*[Include screenshots showing:]*
- *Application in system tray*
- *Crosshair in normal mode*
- *Crosshair in inverted mode*
- *Settings dialog*
- *Welcome screen*

## 🔗 Links

- **Official Website**: [https://slohmaier.de/MouseCross](https://slohmaier.de/MouseCross)
- **Source Code**: Available on GitHub
- **Version**: 0.1.0
- **License**: Open Source

## 📋 Installation Instructions

### Quick Start
1. Download MouseCross executable
2. Run the application
3. Grant permissions if prompted (for global hotkey access)
4. Configure settings through system tray menu

### Building from Source
```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH="path/to/qt6" ..
cmake --build . --config Release
```

## 🎨 Design Philosophy

MouseCross follows accessibility-first design principles:
- **High Contrast**: Always visible regardless of background
- **Consistent Behavior**: Predictable operation across all applications
- **User Control**: Extensive customization without overwhelming interface
- **System Integration**: Native feel with OS conventions
- **Performance**: Minimal impact on system resources

## 🆘 Support

For support, feature requests, or bug reports, visit our website or contact us directly. MouseCross is committed to improving accessibility for all users.

---

*MouseCross - Making the invisible cursor visible*