local json = require 'dkjson'

local db = io.open( 'gamedb.json' ):read( '*a' )
local obj, pos, err = json.decode( db, 1, json.null )

if err then
  error( err )
end

io.write( '#include <stdint.h>\n' )
io.write( '#include <libretro.h>\n\n' )

io.write( 'typedef struct\n' )
io.write( '{\n' )
io.write( '  uint32_t sha1[ 5 ];\n' )
io.write( '  struct retro_variable vars[ 16 ];\n' )
io.write( '}\n' )
io.write( 'overrides_t;\n\n' )

io.write( 'static overrides_t s_overrides[] =\n' )
io.write( '{\n' )

table.sort( obj.games, function (e1, e2) return e1.sha1 < e2.sha1 end )

for i = 1, #obj.games do
  local game = obj.games[ i ]
  
  io.write( '  {\n' )
  io.write( '    /* ', game.name, ', ', game.year, ', ', game.author, ', ', game.url, ' */\n' )
  
  io.write( '    { ' )
  io.write( '0x', game.sha1:sub( 1, 8 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 9, 16 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 17, 24 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 25, 32 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 33, 40 ), 'U ' )
  io.write( '},\n' )
  
  local settings = {}
  local unused =
  {
    [ '81_8_16_contents' ] = 'ROM shadow',
    [ '81_chroma_81' ] = 'disabled',
    [ '81_highres' ] = 'none',
    [ '81_sound' ] = 'none',
    
    [ '81_joypad_a' ] = 'default',
    [ '81_joypad_b' ] = 'default',
    [ '81_joypad_down' ] = 'default',
    [ '81_joypad_l' ] = 'default',
    [ '81_joypad_l2' ] = 'default',
    [ '81_joypad_left' ] = 'default',
    [ '81_joypad_r' ] = 'default',
    [ '81_joypad_r2' ] = 'default',
    [ '81_joypad_right' ] = 'default',
    [ '81_joypad_up' ] = 'default',
    [ '81_joypad_x' ] = 'default',
    [ '81_joypad_y' ] = 'default',
  }
  
  for key, value in pairs( game.settings ) do
    if value ~= json.null then
      settings[ #settings + 1 ] = { key = key, value = value }
      unused[ key ] = nil
    end
  end
  
  for key, value in pairs( unused ) do
    settings[ #settings + 1 ] = { key = key, value = value }
  end
  
  table.sort( settings, function( e1, e2 ) return e1.key < e2.key end )
  io.write( '    {\n' )
  
  for _, setting in ipairs( settings ) do
    io.write( '      { "', setting.key, '", "', setting.value, '" },\n')
  end
  
  io.write( '    },\n' )
  io.write( '  },\n' )
end

io.write( '};\n' )
