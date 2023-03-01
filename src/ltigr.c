#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <lua.h>
#include <lauxlib.h>
#include "moon.h"
#include "compat-5.3.h"

#include "tigr.h"


/* pixel format conversion functions */
static inline uint32_t rgba2p( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
  return (uint32_t)r | (((uint32_t)g) << 8) | (((uint32_t)b) << 16) | (((uint32_t)a) << 24);
}

static inline uint8_t p2r( uint32_t p )
{
  return p & 0xFFu;
}

static inline uint8_t p2g( uint32_t p )
{
  return (p >> 8) & 0xFFu;
}

static inline uint8_t p2b( uint32_t p )
{
  return (p >> 16) & 0xFFu;
}

static inline uint8_t p2a( uint32_t p )
{
  return (p >> 24) & 0xFFu;
}

static inline TPixel p2tp( uint32_t p ) /* pixel to Tigr pixel */
{
  return tigrRGBA( p2r( p ), p2g( p ), p2b( p ), p2a( p ) );
}

static inline uint32_t tp2p( TPixel tp ) /* Tigr pixel to pixel */
{
  return rgba2p( tp.r, tp.g, tp.b, tp.a );
}


static inline TPixel check_pixel( lua_State* L, int idx )
{
  uint32_t p = moon_checkint( L, idx, 0, UINT32_MAX );
  return p2tp( p );
}


static char const* const ltigr_window_option_names[] = {
  "fixed",
  "auto",
  "2x",
  "3x",
  "4x",
  "retina",
  "nocursor",
  "fullscreen",
  NULL
};

static int const ltigr_window_option_values[] = {
  TIGR_FIXED,
  TIGR_AUTO,
  TIGR_2X,
  TIGR_3X,
  TIGR_4X,
  TIGR_RETINA,
  TIGR_NOCURSOR,
  TIGR_FULLSCREEN,
};


static char const* const ltigr_blitmode_names[] = {
  "keep_alpha",
  "blend_alpha",
  NULL
};

static enum TIGRBlitMode const ltigr_blitmode_values[] = {
  TIGR_KEEP_ALPHA,
  TIGR_BLEND_ALPHA,
};


static void ltigr_free( void* p )
{
  tigrFree( p );
}


static int ltigr_window( lua_State* L )
{
  int width = moon_checkint( L, 1, 0, INT_MAX );
  int height = moon_checkint( L, 2, 0, INT_MAX );
  char const* title = luaL_checkstring( L, 3 );
  int flags = 0;
  int i = 0;
  for( i = 4; i <= lua_gettop( L ); ++i )
  {
    flags |= ltigr_window_option_values[
      luaL_checkoption( L, i, NULL, ltigr_window_option_names )
    ];
  }
  {
    void** p = moon_newpointer( L, "tigrWindow", ltigr_free );
    *p = tigrWindow( width, height, title, flags );
    if( !*p )
    {
      luaL_error( L, "error creating tigrWindow" );
    }
  }
  return 1;
}


static int ltigr_closed( lua_State* L )
{
  Tigr* window = moon_checkobject( L, 1, "tigrWindow" );
  lua_pushboolean( L, tigrClosed( window ) );
  return 1;
}


static int ltigr_update( lua_State* L )
{
  Tigr* window = moon_checkobject( L, 1, "tigrWindow" );
  tigrUpdate( window );
  return 0;
}


static int ltigr_bitmap( lua_State* L )
{
  int width = moon_checkint( L, 1, 0, INT_MAX );
  int height = moon_checkint( L, 2, 0, INT_MAX );
  void** p = moon_newpointer( L, "tigrBitmap", ltigr_free );
  *p = tigrBitmap( width, height );
  if( !*p )
  {
    luaL_error( L, "error creating tigrBitmap" );
  }
  return 1;
}


static void* ltigr_window_to_bitmap( void* p )
{
  return p; /* no pointer conversion necessary */
}


