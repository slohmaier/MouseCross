#!/usr/bin/env python3
"""
Creates a background image for the DMG installer
"""

from PIL import Image, ImageDraw, ImageFont
import os

def create_dmg_background():
    # DMG window size
    width, height = 640, 400
    
    # Create background with light blue gradient
    img = Image.new('RGB', (width, height), '#f0f4f8')
    draw = ImageDraw.Draw(img)
    
    # Create subtle gradient
    for y in range(height):
        alpha = int(255 * (1 - y / height * 0.3))
        color = (240 + int(15 * y / height), 244 + int(11 * y / height), 248 + int(7 * y / height))
        draw.line([(0, y), (width, y)], fill=color)
    
    # Add instructional text
    try:
        # Try to use system font
        font_large = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 24)
        font_small = ImageFont.truetype("/System/Library/Fonts/Helvetica.ttc", 16)
    except:
        # Fallback to default font
        font_large = ImageFont.load_default()
        font_small = ImageFont.load_default()
    
    # Main instruction text
    instruction = "Drag MouseCross to the Applications folder"
    
    # Get text size
    bbox = draw.textbbox((0, 0), instruction, font=font_large)
    text_width = bbox[2] - bbox[0]
    text_x = (width - text_width) // 2
    text_y = height - 80
    
    # Draw text with shadow
    draw.text((text_x + 1, text_y + 1), instruction, fill='#888888', font=font_large)
    draw.text((text_x, text_y), instruction, fill='#333333', font=font_large)
    
    # Add MouseCross branding
    brand_text = "MouseCross - Visual Mouse Locator"
    bbox = draw.textbbox((0, 0), brand_text, font=font_small)
    brand_width = bbox[2] - bbox[0]
    brand_x = (width - brand_width) // 2
    brand_y = 30
    
    draw.text((brand_x + 1, brand_y + 1), brand_text, fill='#cccccc', font=font_small)
    draw.text((brand_x, brand_y), brand_text, fill='#666666', font=font_small)
    
    # Draw arrow pointing from app position to Applications
    # App will be positioned at ~160, ~200
    # Applications will be positioned at ~480, ~200
    arrow_start_x, arrow_start_y = 300, 200
    arrow_end_x, arrow_end_y = 440, 200
    
    # Draw arrow shaft
    draw.line([(arrow_start_x, arrow_start_y), (arrow_end_x - 20, arrow_end_y)], 
             fill='#4a90e2', width=3)
    
    # Draw arrowhead
    draw.polygon([
        (arrow_end_x, arrow_end_y),
        (arrow_end_x - 20, arrow_end_y - 10),
        (arrow_end_x - 20, arrow_end_y + 10)
    ], fill='#4a90e2')
    
    return img

if __name__ == "__main__":
    # Create deployment directory if it doesn't exist
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Create background image
    bg_img = create_dmg_background()
    
    # Save the image
    output_path = os.path.join(script_dir, "dmg_background.png")
    bg_img.save(output_path, "PNG")
    print(f"Created DMG background: {output_path}")
    
    # Also create a high-DPI version
    bg_img_hires = bg_img.resize((1280, 800), Image.LANCZOS)
    output_path_hires = os.path.join(script_dir, "dmg_background@2x.png")
    bg_img_hires.save(output_path_hires, "PNG")
    print(f"Created high-DPI DMG background: {output_path_hires}")