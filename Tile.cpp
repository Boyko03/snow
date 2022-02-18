#include "Tile.h"
#include "surface.h"


Surface Tile::tiles = Surface("assets/IceTileset.png");
int Tile::t_width = tiles.GetWidth();

map<Tile::Terrains_t, pair<int, int>> Tile::terrains {
        {Terrains_t::Snow, pair<int, int>(0, 2)},
        {Terrains_t::Ice, pair<int, int>(1, 2)},
        {Terrains_t::Gravel, pair<int, int>(2, 2)},
        {Terrains_t::CobbleStone, pair<int, int>(3, 2)},
};

void Tile::Draw(int x, int y, Surface& screen)
{
    // Draw Terrain
    int tx = terrains.at(terrain).first;
    int ty = terrains.at(terrain).second;
    DrawTile(tx, ty, screen, x, y);

    // Draw Object
    DrawObjectOnly(x, y, screen);
}

void Tile::DrawObjectOnly(int x, int y, Surface& screen)
{
    // Draw Object If Exists
    if (object != Objects_t::None) {
        DrawTile(ox, oy, screen, x, y);
    }
}

void Tile::DrawTile(int tx, int ty, Surface& screen, int x, int y)
{
    int s_width = screen.GetWidth(), s_height = screen.GetHeight();
    Pixel* src = tiles.GetBuffer() + tx * TILE + (ty * TILE) * t_width;
    Pixel* dst = screen.GetBuffer() + x + y * s_width;
    for (int i = 0; i < TILE && i + y < s_height; i++, src += t_width, dst += s_width)
        for (int j = 0; j < TILE && j + x < s_width; j++)
            // Skip alpha and draw only visible pixels
            if (j + x >= 0 && i + y >= 0 && (src[j] & (0xff << 24)) != 0) dst[j] = src[j];
}
