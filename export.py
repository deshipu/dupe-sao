import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "hide"
import pygame

image = pygame.image.load("dupe.png")
height = image.get_height()
width = image.get_width()
print('// Image %dx%d size %d' % (width, height, width * height * 2))
print('const unsigned char image_data[%d] = {' % (width * height * 2))
for y in range(height):
    for x in range(width):
        r, g, b, a = image.get_at((x, y))
        rgb565 = ((r & 0x1f) << 11) | ((g & 0x3f) << 5) | (b & 0x1f)
        print('0x%02x, 0x%02x, ' % (rgb565 >> 8, rgb565 & 0xff), end='')
    print()
print('};')