static int ltigr_bitmap_w( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  if( lua_gettop( L ) < 3 )
  {
    /* __index */
    lua_pushinteger( L, bitmap->w );
    return 1;
  }
  else
  {
    /* __newindex */
    luaL_error( L, "attempt to set read-only property 'w'" );
    return 0;
  }
}


static int ltigr_bitmap_h( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  if( lua_gettop( L ) < 3 )
  {
    /* __index */
    lua_pushinteger( L, bitmap->h );
    return 1;
  }
  else
  {
    /* __newindex */
    luaL_error( L, "attempt to set read-only property 'h'" );
    return 0;
  }
}


static int ltigr_bitmap_cx( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  if( lua_gettop( L ) < 3 )
  {
    /* __index */
    lua_pushinteger( L, bitmap->cx );
    return 1;
  }
  else
  {
    /* __newindex */
    luaL_error( L, "attempt to set read-only property 'cx'" );
    return 0;
  }
}


static int ltigr_bitmap_cy( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  if( lua_gettop( L ) < 3 )
  {
    /* __index */
    lua_pushinteger( L, bitmap->cy );
    return 1;
  }
  else
  {
    /* __newindex */
    luaL_error( L, "attempt to set read-only property 'cy'" );
    return 0;
  }
}


static int ltigr_bitmap_cw( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  if( lua_gettop( L ) < 3 )
  {
    /* __index */
    lua_pushinteger( L, bitmap->cw );
    return 1;
  }
  else
  {
    /* __newindex */
    luaL_error( L, "attempt to set read-only property 'cw'" );
    return 0;
  }
}


static int ltigr_bitmap_ch( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  if( lua_gettop( L ) < 3 )
  {
    /* __index */
    lua_pushinteger( L, bitmap->ch );
    return 1;
  }
  else
  {
    /* __newindex */
    luaL_error( L, "attempt to set read-only property 'ch'" );
    return 0;
  }
}


static int ltigr_bitmap_blitmode( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  if( lua_gettop( L ) < 3 )
  {
    /* __index */
    size_t i = 0;
    for( i = 0; i < (sizeof( ltigr_blitmode_values )/sizeof( *ltigr_blitmode_values )); ++i )
    {
      if( bitmap->blitMode == ltigr_blitmode_values[ i ] )
      {
        lua_pushstring( L, ltigr_blitmode_names[ i ] );
        return 1;
      }
    }
#if 1
    /* raising an error here is probably safest */
    luaL_error( L, "unknown blitmode in bitmap/window" );
#endif
    lua_pushnil( L );
    return 1;
  }
  else
  {
    /* __newindex */
    enum TIGRBlitMode mode = ltigr_blitmode_values[
      luaL_checkoption( L, 3, NULL, ltigr_blitmode_names )
    ];
    tigrBlitMode( bitmap, mode );
    return 0;
  }
}


static int ltigr_get( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x = moon_checkint( L, 2, 0, INT_MAX );
  int y = moon_checkint( L, 3, 0, INT_MAX );
  lua_pushinteger( L, tp2p( tigrGet( bitmap, x, y ) ) );
  return 1;
}


static int ltigr_plot( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x = moon_checkint( L, 2, 0, INT_MAX );
  int y = moon_checkint( L, 3, 0, INT_MAX );
  TPixel pixel = check_pixel( L, 4 );
  tigrPlot( bitmap, x, y, pixel );
  return 0;
}


static int ltigr_clear( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  TPixel color = check_pixel( L, 2 );
  tigrClear( bitmap, color );
  return 0;
}


static int ltigr_fill( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x = moon_checkint( L, 2, 0, INT_MAX );
  int y = moon_checkint( L, 3, 0, INT_MAX );
  int w = moon_checkint( L, 4, 0, INT_MAX );
  int h = moon_checkint( L, 5, 0, INT_MAX );
  TPixel color = check_pixel( L, 6 );
  tigrFill( bitmap, x, y, w, h, color );
  return 0;
}


