#include "Tile.h"
#include "surface.h"

Tmpl8::Surface Tile::tiles = Tmpl8::Surface("assets/IceTileset.png");
int Tile::t_width = tiles.GetWidth();

std::map<Tile::Terrains_t, std::pair<int, int>> Tile::terrains{
        {Terrains_t::Snow, std::pair<int, int>(0, 2)},
        {Terrains_t::FinishLine, std::pair<int, int>(4, 2)},
        {Terrains_t::Ice, std::pair<int, int>(1, 2)},
        {Terrains_t::Gravel, std::pair<int, int>(2, 2)},
        {Terrains_t::CobbleStone, std::pair<int, int>(3, 2)},
};

Tile::Tile(int ox, int oy, int cx, int cy, int dx, int dy, Objects_t object, Terrains_t terrain, int tile_width, int tile_height) :
    ox(ox), oy(oy), cx(cx), cy(cy), dx(dx), dy(dy), object(object), terrain(terrain), tile_width(tile_width), tile_height(tile_height)
{
}

void Tile::Draw(int x, int y, Tmpl8::Surface& screen)
{
    // Draw Terrain
    int tx = terrains.at(terrain).first;
    int ty = terrains.at(terrain).second;
    DrawTile(tx, ty, screen, x, y);

    // Draw Object
    DrawObjectOnly(x, y - (tile_height - TILE), screen);
}

void Tile::DrawBackground(int x, int y, Tmpl8::Surface& screen)
{
    int tx = terrains.at(terrain).first;
    int ty = terrains.at(terrain).second;

    DrawTile(tx, ty, screen, x, y);
}

void Tile::DrawForegound(int x, int y, Tmpl8::Surface& screen)
{
    DrawObjectOnly(x, y - (tile_height - TILE), screen);
}

void Tile::DrawObjectOnly(int x, int y, Tmpl8::Surface& screen)
{
    // Draw Object If Exists
    if (object != Objects_t::None) {
        DrawTile(ox, oy, screen, x, y, true);
    }
}

void Tile::DrawTile(int tx, int ty, Tmpl8::Surface& screen, int x, int y, bool is_object)
{
    int s_width = screen.GetWidth(), s_height = screen.GetHeight();
    int width = is_object ? tile_width : TILE;
    int height = is_object ? tile_height : TILE;
    Tmpl8::Pixel* src = tiles.GetBuffer() + tx * TILE + (ty * TILE) * t_width;
    Tmpl8::Pixel* dst = screen.GetBuffer() + x + y * s_width;
    for (int i = 0; i < height && i + y < s_height; i++, src += t_width, dst += s_width)
        for (int j = 0; j < width && j + x < s_width; j++)
            // Skip alpha and draw only visible pixels
            if (j + x >= 0 && i + y >= 0 && (src[j] & (0xff000000)) != 0) dst[j] = src[j];
}