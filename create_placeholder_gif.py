#!/usr/bin/env python3
"""Create a larger, visible placeholder GIF"""
import struct

def create_gif_file(filename, width=800, height=600, color=(100, 150, 200)):
    """Create a minimal but visible GIF with solid color"""
    gif_data = bytearray()
    gif_data += b'GIF89a'
    
    # Logical screen descriptor
    gif_data += struct.pack('<H', width)
    gif_data += struct.pack('<H', height)
    gif_data += bytes([0xF0])  # Global color table flag
    gif_data += bytes([0x00])  # Background
    gif_data += bytes([0x00])  # Aspect ratio
    
    # Global color table (256 colors)
    for i in range(256):
        if i == 0:
            gif_data += bytes([255, 255, 255])  # White background
        elif i == 1:
            gif_data += bytes(color)  # Main color
        else:
            gif_data += bytes([200, 200, 200])  # Gray
    
    # Image data block
    gif_data += bytes([0x2C])  # Image separator
    gif_data += struct.pack('<H', 0)
    gif_data += struct.pack('<H', 0)
    gif_data += struct.pack('<H', width)
    gif_data += struct.pack('<H', height)
    gif_data += bytes([0x00])  # No local color table
    
    gif_data += bytes([0x08])  # LZW minimum code size
    gif_data += bytes([0x02, 0x00, 0x01])  # Minimal image data
    gif_data += bytes([0x00])  # Block terminator
    
    # Trailer
    gif_data += bytes([0x3B])
    
    with open(filename, 'wb') as f:
        f.write(gif_data)
    print(f'Created: {filename} ({len(gif_data)} bytes)')

if __name__ == '__main__':
    create_gif_file('d:/studium/tsunami_lab/sphinx/source/chapters/paraview_height_animation.gif')
