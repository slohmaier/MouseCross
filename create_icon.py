#!/usr/bin/env python3

import sys
try:
    from PIL import Image, ImageDraw
except ImportError:
    print("PIL (Pillow) is required to generate icons.")
    print("Install it with: pip install Pillow")
    sys.exit(1)

def create_crosshair_icon(size=64):
    # Create image with solid background for better visibility
    img = Image.new('RGBA', (size, size), (44, 62, 80, 255))
    draw = ImageDraw.Draw(img)
    
    # Colors
    bg_color = (44, 62, 80, 255)  # Dark blue-gray background
    border_color = (255, 255, 255, 255)  # White border
    crosshair_color = (255, 255, 255, 255)  # White crosshair
    
    # Calculate dimensions
    center = size // 2
    margin = max(2, size // 32)
    corner_radius = max(4, size // 16)
    line_width = max(2, size // 32)
    clearance = max(6, size // 10)
    line_length = center - margin - 8
    
    # Draw rounded rectangle background
    draw.rounded_rectangle([margin, margin, size-margin, size-margin], 
                          radius=corner_radius, fill=bg_color, outline=border_color, width=max(2, size//32))
    
    # Draw crosshair lines extending to edges with gap in center
    # Horizontal lines (ensure x2 > x1)
    left_x2 = center - clearance
    if left_x2 > margin + 4:
        draw.rectangle([margin + 4, center - line_width//2, 
                       left_x2, center + line_width//2], 
                       fill=crosshair_color)
    
    right_x1 = center + clearance
    if right_x1 < size - margin - 4:
        draw.rectangle([right_x1, center - line_width//2, 
                       size - margin - 4, center + line_width//2], 
                       fill=crosshair_color)
    
    # Vertical lines (ensure y2 > y1)
    top_y2 = center - clearance
    if top_y2 > margin + 4:
        draw.rectangle([center - line_width//2, margin + 4, 
                       center + line_width//2, top_y2], 
                       fill=crosshair_color)
    
    bottom_y1 = center + clearance
    if bottom_y1 < size - margin - 4:
        draw.rectangle([center - line_width//2, bottom_y1, 
                       center + line_width//2, size - margin - 4], 
                       fill=crosshair_color)
    
    # Draw center circle to indicate cursor position
    circle_radius = max(2, size // 24)
    draw.ellipse([center - circle_radius, center - circle_radius,
                 center + circle_radius, center + circle_radius], 
                outline=crosshair_color, width=max(1, size // 64))
    
    # Draw corner indicators for better visibility
    corner_size = max(2, size // 32)
    corner_offset = max(8, size // 16)
    
    # Top-left corner
    draw.rectangle([center - corner_offset - corner_size, center - corner_offset,
                   center - corner_offset, center - corner_offset], fill=crosshair_color)
    draw.rectangle([center - corner_offset, center - corner_offset - corner_size,
                   center - corner_offset, center - corner_offset], fill=crosshair_color)
    
    # Top-right corner  
    draw.rectangle([center + corner_offset, center - corner_offset,
                   center + corner_offset + corner_size, center - corner_offset], fill=crosshair_color)
    draw.rectangle([center + corner_offset, center - corner_offset - corner_size,
                   center + corner_offset, center - corner_offset], fill=crosshair_color)
    
    # Bottom-left corner
    draw.rectangle([center - corner_offset - corner_size, center + corner_offset,
                   center - corner_offset, center + corner_offset], fill=crosshair_color)
    draw.rectangle([center - corner_offset, center + corner_offset,
                   center - corner_offset, center + corner_offset + corner_size], fill=crosshair_color)
    
    # Bottom-right corner
    draw.rectangle([center + corner_offset, center + corner_offset,
                   center + corner_offset + corner_size, center + corner_offset], fill=crosshair_color)
    draw.rectangle([center + corner_offset, center + corner_offset,
                   center + corner_offset, center + corner_offset + corner_size], fill=crosshair_color)
    
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