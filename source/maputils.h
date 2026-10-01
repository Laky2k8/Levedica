#include <grrlib.h>
#include <cmath>
#include <cstdint>
#include <vector>
#include <algorithm>
#include "defs.h"

uint16_t rgb_to_16bit(uint8_t r, uint8_t g, uint8_t b) 
{
    uint16_t r_bits = (r * 31 + 127) / 255;
    uint16_t g_bits = (g * 63 + 127) / 255;
    uint16_t b_bits = (b * 31 + 127) / 255;
    
    return (r_bits << 11) | (g_bits << 5) | b_bits;
}

GRRLIB_texImg* prerenderMap(GRRLIB_texImg *source_map, u32 provinceColor, u32 borderColor, u32 coastlineColor, int borderThickness)
{
	if(!source_map)
	{
		printf("Source map is empty!");
		return NULL;
	}

	u32 width = source_map->w;
	u32 height = source_map->h;

	GRRLIB_texImg *output_map = GRRLIB_CreateEmptyTexture(width,height);
	if(!output_map)
	{
		printf("Failed to create new image!");
		return NULL;
	}

	// neighbor offsets
	const int offset_x[4] = { 1, -1,  0,  0 };
    const int offset_y[4] = { 0,  0,  1, -1 };

	for(u32 y = 0; y < height; y++)
	{
		for(u32 x = 0; x < width; x++)
		{
			u32 current_pixel = GRRLIB_GetPixelFromtexImg(x, y, source_map);

            u8 r = (current_pixel >> 24) & 0xFF;
            u8 g = (current_pixel >> 16) & 0xFF;
            u8 b = (current_pixel >> 8)  & 0xFF;

			// skip background rendering
			if(r == 255 && g == 255 && b == 255)
			{
				GRRLIB_SetPixelTotexImg(x, y, output_map, GRRLIB_EMPTY);
				continue;
			}

			// neighbor check (for borders)
			bool isBorder = false;
			bool isCoastline = false;

			for(int dy = -borderThickness; dy <= borderThickness; dy++)
			{
				for(int dx = -borderThickness; dx <= borderThickness; dx++)
				{

					// Skip checking the pixel against itself
					if(dx == 0 && dy == 0) continue;

					if(dx*dx + dy*dy > borderThickness*borderThickness) continue;

					int neighbor_x = static_cast<int>(x) + dx;
					int neighbor_y = static_cast<int>(y) + dy;

					if(neighbor_x >= 0 && neighbor_x < static_cast<int>(width) && neighbor_y >= 0 && neighbor_y < static_cast<int>(height)) // if neighbor pixel is in bounds
					{
						u32 neighbor_pixel = GRRLIB_GetPixelFromtexImg(neighbor_x, neighbor_y, source_map);

						if(neighbor_pixel != current_pixel)
						{

							// check if the neighbor is a coastline
							u8 neighbor_r = (neighbor_pixel >> 24) & 0xFF;
							u8 neighbor_g = (neighbor_pixel >> 16) & 0xFF;
							u8 neighbor_b = (neighbor_pixel >> 8) & 0xFF;

							if(neighbor_r == 255 && neighbor_g == 255 && neighbor_b == 255)
							{
								isCoastline = true;
							}
							else if(current_pixel > neighbor_pixel)
							{
								isBorder = true;
							}
						}
					}
					else
					{
						// pixels neighbouring the bounds are obviously borders lol
						isBorder = true;
					}
				}
			}

			if(isCoastline)
			{
				GRRLIB_SetPixelTotexImg(x, y, output_map, coastlineColor);
			}
			else if(isBorder)
			{
				GRRLIB_SetPixelTotexImg(x, y, output_map, borderColor); 
			}
			else
			{
				GRRLIB_SetPixelTotexImg(x, y, output_map, provinceColor);
			}
		}
	}

	GRRLIB_FlushTex(output_map);
    return output_map;

}