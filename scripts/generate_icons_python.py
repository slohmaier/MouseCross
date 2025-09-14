#!/usr/bin/env python3

import sys
import os
import subprocess
from pathlib import Path

def check_dependencies():
    """Check Python dependencies"""
    try:
        from PIL import Image
        print("✓ PIL/Pillow - Found")
        pil_available = True
    except ImportError:
        print("✗ PIL/Pillow - Not found (install with: pip install Pillow)")
        pil_available = False
    
    try:
        import cairosvg
        print("✓ CairoSVG - Found")
        cairosvg_available = True
    except ImportError:
        print("✗ CairoSVG - Not found (install with: pip install cairosvg)")
        cairosvg_available = False
    
    # Check iconutil
    result = subprocess.run(['which', 'iconutil'], capture_output=True)
    iconutil_available = result.returncode == 0
    if iconutil_available:
        print("✓ iconutil (macOS) - Found")
    else:
        print("✗ iconutil (macOS) - Not found")
    
    return pil_available, cairosvg_available, iconutil_available

def svg_to_png_cairosvg(svg_path, png_path, size):
    """Convert SVG to PNG using CairoSVG"""
    try:
        import cairosvg
        cairosvg.svg2png(url=str(svg_path), write_to=str(png_path), 
                        output_width=size, output_height=size)
        return True
    except Exception as e:
        print(f"Error converting SVG to PNG: {e}")
        return False

def generate_png_from_svg_python(svg_path, png_path, size):
    """Generate PNG from SVG using pure Python tools"""
    return svg_to_png_cairosvg(svg_path, png_path, size)

def generate_ico_with_pil(base_png_path, ico_path):
    """Generate ICO using PIL"""
    try:
        from PIL import Image
        
        # Generate multiple sizes for ICO
        sizes = [16, 24, 32, 48, 64, 128, 256]
        images = []
        
        # Load the base PNG and create different sizes
        base_img = Image.open(base_png_path)
        
        for size in sizes:
            img_resized = base_img.resize((size, size), Image.Resampling.LANCZOS)
            images.append(img_resized)
        
        # Save as ICO
        images[0].save(ico_path, format='ICO', sizes=[(img.width, img.height) for img in images])
        print(f"✓ Generated ICO: {ico_path}")
        return True
        
    except Exception as e:
        print(f"✗ Error generating ICO: {e}")
        return False

def generate_icns_python(base_png_path, icns_path):
    """Generate ICNS using iconutil"""
    try:
        from PIL import Image
        
        # Create iconset directory
        iconset_dir = str(icns_path).replace('.icns', '.iconset')
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
        
        # Load base image
        base_img = Image.open(base_png_path)
        
        # Generate all required PNG sizes
        for size, filename in sizes:
            target_path = os.path.join(iconset_dir, filename)
            resized_img = base_img.resize((size, size), Image.Resampling.LANCZOS)
            resized_img.save(target_path, 'PNG')
        
        # Generate ICNS from iconset using iconutil
        cmd = f'iconutil -c icns "{iconset_dir}" -o "{icns_path}"'
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        
        if result.returncode == 0:
            print(f"✓ Generated ICNS: {icns_path}")
            # Clean up iconset directory
            import shutil
            shutil.rmtree(iconset_dir)
            return True
        else:
            print(f"✗ Error generating ICNS: {result.stderr}")
            return False
            
    except Exception as e:
        print(f"✗ Error generating ICNS: {e}")
        return False

def main():
    """Main icon generation pipeline using Python tools"""
    print("MouseCross Icon Generation (Python)")
    print("=" * 40)
    
    # Check dependencies
    pil_available, cairosvg_available, iconutil_available = check_dependencies()
    print()
    
    if not (pil_available and cairosvg_available):
        print("Required dependencies missing. Install with:")
        print("  pip install Pillow cairosvg")
        sys.exit(1)
    
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
    hires_png = icons_dir / 'app_icon_hires.png'
    print(f"Generating high-res PNG: {hires_png}")
    if not generate_png_from_svg_python(svg_path, hires_png, 1024):
        success = False
    
    # 2. Generate standard PNG (64x64) from SVG  
    if success:
        std_png = icons_dir / 'app_icon.png'
        print(f"Generating standard PNG: {std_png}")
        if not generate_png_from_svg_python(svg_path, std_png, 64):
            success = False
    
    # 3. Generate ICO from high-res PNG
    if success:
        ico_path = icons_dir / 'app_icon.ico'
        print(f"Generating ICO: {ico_path}")
        if not generate_ico_with_pil(hires_png, ico_path):
            success = False
    
    # 4. Generate ICNS from high-res PNG (macOS)
    if success and iconutil_available:
        icns_path = icons_dir / 'app_icon.icns'
        print(f"Generating ICNS: {icns_path}")
        if not generate_icns_python(hires_png, icns_path):
            success = False
    elif success:
        print("⚠ Skipping ICNS generation (iconutil not available)")
    
    if success:
        print("\n✅ Icon generation completed!")
        print(f"Generated files in {icons_dir}:")
        for icon_file in ['app_icon.png', 'app_icon_hires.png', 'app_icon.ico', 'app_icon.icns']:
            icon_path = icons_dir / icon_file
            if icon_path.exists():
                print(f"  - {icon_file}")
    else:
        print("\n❌ Icon generation failed")
        sys.exit(1)

if __name__ == "__main__":
    main()