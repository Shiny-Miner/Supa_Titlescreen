#include "../include/global.h" 
#include "../include/fullimages.h" 
#include "../include/bg.h"
#include "../include/data.h"
#include "../include/decompress.h"
#include "../include/event_data.h"
#include "../include/gpu_regs.h"
#include "../include/graphics.h"
#include "../include/item.h"
#include "../include/list_menu.h"
#include "../include/international_string_util.h"
#include "../include/main.h"
#include "../include/malloc.h"
#include "../include/menu.h"
#include "../include/menu_helpers.h" 
#include "../include/palette.h"
#include "../include/party_menu.h"
#include "../include/scanline_effect.h"
#include "../include/script.h"
#include "../include/sound.h"
#include "../include/string_util.h"
#include "../include/strings.h"
#include "../include/task.h"
#include "../include/text_window.h"
#include "../include/event_data.h"
#include "../include/constants/items.h"
#include "../include/constants/field_weather.h"
#include "../include/constants/songs.h"
#include "../include/constants/flags.h"
#include "../include/gba/types.h"
#include "../include/new/Vanilla_functions.h"

#define VAR_FULL_IMAGE 0x8000

#define tilemapbuffer (*((u8**) 0x203E038)) 


struct ImageData 
{
  u8 *tiles;
  u8 *tilemap; 
  u16 *pal;
};

const struct ImageData ImageDataTable[] =
{
  {
    .tiles = Image0Tiles,
    .tilemap = Image0Map,
    .pal = Image0Pal
  },
  {
    .tiles = Image1Tiles,
    .tilemap = Image1Map,
    .pal = Image1Pal
  }
};

static void LoadImage(u16 image) 
{
  u8 *tiles, *map;
  u16 *palette;
  tiles = ImageDataTable[image].tiles; 
  map = ImageDataTable[image].tilemap;
  palette = ImageDataTable[image].pal;
	DecompressAndCopyTileDataToVram(0, tiles, 0, 0, 0);
	LZDecompressWram(map, tilemapbuffer);
	LoadPalette(palette, 0, 0x20);  
} 

void ShowImage(void) 
{ 
  DmaFill16(3, 0, VRAM, VRAM_SIZE);
    DmaFill32(3, 0, OAM, OAM_SIZE);
    DmaFill16(3, 0, PLTT, PLTT_SIZE);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    SetGpuReg(REG_OFFSET_BG3CNT, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG2CNT, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG1CNT, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG0CNT, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG3HOFS, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG3VOFS, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG2HOFS, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG2VOFS, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG1HOFS, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG1VOFS, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG0HOFS, DISPCNT_MODE_0);
    SetGpuReg(REG_OFFSET_BG0VOFS, DISPCNT_MODE_0);
  tilemapbuffer = Malloc(0x1000);
  CleanupOverworldWindowsAndTilemaps();
  SetBgTilemapBuffer(0, tilemapbuffer); 
  LoadImage(VarGet(VAR_FULL_IMAGE));
  ShowBg(0); 
    CopyBgTilemapBufferToVram(0);
}