static int ltigr_line( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x0 = moon_checkint( L, 2, 0, INT_MAX );
  int y0 = moon_checkint( L, 3, 0, INT_MAX );
  int x1 = moon_checkint( L, 4, 0, INT_MAX );
  int y1 = moon_checkint( L, 5, 0, INT_MAX );
  TPixel color = check_pixel( L, 6 );
  tigrLine( bitmap, x0, y0, x1, y1, color );
  return 0;
}


static int ltigr_rect( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x = moon_checkint( L, 2, 0, INT_MAX );
  int y = moon_checkint( L, 3, 0, INT_MAX );
  int w = moon_checkint( L, 4, 0, INT_MAX );
  int h = moon_checkint( L, 5, 0, INT_MAX );
  TPixel color = check_pixel( L, 6 );
  tigrRect( bitmap, x, y, w, h, color );
  return 0;
}


static int ltigr_fill_rect( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x = moon_checkint( L, 2, 0, INT_MAX );
  int y = moon_checkint( L, 3, 0, INT_MAX );
  int w = moon_checkint( L, 4, 0, INT_MAX );
  int h = moon_checkint( L, 5, 0, INT_MAX );
  TPixel color = check_pixel( L, 6 );
  tigrFillRect( bitmap, x, y, w, h, color );
  return 0;
}


static int ltigr_circle( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x = moon_checkint( L, 2, 0, INT_MAX );
  int y = moon_checkint( L, 3, 0, INT_MAX );
  int r = moon_checkint( L, 4, 0, INT_MAX );
  TPixel color = check_pixel( L, 5 );
  tigrCircle( bitmap, x, y, r, color );
  return 0;
}


static int ltigr_fill_circle( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int x = moon_checkint( L, 2, 0, INT_MAX );
  int y = moon_checkint( L, 3, 0, INT_MAX );
  int r = moon_checkint( L, 4, 0, INT_MAX );
  TPixel color = check_pixel( L, 5 );
  tigrFillCircle( bitmap, x, y, r, color );
  return 0;
}


static int ltigr_clip( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int cx = moon_checkint( L, 2, 0, INT_MAX );
  int cy = moon_checkint( L, 3, 0, INT_MAX );
  int cw = moon_checkint( L, 4, -1, INT_MAX );
  int ch = moon_checkint( L, 5, -1, INT_MAX );
  tigrClip( bitmap, cx, cy, cw, ch );
  return 0;
}


static int ltigr_blit( lua_State* L )
{
  Tigr* dest = moon_checkobject( L, 1, "tigrBitmap" );
  Tigr* src = moon_checkobject( L, 2, "tigrBitmap" );
  int dx = moon_checkint( L, 3, 0, INT_MAX );
  int dy = moon_checkint( L, 4, 0, INT_MAX );
  int sx = moon_checkint( L, 5, 0, INT_MAX );
  int sy = moon_checkint( L, 6, 0, INT_MAX );
  int w = moon_checkint( L, 7, 0, INT_MAX );
  int h = moon_checkint( L, 8, 0, INT_MAX );
  tigrBlit( dest, src, dx, dy, sx, sy, w, h );
  return 0;
}


static int ltigr_blit_alpha( lua_State* L )
{
  Tigr* dest = moon_checkobject( L, 1, "tigrBitmap" );
  Tigr* src = moon_checkobject( L, 2, "tigrBitmap" );
  int dx = moon_checkint( L, 3, 0, INT_MAX );
  int dy = moon_checkint( L, 4, 0, INT_MAX );
  int sx = moon_checkint( L, 5, 0, INT_MAX );
  int sy = moon_checkint( L, 6, 0, INT_MAX );
  int w = moon_checkint( L, 7, 0, INT_MAX );
  int h = moon_checkint( L, 8, 0, INT_MAX );
  float alpha = (float)luaL_checknumber( L, 9 );
  tigrBlitAlpha( dest, src, dx, dy, sx, sy, w, h, alpha );
  return 0;
}


