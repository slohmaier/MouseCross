# MouseCross

<div align="center">
  <img src="app_icon.png" alt="MouseCross Icon" width="128" height="128">
</div>

MouseCross is a crosshair overlay application designed to help visually impaired users with tunnel vision locate their mouse cursor on the screen.

## Features

- **Always-on-top crosshair overlay** with lines extending from cursor to screen edges
- **Direction shapes** along crosshair lines (Circle, Arrow, Cross, or Raute/Diamond)
- **Customizable appearance**: line width, color, opacity, and shape type
- **Progressive scaling**: Shapes grow from center to screen edges for better visibility
- **Configurable offset** from cursor position where lines start
- **Inverted mode** for visibility on any background (like Windows inverted cursor)
- **System tray integration** with easy toggle functionality
- **Configurable hotkey** for quick activation/deactivation
- **Auto-start** with Windows/macOS support
- **Welcome screen** for first-time users

## System Requirements

- Windows 10+ or macOS 10.14+
- Qt 6.2+
- CMake 3.16+
- C++17 compatible compiler

## Building

### Prerequisites

Install Qt 6.2 or later and CMake 3.16 or later.

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Windows

On Windows, you may need to specify the Qt installation path:

```bash
cmake -DCMAKE_PREFIX_PATH="C:\Qt\6.5.0\msvc2022_64" ..
```

### macOS

On macOS, if using Homebrew Qt:

```bash
cmake -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt@6" ..
```

## Usage

1. **System Tray**: MouseCross runs from the system tray
2. **Toggle**: Double-click the tray icon or use the configured hotkey to toggle the crosshair
3. **Settings**: Right-click the tray icon and select "Settings..." to customize appearance and behavior
4. **Auto-start**: Enable in settings to start MouseCross automatically when your computer boots

## Default Settings

- **Hotkey**: Ctrl+Alt+C
- **Line Width**: 2 pixels
- **Offset from Cursor**: 10 pixels
- **Color**: Red
- **Opacity**: 80%

## Customization

The crosshair can be customized through the Settings dialog:

- **Appearance**: Color, line width, opacity, thickness growth
- **Direction shapes**: Choose between Circle, Arrow, Cross, or Raute (Diamond)
- **Shape spacing**: Control how spacing between shapes increases from center to edges
- **Offset from cursor**: Distance from cursor where lines start
- **Inverted mode**: Uses difference blending for visibility on any background
- **Behavior**: Auto-start and activation settings
- **Hotkey**: Custom keyboard shortcut for toggling

## License

This project is open source. See the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.