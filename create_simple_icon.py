#!/usr/bin/env python3

import sys
try:
    from PIL import Image, ImageDraw
except ImportError:
    print("PIL (Pillow) is required to generate icons.")
    print("Install it with: pip install Pillow")
    sys.exit(1)

def create_simple_crosshair_icon(size=64):
    # Create image with solid dark background
    img = Image.new('RGBA', (size, size), (32, 45, 64, 255))
    draw = ImageDraw.Draw(img)
    
    # Colors
    white = (255, 255, 255, 255)
    
    # Simple thick white border
    border_width = max(3, size//20)
    draw.rectangle([0, 0, size-1, size-1], outline=white, width=border_width)
    
    # Simple crosshair - thick white lines
    center = size // 2
    line_thickness = max(4, size//16)
    gap = max(8, size//8)
    
    # Horizontal line (with gap in center)
    left_end = center - gap//2
    right_start = center + gap//2
    
    if left_end > border_width + 2:
        draw.rectangle([border_width + 2, center - line_thickness//2, 
                       left_end, center + line_thickness//2], fill=white)
    if right_start < size - border_width - 2:
        draw.rectangle([right_start, center - line_thickness//2, 
                       size - border_width - 2, center + line_thickness//2], fill=white)
    
    # Vertical line (with gap in center)  
    top_end = center - gap//2
    bottom_start = center + gap//2
    
    if top_end > border_width + 2:
        draw.rectangle([center - line_thickness//2, border_width + 2,
                       center + line_thickness//2, top_end], fill=white)
    if bottom_start < size - border_width - 2:
        draw.rectangle([center - line_thickness//2, bottom_start,
                       center + line_thickness//2, size - border_width - 2], fill=white)
    
    # Small center circle
    circle_size = max(2, size//24)
    draw.ellipse([center - circle_size, center - circle_size,
                 center + circle_size, center + circle_size], outline=white, width=2)
    
    return img

def main():
    # Create different sizes for the icon
    sizes = [16, 24, 32, 48, 64, 128, 256]
    
    # Create PNG icon
    icon_64 = create_simple_crosshair_icon(64)
    icon_64.save('resources/icons/app_icon.png', 'PNG')
    print("Created resources/icons/app_icon.png")
    
    # Create ICO file with multiple sizes for Windows
    icons = []
    for size in sizes:
        icons.append(create_simple_crosshair_icon(size))
    
    # Save as ICO
    icons[0].save('resources/icons/app_icon.ico', format='ICO', 
                  sizes=[(size, size) for size in sizes])
    print("Created resources/icons/app_icon.ico")
    
    print("Simple icons created successfully!")

if __name__ == "__main__":
    main()