static int ltigr_blit_tint( lua_State* L )
{
  Tigr* dest = moon_checkobject( L, 1, "tigrBitmap" );
  Tigr* src = moon_checkobject( L, 2, "tigrBitmap" );
  int dx = moon_checkint( L, 3, 0, INT_MAX );
  int dy = moon_checkint( L, 4, 0, INT_MAX );
  int sx = moon_checkint( L, 5, 0, INT_MAX );
  int sy = moon_checkint( L, 6, 0, INT_MAX );
  int w = moon_checkint( L, 7, 0, INT_MAX );
  int h = moon_checkint( L, 8, 0, INT_MAX );
  TPixel tint = check_pixel( L, 9 );
  tigrBlitTint( dest, src, dx, dy, sx, sy, w, h, tint );
  return 0;
}


static int ltigr_blitmode( lua_State* L )
{
  Tigr* dest = moon_checkobject( L, 1, "tigrBitmap" );
  enum TIGRBlitMode mode = ltigr_blitmode_values[
      luaL_checkoption( L, 2, "blend_alpha", ltigr_blitmode_names )
  ];
  tigrBlitMode( dest, mode );
  return 0;
}


static void ltigr_free_font( void* p )
{
  tigrFreeFont( p );
}


static int ltigr_load_font( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  int codepage = moon_checkint( L, 2, 0, INT_MAX );
  void **f = moon_newpointer( L, "tigrFont", ltigr_free_font );
  *f = tigrLoadFont( bitmap, codepage );
  if( !*f )
  {
    luaL_error( L, "error creating/loading tigrFont" );
  }
  return 1;
}


static int ltigr_text_width( lua_State* L )
{
  TigrFont* font = moon_checkobject( L, 1, "tigrFont" );
  char const* text = luaL_checkstring( L, 2 );
  lua_pushinteger( L, tigrTextWidth( font, text ) );
  return 1;
}


static int ltigr_text_height( lua_State* L )
{
  TigrFont* font = moon_checkobject( L, 1, "tigrFont" );
  char const* text = luaL_checkstring( L, 2 );
  lua_pushinteger( L, tigrTextHeight( font, text ) );
  return 1;
}


static int ltigr_print( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  TigrFont* font = moon_checkobject( L, 2, "tigrFont" );
  int x = moon_checkint( L, 3, 0, INT_MAX );
  int y = moon_checkint( L, 4, 0, INT_MAX );
  TPixel color = check_pixel( L, 5 );
  char const* text = luaL_checkstring( L, 6 );
  tigrPrint( bitmap, font, x, y, color, "%s", text );
  return 0;
}


static int ltigr_rgba( lua_State* L )
{
  uint8_t r = moon_checkint( L, 1, 0, 255 );
  uint8_t g = moon_checkint( L, 2, 0, 255 );
  uint8_t b = moon_checkint( L, 3, 0, 255 );
  uint8_t a = lua_isnoneornil( L, 4) ? 0xFFu : moon_checkint( L, 4, 0, 255 );
  lua_pushinteger( L, rgba2p( r, g, b, a ) );
  return 1;
}


static int ltigr_mouse( lua_State* L )
{
  Tigr* window = moon_checkobject( L, 1, "tigrWindow" );
  int x = 0;
  int y = 0;
  int buttons = 0;
  tigrMouse( window, &x, &y, &buttons );
  lua_pushinteger( L, x );
  lua_pushinteger( L, y );
  lua_pushinteger( L, buttons );
  return 3;
}


static int ltigr_touch( lua_State* L )
{
  Tigr* window = moon_checkobject( L, 1, "tigrWindow" );
  TigrTouchPoint points[ 10 ];
  int i = 0;
  int num = tigrTouch( window, points, sizeof( points )/sizeof( *points ) );
  lua_createtable( L, num, 0 );
  for ( i = 0; i < num; ++i )
  {
    lua_createtable( L, 0, 2 );
    lua_pushinteger( L, points[ i ].x );
    lua_setfield( L, -2, "x" );
    lua_pushinteger( L, points[ i ].y );
    lua_setfield( L, -2, "y" );
    lua_seti( L, -2, i+1 );
  }
  return 1;
}


