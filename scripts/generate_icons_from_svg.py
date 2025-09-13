#!/usr/bin/env python3

import sys
import os
import subprocess
from pathlib import Path

def run_command(cmd, description):
    """Run a shell command and handle errors"""
    print(f"{description}...")
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error {description}: {result.stderr}")
        return False
    return True

def check_dependencies():
    """Check if required tools are available"""
    tools = {
        'inkscape': 'Inkscape (for SVG to PNG conversion)',
        'iconutil': 'iconutil (macOS, for ICNS generation)',
        'convert': 'ImageMagick (for ICO generation - optional)'
    }
    
    available_tools = {}
    for tool, desc in tools.items():
        result = subprocess.run(['which', tool], capture_output=True)
        available_tools[tool] = result.returncode == 0
        if available_tools[tool]:
            print(f"✓ {desc} - Found")
        else:
            print(f"✗ {desc} - Not found")
    
    return available_tools

def generate_png_from_svg(svg_path, png_path, size):
    """Generate PNG from SVG using Inkscape"""
    cmd = f'inkscape --export-type=png --export-filename="{png_path}" --export-width={size} --export-height={size} "{svg_path}"'
    return run_command(cmd, f"Generating {size}x{size} PNG")

def generate_icns(png_path, icns_path):
    """Generate ICNS from PNG using iconutil (macOS)"""
    # Create iconset directory
    iconset_dir = icns_path.replace('.icns', '.iconset')
    os.makedirs(iconset_dir, exist_ok=True)
    
    # Generate required sizes for ICNS
    sizes = [
        (16, 'icon_16x16.png'),
        (32, 'icon_16x16@2x.png'), 
        (32, 'icon_32x32.png'),
        (64, 'icon_32x32@2x.png'),
        (128, 'icon_128x128.png'),
        (256, 'icon_128x128@2x.png'),
        (256, 'icon_256x256.png'),
        (512, 'icon_256x256@2x.png'),
        (512, 'icon_512x512.png'),
        (1024, 'icon_512x512@2x.png')
    ]
    
    svg_path = Path(__file__).parent.parent / 'resources' / 'icons' / 'app_icon.svg'
    
    # Generate all required PNG sizes
    success = True
    for size, filename in sizes:
        target_path = os.path.join(iconset_dir, filename)
        if not generate_png_from_svg(svg_path, target_path, size):
            success = False
            break
    
    if success:
        # Generate ICNS from iconset
        cmd = f'iconutil -c icns "{iconset_dir}" -o "{icns_path}"'
        success = run_command(cmd, "Generating ICNS file")
        
        # Clean up iconset directory
        import shutil
        shutil.rmtree(iconset_dir)
    
    return success

def generate_ico_with_pil(png_path, ico_path):
    """Generate ICO using PIL as fallback"""
    try:
        from PIL import Image
        
        # Generate multiple sizes for ICO
        sizes = [16, 24, 32, 48, 64, 128, 256]
        images = []
        
        # Load the high-res PNG and create different sizes
        base_img = Image.open(png_path)
        
        for size in sizes:
            img_resized = base_img.resize((size, size), Image.Resampling.LANCZOS)
            images.append(img_resized)
        
        # Save as ICO
        images[0].save(ico_path, format='ICO', sizes=[(img.width, img.height) for img in images])
        print("✓ Generated ICO using PIL")
        return True
        
    except ImportError:
        print("✗ PIL not available for ICO generation")
        return False
    except Exception as e:
        print(f"✗ Error generating ICO with PIL: {e}")
        return False

def generate_ico(png_path, ico_path, has_imagemagick):
    """Generate ICO from PNG"""
    if has_imagemagick:
        # Use ImageMagick if available
        cmd = f'convert "{png_path}" -define icon:auto-resize=256,128,64,48,32,16 "{ico_path}"'
        if run_command(cmd, "Generating ICO with ImageMagick"):
            return True
    
    # Fall back to PIL
    return generate_ico_with_pil(png_path, ico_path)

def main():
    """Main icon generation pipeline"""
    print("MouseCross Icon Generation Pipeline")
    print("=" * 40)
    
    # Check dependencies
    tools = check_dependencies()
    print()
    
    # Paths
    base_dir = Path(__file__).parent.parent
    svg_path = base_dir / 'resources' / 'icons' / 'app_icon.svg'
    icons_dir = base_dir / 'resources' / 'icons'
    
    # Ensure icons directory exists
    icons_dir.mkdir(parents=True, exist_ok=True)
    
    if not svg_path.exists():
        print(f"✗ SVG source not found: {svg_path}")
        sys.exit(1)
    
    success = True
    
    # 1. Generate high-res PNG (1024x1024) from SVG
    if tools['inkscape']:
        hires_png = icons_dir / 'app_icon_hires.png'
        if not generate_png_from_svg(svg_path, hires_png, 1024):
            success = False
    else:
        print("✗ Inkscape required for SVG to PNG conversion")
        success = False
    
    # 2. Generate standard PNG (64x64) from SVG  
    if tools['inkscape'] and success:
        std_png = icons_dir / 'app_icon.png'
        if not generate_png_from_svg(svg_path, std_png, 64):
            success = False
    
    # 3. Generate ICNS from high-res PNG (macOS)
    if tools['iconutil'] and success:
        icns_path = icons_dir / 'app_icon.icns'
        if not generate_icns(hires_png, icns_path):
            success = False
    else:
        print("⚠ Skipping ICNS generation (iconutil not available or previous step failed)")
    
    # 4. Generate ICO from high-res PNG (Windows)
    if success:
        ico_path = icons_dir / 'app_icon.ico'
        if not generate_ico(hires_png, ico_path, tools['convert']):
            success = False
    
    if success:
        print("\n✅ All icons generated successfully!")
        print(f"Generated files in {icons_dir}:")
        for icon_file in ['app_icon.png', 'app_icon_hires.png', 'app_icon.icns', 'app_icon.ico']:
            icon_path = icons_dir / icon_file
            if icon_path.exists():
                print(f"  - {icon_file}")
    else:
        print("\n❌ Icon generation failed")
        sys.exit(1)

if __name__ == "__main__":
    main()