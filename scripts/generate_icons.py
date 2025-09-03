#!/usr/bin/env python3
"""
Icon generation script for MouseCross
Converts the SVG icon to various formats (PNG, ICO, ICNS) for different platforms
"""

import sys
import os
from pathlib import Path

try:
    from cairosvg import svg2png
    from PIL import Image
    import tempfile
except ImportError:
    print("Required packages not installed. Install with:")
    print("pip install cairosvg pillow")
    sys.exit(1)

def svg_to_png(svg_path, png_path, size):
    """Convert SVG to PNG at specified size"""
    try:
        svg2png(url=str(svg_path), write_to=str(png_path), output_width=size, output_height=size)
        return True
    except Exception as e:
        print(f"Error converting {svg_path} to PNG ({size}x{size}): {e}")
        return False

def create_ico(png_files, ico_path):
    """Create Windows ICO file from multiple PNG files"""
    try:
        images = []
        for png_path in png_files:
            if os.path.exists(png_path):
                img = Image.open(png_path)
                images.append(img)
        
        if images:
            images[0].save(str(ico_path), format='ICO', sizes=[(img.width, img.height) for img in images])
            return True
        return False
    except Exception as e:
        print(f"Error creating ICO file: {e}")
        return False

def create_icns(png_files, icns_path):
    """Create macOS ICNS file from multiple PNG files"""
    try:
        # Create a temporary directory for iconset
        with tempfile.TemporaryDirectory() as temp_dir:
            iconset_dir = Path(temp_dir) / "app_icon.iconset"
            iconset_dir.mkdir()
            
            # Map sizes to iconset naming convention
            size_map = {
                16: "icon_16x16.png",
                32: "icon_16x16@2x.png",
                64: "icon_32x32@2x.png",
                128: "icon_128x128.png",
                256: "icon_128x128@2x.png",
                512: "icon_256x256@2x.png",
                1024: "icon_512x512@2x.png"
            }
            
            # Copy PNG files to iconset with correct names
            for png_path in png_files:
                if os.path.exists(png_path):
                    img = Image.open(png_path)
                    size = img.width
                    if size in size_map:
                        img.save(iconset_dir / size_map[size])
                    
                    # Also create the @1x version if needed
                    if size == 32:
                        img.save(iconset_dir / "icon_32x32.png")
                    elif size == 256:
                        img.save(iconset_dir / "icon_256x256.png")
                    elif size == 512:
                        img.save(iconset_dir / "icon_512x512.png")
            
            # Use iconutil to create ICNS (macOS only)
            if sys.platform == "darwin":
                import subprocess
                result = subprocess.run(["iconutil", "-c", "icns", str(iconset_dir), "-o", str(icns_path)], 
                                      capture_output=True, text=True)
                return result.returncode == 0
            else:
                print("ICNS generation skipped (not on macOS)")
                return True
                
    except Exception as e:
        print(f"Error creating ICNS file: {e}")
        return False

def main():
    # Get script directory and project root
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    resources_dir = project_root / "resources" / "icons"
    
    # Paths
    svg_path = resources_dir / "app_icon.svg"
    
    if not svg_path.exists():
        print(f"SVG file not found: {svg_path}")
        return False
    
    print(f"Generating icons from {svg_path}")
    
    # Generate PNG files at different sizes
    sizes = [16, 32, 48, 64, 128, 256, 512, 1024]
    png_files = []
    
    for size in sizes:
        png_path = resources_dir / f"app_icon_{size}x{size}.png"
        if svg_to_png(svg_path, png_path, size):
            print(f"Generated: {png_path.name}")
            png_files.append(png_path)
        else:
            print(f"Failed to generate: {png_path.name}")
    
    # Generate main PNG (64x64 for general use)
    main_png = resources_dir / "app_icon.png"
    if svg_to_png(svg_path, main_png, 64):
        print(f"Generated: {main_png.name}")
    
    # Generate ICO file for Windows
    ico_path = resources_dir / "app_icon.ico"
    if create_ico(png_files[:6], ico_path):  # Use sizes up to 256x256 for ICO
        print(f"Generated: {ico_path.name}")
    else:
        print(f"Failed to generate: {ico_path.name}")
    
    # Generate ICNS file for macOS
    icns_path = resources_dir / "app_icon.icns"
    if create_icns(png_files, icns_path):
        print(f"Generated: {icns_path.name}")
    else:
        print(f"Failed to generate: {icns_path.name}")
    
    print("Icon generation complete!")
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)