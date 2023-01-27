#!/usr/bin/env lua5.4

tigr = require( "tigr" )

print( "global functions:" )
for k,v in pairs( tigr ) do
  print( "", k, v )
end

screen = tigr.window( 320, 240, "Hello" )
while not screen:closed() do
  screen:clear( tigr.rgba( 0x80, 0x90, 0xa0 ) )
  screen:print( tigr.font, 120, 100, tigr.rgba( 0xff, 0xff, 0xff ), "Hello, world." )
  screen:update()
  if screen:key_down( "escape" ) then
    break
  end
end

