#include "Flag.h"

Surface Flag::flags = Surface("assets/flags.png");
int Flag::f_width = flags.GetWidth();

Flag::Flag(Terrains_t terrain)
{
	this->terrain = terrain;
	object = Objects_t::Flag;
	ox = 0, oy = 0;
	cx = TILE / 2, cy = 20, dx = 6, dy = 16;
}

void Flag::Draw(int x, int y, Surface& screen)
{
    // Draw Terrain
    int tx = terrains.at(terrain).first;
    int ty = terrains.at(terrain).second;
    DrawTile(tx, ty, screen, x, y);

    // Draw Object
    DrawObjectOnly(x, y - TILE, screen);
}

void Flag::DrawObjectOnly(int x, int y, Surface& screen)
{
    int s_width = screen.GetWidth(), s_height = screen.GetHeight();
    Pixel* src = flags.GetBuffer() + ox * TILE + (oy * TILE) * f_width;
    Pixel* dst = screen.GetBuffer() + x + y * s_width;
    for (int i = 0; i < 2 * TILE && i + y < s_height; i++, src += f_width, dst += s_width)
        for (int j = 0; j < TILE && j + x < s_width; j++)
            // Skip alpha and draw only visible pixels
            if (j + x >= 0 && i + y >= 0 && (src[j] & (0xff << 24)) != 0) dst[j] = src[j];
}

void Flag::DrawTile(int tx, int ty, Surface& screen, int x, int y)
{
    int s_width = screen.GetWidth(), s_height = screen.GetHeight();
    Pixel* src = tiles.GetBuffer() + tx * TILE + (ty * TILE) * t_width;
    Pixel* dst = screen.GetBuffer() + x + y * s_width;
    for (int i = 0; i < TILE && i + y < s_height; i++, src += t_width, dst += s_width)
        for (int j = 0; j < TILE && j + x < s_width; j++)
            // Skip alpha and draw only visible pixels
            if (j + x >= 0 && i + y >= 0 && (src[j] & (0xff << 24)) != 0) dst[j] = src[j];
}