typedef struct {
  char const* keyname;
  TKey keycode;
} ltigr_keycode_mapping;

static void push_keycode_table( lua_State* L )
{
  static ltigr_keycode_mapping const mappings[] = {
    { "pad0", TK_PAD0 },
    { "pad1", TK_PAD1 },
    { "pad2", TK_PAD2 },
    { "pad3", TK_PAD3 },
    { "pad4", TK_PAD4 },
    { "pad5", TK_PAD5 },
    { "pad6", TK_PAD6 },
    { "pad7", TK_PAD7 },
    { "pad8", TK_PAD8 },
    { "pad9", TK_PAD9 },
    { "padmul", TK_PADMUL },
    { "padadd", TK_PADADD },
    { "padenter", TK_PADENTER },
    { "padsub", TK_PADSUB },
    { "paddot", TK_PADDOT },
    { "paddiv", TK_PADDIV },
    { "f1", TK_F1 },
    { "f2", TK_F2 },
    { "f3", TK_F3 },
    { "f4", TK_F4 },
    { "f5", TK_F5 },
    { "f6", TK_F6 },
    { "f7", TK_F7 },
    { "f8", TK_F8 },
    { "f9", TK_F9 },
    { "f10", TK_F10 },
    { "f11", TK_F11 },
    { "f12", TK_F12 },
    { "backspace", TK_BACKSPACE },
    { "tab", TK_TAB },
    { "return", TK_RETURN },
    { "shift", TK_SHIFT },
    { "control", TK_CONTROL },
    { "alt", TK_ALT },
    { "pause", TK_PAUSE },
    { "capslock", TK_CAPSLOCK },
    { "escape", TK_ESCAPE },
    { "space", TK_SPACE },
    { "pageup", TK_PAGEUP },
    { "pagedn", TK_PAGEDN },
    { "end", TK_END },
    { "home", TK_HOME },
    { "left", TK_LEFT },
    { "up", TK_UP },
    { "right", TK_RIGHT },
    { "down", TK_DOWN },
    { "insert", TK_INSERT },
    { "delete", TK_DELETE },
    { "lwin", TK_LWIN },
    { "rwin", TK_RWIN },
    { "numlock", TK_NUMLOCK },
    { "scroll", TK_SCROLL },
    { "lshift", TK_LSHIFT },
    { "rshift", TK_RSHIFT },
    { "lcontrol", TK_LCONTROL },
    { "rcontrol", TK_RCONTROL },
    { "lalt", TK_LALT },
    { "ralt", TK_RALT },
    { "semicolon", TK_SEMICOLON },
    { "equals", TK_EQUALS },
    { "comma", TK_COMMA },
    { "minus", TK_MINUS },
    { "dot", TK_DOT },
    { "slash", TK_SLASH },
    { "backtick", TK_BACKTICK },
    { "lsquare", TK_LSQUARE },
    { "backslash", TK_BACKSLASH },
    { "rsquare", TK_RSQUARE },
    { "tick", TK_TICK },
  };
  size_t i = 0;
  lua_createtable( L, 0, (int)(sizeof( mappings )/sizeof( *mappings )) );
  for( i = 0; i < sizeof( mappings )/sizeof( *mappings ); ++i )
  {
    lua_pushinteger( L, mappings[ i ].keycode );
    lua_setfield( L, -2, mappings[ i ].keyname );
  }
}


