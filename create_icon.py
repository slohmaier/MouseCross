#!/usr/bin/env python3

import sys
try:
    from PIL import Image, ImageDraw
except ImportError:
    print("PIL (Pillow) is required to generate icons.")
    print("Install it with: pip install Pillow")
    sys.exit(1)

def create_crosshair_icon(size=64):
    # Create image with transparent background 
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Colors - consistent with tray icon
    bg_color = (44, 62, 80, 255)  # Dark blue-gray background
    border_color = (255, 255, 255, 255)  # White border
    crosshair_color = (255, 255, 255, 255)  # White crosshair
    
    # Calculate dimensions - simpler design like tray icon
    center = size // 2
    margin = max(1, size // 32)
    corner_radius = max(4, size // 8)  # More prominent rounded corners
    line_width = max(2, size // 11)  # Proportional line width
    border_width = max(2, size // 16)
    
    # Draw rounded rectangle background with border - match tray icon style
    draw.rounded_rectangle([margin, margin, size-margin-1, size-margin-1], 
                          radius=corner_radius, fill=bg_color, outline=border_color, width=border_width)
    
    # Draw simple crosshair like tray icon - horizontal lines with gap in center
    gap = max(6, size // 8)
    inner_margin = max(4, size // 8)
    
    # Horizontal line (with gap in center)
    left_end = center - gap//2
    right_start = center + gap//2
    
    if left_end > margin + inner_margin:
        draw.rectangle([margin + inner_margin, center - line_width//2, 
                       left_end, center + line_width//2], fill=crosshair_color)
    if right_start < size - margin - inner_margin:
        draw.rectangle([right_start, center - line_width//2, 
                       size - margin - inner_margin, center + line_width//2], fill=crosshair_color)
    
    # Vertical line (with gap in center)  
    top_end = center - gap//2
    bottom_start = center + gap//2
    
    if top_end > margin + inner_margin:
        draw.rectangle([center - line_width//2, margin + inner_margin,
                       center + line_width//2, top_end], fill=crosshair_color)
    if bottom_start < size - margin - inner_margin:
        draw.rectangle([center - line_width//2, bottom_start,
                       center + line_width//2, size - margin - inner_margin], fill=crosshair_color)
    
    return img

def create_icns_from_images(images, icns_path):
    """Create ICNS file from images using iconutil on macOS"""
    import tempfile
    import shutil
    import subprocess
    import os
    
    # Create temporary iconset directory
    with tempfile.TemporaryDirectory() as temp_dir:
        iconset_dir = os.path.join(temp_dir, 'app_icon.iconset')
        os.makedirs(iconset_dir)
        
        # Define required ICNS sizes and filenames
        icns_specs = [
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
        
        # Generate and save all required sizes
        for size, filename in icns_specs:
            icon_img = create_crosshair_icon(size)
            icon_path = os.path.join(iconset_dir, filename)
            icon_img.save(icon_path, 'PNG')
        
        # Use iconutil to create ICNS
        try:
            cmd = ['iconutil', '-c', 'icns', iconset_dir, '-o', icns_path]
            result = subprocess.run(cmd, capture_output=True, text=True)
            if result.returncode == 0:
                return True
            else:
                print(f"iconutil error: {result.stderr}")
                return False
        except FileNotFoundError:
            print("iconutil not found (macOS required for ICNS generation)")
            return False
        except Exception as e:
            print(f"Error creating ICNS: {e}")
            return False

def main():
    # Create different sizes for the icon
    sizes = [16, 32, 48, 64, 128, 256]
    
    # Create standard PNG icon (64x64)
    icon_64 = create_crosshair_icon(64)
    icon_64.save('resources/icons/app_icon.png', 'PNG')
    print("Created resources/icons/app_icon.png (64x64)")
    
    # Create high-resolution PNG icon (1024x1024)
    icon_1024 = create_crosshair_icon(1024)
    icon_1024.save('resources/icons/app_icon_hires.png', 'PNG')
    print("Created resources/icons/app_icon_hires.png (1024x1024)")
    
    # Create ICO file with multiple sizes for Windows
    icons = []
    for size in sizes:
        icons.append(create_crosshair_icon(size))
    
    # Save as ICO
    icons[0].save('resources/icons/app_icon.ico', format='ICO', 
                  sizes=[(size, size) for size in sizes])
    print("Created resources/icons/app_icon.ico")
    
    # Create ICNS file for macOS
    if create_icns_from_images(icons, 'resources/icons/app_icon.icns'):
        print("Created resources/icons/app_icon.icns")
    else:
        print("Failed to create ICNS file")
    
    print("Icons created successfully!")

if __name__ == "__main__":
    main()