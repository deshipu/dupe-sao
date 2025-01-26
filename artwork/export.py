import os
os.environ['PYGAME_HIDE_SUPPORT_PROMPT'] = "hide"
import pygame


frames = "face1", "face2", "face3"
palette = []

print('#include <stdio.h>')
for frame in frames:
    image = pygame.image.load(frame + '.png')
    height = image.get_height()
    width = image.get_width()
    print('// Image %dx%d size %d' % (width, height, width * height * 2))
    print('const uint8_t %s_data[%d] = {' % (frame, width * height // 2))
    byte = -1
    for y in range(height):
        for x in range(width):
            r, g, b, a = image.get_at((x, y))
            rgb565 = ((int(r) & 0xf8) << 8) | ((int(g) & 0xfc) << 3) | (int(b) >> 3)
            try:
                color = palette.index(rgb565)
            except ValueError:
                palette.append(rgb565)
                color = palette.index(rgb565)
            if byte == -1:
                byte = color << 4
            else:
                byte |= color
                print('0x%02x, ' % byte, end='')
                byte = -1
            #print('0x%02x, 0x%02x, ' % (rgb565 >> 8, rgb565 & 0xff), end='')
        print()
    print('};')
print('const uint16_t palette[16] = {')
for color in palette:
    print('0x%04x, ' % color, end='')
print('};')