static int ltigr_key_down( lua_State* L )
{
  Tigr* window = moon_checkobject( L, 1, "tigrWindow" );
  size_t len = 0;
  char const* keyname = luaL_checklstring( L, 2, &len );
  int keycode = 0;
  lua_settop( L, 2 );
  if( len == 1 && ((keyname[ 0 ] >= 'a' && keyname[ 0 ] <= 'z')
                || (keyname[ 0 ] >= 'A' && keyname[ 0 ] <= 'Z')
                || (keyname[ 0 ] >= '0' && keyname[ 0 ] <= '9')) )
  {
    keycode = keyname[ 0 ];
  }
  else if( LUA_TNIL != lua_gettable( L, lua_upvalueindex( 1 ) ) )
  {
    keycode = moon_checkint( L, 2, 0, 255 );
  }
  else
  {
    lua_pushnil( L );
    return 1;
  }
  lua_pushboolean( L, tigrKeyDown( window, keycode ) );
  return 1;
}


static int ltigr_key_held( lua_State* L )
{
  Tigr* window = moon_checkobject( L, 1, "tigrWindow" );
  size_t len = 0;
  char const* keyname = luaL_checklstring( L, 2, &len );
  int keycode = 0;
  lua_settop( L, 2 );
  if( len == 1 && ((keyname[ 0 ] >= 'a' && keyname[ 0 ] <= 'z')
                || (keyname[ 0 ] >= 'A' && keyname[ 0 ] <= 'Z')
                || (keyname[ 0 ] >= '0' && keyname[ 0 ] <= '9')) )
  {
    keycode = keyname[ 0 ];
  }
  else if( LUA_TNIL != lua_gettable( L, lua_upvalueindex( 1 ) ) )
  {
    keycode = moon_checkint( L, 2, 0, 255 );
  }
  else
  {
    lua_pushnil( L );
    return 1;
  }
  lua_pushboolean( L, tigrKeyHeld( window, keycode ) );
  return 1;
}


static int ltigr_read_char( lua_State* L )
{
  Tigr* window = moon_checkobject( L, 1, "tigrWindow" );
  int keycode = tigrReadChar( window );
  if( keycode == 0 )
  {
    lua_pushnil( L );
  }
  else
  {
    lua_pushinteger( L, keycode );
  }
  return 1;
}


static int ltigr_load_image( lua_State* L )
{
  char const* filename = luaL_checkstring( L, 1 );
  void** p = moon_newpointer( L, "tigrBitmap", ltigr_free );
  *p = tigrLoadImage( filename );
  if( !*p )
  {
    luaL_fileresult( L, 0, filename );
    lua_pop( L, 1 ); /* pop error code */
    lua_error( L ); /* error message is at the top of the stack now */
  }
  return 1;
}


static int ltigr_load_image_mem( lua_State* L )
{
  size_t len = 0;
  char const* data = luaL_checklstring( L, 1, &len );
  void** p = moon_newpointer( L, "tigrBitmap", ltigr_free );
  assert( len <= INT_MAX );
  *p = tigrLoadImageMem( data, len );
  if( !*p )
  {
    luaL_fileresult( L, 0, NULL );
    lua_pop( L, 1 ); /* pop error code */
    lua_error( L ); /* error message is at the top of the stack now */
  }
  return 1;
}


static int ltigr_save_image( lua_State* L )
{
  Tigr* bitmap = moon_checkobject( L, 1, "tigrBitmap" );
  char const* filename = luaL_checkstring( L, 2 );
  return luaL_fileresult( L, tigrSaveImage( filename, bitmap ), filename );
}


static int ltigr_time( lua_State* L )
{
  lua_pushnumber( L, tigrTime() );
  return 1;
}


static int ltigr_error( lua_State* L ) {
  Tigr* window = NULL;
  char const* msg = lua_tostring( L, 1 );
  if( msg == NULL )
  {
    window = moon_checkobject( L, 1, "tigrWindow" );
    msg = luaL_checkstring( L, 2 );
  }
  tigrError( window, "%s", msg );
  return 0;
}


#define BITMAP_PROPERTIES \
  { ".w", ltigr_bitmap_w }, \
  { ".h", ltigr_bitmap_h }, \
  { ".cx", ltigr_bitmap_cx }, \
  { ".cy", ltigr_bitmap_cy }, \
  { ".cw", ltigr_bitmap_cw }, \
  { ".ch", ltigr_bitmap_ch }, \
  { ".blitmode", ltigr_bitmap_blitmode }

