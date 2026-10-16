#pragma once

// ROM addresses for BPRE 1.0 (taken from pokefirered.sym).
//   ADDR_CB2_INIT_MAIN_MENU : where the title screen hands off on A/Start.
// The hook address (CB2_InitTitleScreen = 08078914) is in the `hooks` file.
#define ADDR_CB2_INIT_MAIN_MENU 0x0800C300 // CB2_InitMainMenu (pokefirered.sym)

// ---------------------------------------------------------------------------
// Background scene: which image from fullimages.h (0 = Image0, 1 = Image1).
// ---------------------------------------------------------------------------
#define TITLE_IMAGE_INDEX 0

// ---------------------------------------------------------------------------
// Logo layer: the "Pokemon" wordmark, 256-colour art from title_assets/logo_source.png.
// To use different art, run:  python tools/make_title_logo.py <your logo.png>
// (see the top of that script; the key colour = transparent). 0 = off.
// ---------------------------------------------------------------------------
#define TITLE_LOGO 1
#define TITLE_LOGO_DX 0     // nudge the logo (pixels, + = right / down)
#define TITLE_LOGO_DY 0

// ---------------------------------------------------------------------------
// Editable sub-title under the logo ("FIRERED" / "VERSION"), drawn as text with an
// outline and a white-to-silver gradient. Rename your hack here.
//   1 = draw this text (the logo then uses its wordmark-only art)
//   0 = keep the original baked-in banner image instead (text below is ignored)
// ---------------------------------------------------------------------------
#define TITLE_SUBTITLE        1
#define TITLE_SUBTITLE_LINE1  "FIRERED"    // 2x size
#define TITLE_SUBTITLE_LINE2  "VERSION"    // 1x size
#define TITLE_SUBTITLE_ROW    7            // tile row of the block's top (7 = pixel row 56)
#define TITLE_SUBTITLE_SPACING1 0          // extra pixels between letters, line 1
#define TITLE_SUBTITLE_SPACING2 2          // line 2 (small text needs room for its outline)
#define TITLE_SUBTITLE_OUTLINE (1 | (1 << 5) | (2 << 10))        // near-black
#define TITLE_SUBTITLE_TOP     (31 | (31 << 5) | (31 << 10))     // upper half of the letters
#define TITLE_SUBTITLE_BOTTOM  (22 | (24 << 5) | (27 << 10))     // lower half (silver)

// ---------------------------------------------------------------------------
// Text band along the bottom: a blinking prompt plus a copyright line.
// Both strings are plain C strings, edit them freely. Supported characters:
// printable ASCII, plus the (c) sign and e-acute (as UTF-8 or Latin-1).
// 0 = off (the scene's own baked-in bottom strip is then visible).
// ---------------------------------------------------------------------------
#define TITLE_TEXT 1

#define TITLE_PROMPT_TEXT     "PRESS START"        // blinks; use it for any important message
#define TITLE_PROMPT_SCALE    2                    // 1 or 2 (2 = double size, like the original)
#define TITLE_BLINK_SHIFT     5                    // blink speed: on/off every (1 << n) frames; 5 = 32 frames
#define TITLE_COPYRIGHT_TEXT  "©2004 GAME FREAK inc."   // static, 1x size

// Colour of the band behind the text. Must match the scene's bottom strip so the
// band is invisible (RGB 0,24,57 in Image0.png -> 5-bit 0,3,7).
#define TITLE_BAND_COLOR      (0 | (3 << 5) | (7 << 10))
#define TITLE_TEXT_COLOR      (31 | (31 << 5) | (31 << 10))

// ---------------------------------------------------------------------------
// Animated affine Pokemon (its normal front sprite from the ROM, so no art needed).
// Set TITLE_MON_SPECIES to 0 to disable it. Use any SPECIES_* from constants/species.h.
// ---------------------------------------------------------------------------
#define TITLE_MON_SPECIES SPECIES_SKITTY
#define TITLE_MON_X       172   // sprite CENTRE, screen pixels (screen is 240x160)
#define TITLE_MON_Y       108

// It idles perfectly still (exact 1:1 pixels, so it stays crisp), then does a short
// hop/squash/sway every few seconds. Affine scaling is nearest-neighbour, so on
// pixel art smaller values = cleaner pixels. Set any of them to 0 to remove that motion.
#define TITLE_MON_BREATHE 12    // squash/stretch, in 1/256ths (12 = about 5%)
#define TITLE_MON_SWAY    3     // rotation, in 1/256ths of a turn (3 = about 4 degrees)
#define TITLE_MON_BOB     4     // hop height in pixels
#define TITLE_MON_PULSE_PERIOD 96   // frames between hops (96 = 1.6 s)
#define TITLE_MON_PULSE_START  56   // frame within the period where the hop begins
#define TITLE_MON_PULSE_LEN    36   // how long the hop lasts, in frames

// Soft translucent ground shadow under the Pokemon (helps it sit on the scene).
#define TITLE_MON_SHADOW    1
#define TITLE_MON_SHADOW_DY 24  // shadow centre, pixels below the Pokemon's centre

// ---------------------------------------------------------------------------
// Continue + cry. The cry plays when the player presses a key to continue.
// ---------------------------------------------------------------------------
// Keys that continue to the game. Default = any button. For vanilla behaviour use
// (A_BUTTON | START_BUTTON).
#define TITLE_CONTINUE_KEYS (A_BUTTON | START_BUTTON)

// Any species' cry (independent of the sprite). 0 = no cry (the select beep plays instead).
#define TITLE_CRY_SPECIES TITLE_MON_SPECIES
// 1 = after the fade-out, wait for the cry to finish before the main menu (screen stays
//     black meanwhile, max ~4 s). 0 = go straight to the main menu and let the cry play over it.
#define TITLE_CRY_WAIT    1
// 1 = also play the select beep on the key press (it overlaps the cry).
#define TITLE_PRESS_SE    0
