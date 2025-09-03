#!/usr/bin/env python3

import sys
try:
    from PIL import Image, ImageDraw
except ImportError:
    print("PIL (Pillow) is required to generate icons.")
    print("Install it with: pip install Pillow")
    sys.exit(1)

def create_crosshair_icon(size=64):
    # Create a transparent image
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Colors
    bg_color = (255, 255, 255, 200)  # Semi-transparent white background
    crosshair_color = (255, 0, 0, 255)  # Red crosshair
    border_color = (128, 128, 128, 255)  # Gray border
    
    # Calculate dimensions
    center = size // 2
    radius = size // 2 - 4
    line_width = max(2, size // 16)
    clearance = max(4, size // 8)
    line_length = max(8, size // 4)
    
    # Draw background circle with border
    draw.ellipse([2, 2, size-2, size-2], fill=bg_color, outline=border_color, width=2)
    
    # Draw crosshair
    # Horizontal lines
    draw.rectangle([center - clearance - line_length, center - line_width//2, 
                   center - clearance, center + line_width//2], 
                   fill=crosshair_color)
    draw.rectangle([center + clearance, center - line_width//2, 
                   center + clearance + line_length, center + line_width//2], 
                   fill=crosshair_color)
    
    # Vertical lines
    draw.rectangle([center - line_width//2, center - clearance - line_length, 
                   center + line_width//2, center - clearance], 
                   fill=crosshair_color)
    draw.rectangle([center - line_width//2, center + clearance, 
                   center + line_width//2, center + clearance + line_length], 
                   fill=crosshair_color)
    
    return img

def main():
    # Create different sizes for the icon
    sizes = [16, 32, 48, 64, 128, 256]
    
    # Create PNG icon
    icon_64 = create_crosshair_icon(64)
    icon_64.save('resources/icons/app_icon.png', 'PNG')
    print("Created resources/icons/app_icon.png")
    
    # Create ICO file with multiple sizes for Windows
    icons = []
    for size in sizes:
        icons.append(create_crosshair_icon(size))
    
    # Save as ICO
    icons[0].save('resources/icons/app_icon.ico', format='ICO', 
                  sizes=[(size, size) for size in sizes])
    print("Created resources/icons/app_icon.ico")
    
    print("Icons created successfully!")

if __name__ == "__main__":
    main()