#define BITMAP_METHODS \
  { "get", ltigr_get }, \
  { "plot", ltigr_plot }, \
  { "clear", ltigr_clear }, \
  { "fill", ltigr_fill }, \
  { "line", ltigr_line }, \
  { "rect", ltigr_rect }, \
  { "fill_rect", ltigr_fill_rect }, \
  { "circle", ltigr_circle }, \
  { "fill_circle", ltigr_fill_circle }, \
  { "clip", ltigr_clip }, \
  { "blit", ltigr_blit }, \
  { "blit_alpha", ltigr_blit_alpha }, \
  { "blit_tint", ltigr_blit_tint }, \
  { "load_font", ltigr_load_font }, \
  { "print", ltigr_print }, \
  { "save_image", ltigr_save_image }

#define WINDOW_METHODS \
  { "closed", ltigr_closed }, \
  { "update", ltigr_update }, \
  { "mouse", ltigr_mouse }, \
  { "touch", ltigr_touch }, \
  { "read_char", ltigr_read_char }, \
  { "error", ltigr_error }

#define FONT_METHODS \
  { "text_width", ltigr_text_width }, \
  { "text_height", ltigr_text_height }


#ifndef EXPORT
#  define EXPORT extern
#endif /* EXPORT */

EXPORT int luaopen_tigr( lua_State* L )
{
  luaL_Reg const module_functions[] = {
    /* constructors */
    { "window", ltigr_window },
    { "bitmap", ltigr_bitmap },
    /* the font constructor is actually (also) a method of bitmap and included down below */
    { "load_image", ltigr_load_image },
    { "load_image_mem", ltigr_load_image_mem },
    /* aliases to the various methods */
    WINDOW_METHODS,
    BITMAP_METHODS,
    FONT_METHODS,
    /* misc functions */
    { "blitmode", ltigr_blitmode }, /* function variant of the bitmap property */
    { "rgba", ltigr_rgba },
    { "time", ltigr_time },
    { NULL, NULL }
  };
  luaL_Reg const keyboard_functions[] = {
    { "key_down", ltigr_key_down },
    { "key_held", ltigr_key_held },
    { NULL, NULL }
  };
  luaL_Reg const window_methods[] = {
    BITMAP_PROPERTIES,
    BITMAP_METHODS,
    WINDOW_METHODS,
    { NULL, NULL }
  };
  luaL_Reg const bitmap_methods[] = {
    BITMAP_PROPERTIES,
    BITMAP_METHODS,
    { NULL, NULL }
  };
  luaL_Reg const font_methods[] = {
    FONT_METHODS,
    { NULL, NULL }
  };
  moon_defobject( L, "tigrWindow", 0, window_methods, 0 );
  moon_defobject( L, "tigrBitmap", 0, bitmap_methods, 0 );
  moon_defobject( L, "tigrFont", 0, font_methods, 0 );
  moon_defcast( L, "tigrWindow", "tigrBitmap", ltigr_window_to_bitmap );
  luaL_newlib( L, module_functions );
  /* add the keyboard functions with the keycode table as upvalue */
  push_keycode_table( L );
  luaL_setfuncs( L, keyboard_functions, 1 );
  /* copy keyboard functions to window methods table */
  if( LUA_TTABLE == moon_getmethods( L, "tigrWindow" ) )
  {
    luaL_Reg const* funcdef = keyboard_functions;
    for( ; funcdef->name != NULL; ++funcdef )
    {
      if( LUA_TNIL != lua_getfield( L, -2, funcdef->name ) )
      {
        lua_setfield( L, -2, funcdef->name );
      }
    }
    lua_pop( L, 1 ); /* pop window methods table from stack */
  }
  {
    /* make the builtin (global) font available */
    void **f = moon_newpointer( L, "tigrFont", 0 );
    *f = tfont;
    lua_setfield( L, -2, "font" );
  }
  return 1;
}
