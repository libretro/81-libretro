local json = require 'dkjson'

local function map( game, button )
  if game.settings.mapping[ button ] then
    return game.settings.mapping[ button ].key
  else
    return 'default'
  end
end

local function label( game, button )
  if game.settings.mapping[ button ] then
    return game.settings.mapping[ button ].label
  else
    return ''
  end
end

local db = io.open( 'gamedb.json' ):read( '*a' )
local obj, pos, err = json.decode( db, 1, nil )

if err then
  error( err )
end

io.write( '#include <stdint.h>\n' )
io.write( '#include <libretro.h>\n\n' )

io.write( 'typedef struct\n' )
io.write( '{\n' )
io.write( '  uint32_t sha1[ 5 ];\n' )
io.write( '  struct retro_variable vars[ 15 ];\n' )
io.write( '}\n' )
io.write( 'overrides_t;\n\n' )

io.write( 'static overrides_t s_overrides[] =\n' )
io.write( '{\n' )

table.sort( obj.games, function (e1, e2) return e1.sha1 < e2.sha1 end )

for i = 1, #obj.games do
  local game = obj.games[ i ]
  
  io.write( '  {\n' )
  io.write( '    /* ', game.name, ', ', game.year, ', ', game.author, ', ', game.url, '*/\n' )
  
  io.write( '    { ' )
  io.write( '0x', game.sha1:sub( 1, 8 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 9, 16 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 17, 24 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 25, 32 ), 'U, ' )
  io.write( '0x', game.sha1:sub( 33, 40 ), 'U ' )
  io.write( '},\n' )
  
  local settings = {}
  
  for key, value in pairs( game.settings ) do
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
