
//{{BLOCK(Image1)

//======================================================================
//
//	Image1, 256x160@4, 
//	+ palette 16 entries, not compressed
//	+ 362 tiles (t|f|p reduced) lz77 compressed
//	+ regular map (flat), lz77 compressed, 32x20 
//	Total size: 32 + 5200 + 948 = 6180
//
//	Time-stamp: 2026-10-16, 00:47:42
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.6
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_IMAGE1_H
#define GRIT_IMAGE1_H

#define Image1TilesLen 5200
extern const unsigned char Image1Tiles[5200];

#define Image1MapLen 948
extern const unsigned short Image1Map[474];

#define Image1PalLen 32
extern const unsigned char Image1Pal[32];

#endif // GRIT_IMAGE1_H

//}}BLOCK(Image1)
