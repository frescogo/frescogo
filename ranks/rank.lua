-- lua5.3 rank.lua | sort -n -r -k3
-- lua5.3 rank.lua | sort -n -r -k2 | head -n 9 | sort -n -r -k3

X = {
    Lira        = true,
    Antonio     = true,
    Douglas     = true,
    Rogerio     = true,
    Silvio      = true,
    Tome        = true,
    Paulinha    = true,
    Mauricio    = true,
    Luiz_N      = true,
    Ira         = true,
    Eliane      = true,
    Lucia       = true,
    Franklin    = true,
    Sergio      = true,
}

ALL = {
    n         = 0,
    timestamp = '00000000_000000',
    players   = {
        --Chico   = { 1, 0 },
        --Antonio = { 1, 0 },
    },
}

function MATCH (t)
    local k1, k2 = table.unpack(t.players)
    if not (X[k1] and X[k2]) then
        --return
    end

    if not ALL.players[k1] then ALL.players[k1]={1,3000} end
    if not ALL.players[k2] then ALL.players[k2]={1,3000} end

    local p1 = assert(ALL.players[k1], k1)
    local p2 = assert(ALL.players[k2], k2)

--[[
    if k2 == 'Chico' then
        print(k1, table.unpack(p1))
        print(k2, table.unpack(p2))
    end
    print'==============='
    print(k1, table.unpack(p1))
    print(k2, table.unpack(p2))
]]

    --assert(t.timestamp > ALL.timestamp)
    ALL.timestamp = t.timestamp
    ALL.n         = ALL.n + 1

    local xp  = (p1[2] + p2[2]) / 2
    local dif = t.score - xp
    print(dif)

    p1[2] = p1[2] + dif/math.sqrt(p1[1])
    p2[2] = p2[2] + dif/math.sqrt(p2[1])

    p1[1] = p1[1] + 1
    p2[1] = p2[1] + 1

--[[
    print('---', t.score, xp)
    print(k1, table.unpack(p1))
    print(k2, table.unpack(p2))
]]

end

--dofile (...)
--dofile '/tmp/matches.lua'
for i=1,1 do
dofile '../Jogos/Bolivar/20190319/matches.lua'
dofile '../Jogos/Bolivar/20190323/matches.lua'
dofile '../Jogos/Bolivar/20190325/matches.lua'
dofile '../Jogos/Bolivar/20190327/matches.lua'
dofile '../Jogos/Bolivar/20190328/matches.lua'
dofile '../Jogos/Bolivar/20190330/matches.lua'
dofile '../Jogos/Bolivar/20190403/matches.lua'
dofile '../Jogos/Bolivar/20190405/matches.lua'
dofile '../Jogos/Bolivar/20190407/matches.lua'
dofile '../Jogos/Bolivar/20190414/matches.lua'
dofile '../Jogos/Bolivar/20190501/matches.lua'
dofile '../Jogos/Bolivar/20190601/matches.lua'
end
--[[
dofile '../Jogos/ms/m1.lua'
dofile '../Jogos/ms/m2.lua'
dofile '../Jogos/ms/m3.lua'
dofile '../Jogos/ms/m4.lua'
dofile '../Jogos/ms/m5.lua'
dofile '../Jogos/ms/m6.lua'
dofile '../Jogos/ms/m7.lua'
dofile '../Jogos/ms/m8.lua'
dofile '../Jogos/ms/m9.lua'
dofile '../Jogos/ms/m10.lua'
dofile '../Jogos/ms/m11.lua'
dofile '../Jogos/ms/m12.lua'

for i=1, 100000 do
    MATCH {
        timestamp = '20190601_101146',
        players   = { 'Marcelo', 'Lucia' },
        arena     = 'Bolivar',
        referee   = 'Chico',
        score     = 3000,
        version   = '1.8.1',
    }
    print(ALL.players.Marcelo[2], ALL.players.Lucia[2])
    MATCH {
        timestamp = '20190601_101146',
        players   = { 'Marcelo', 'Lucia' },
        arena     = 'Bolivar',
        referee   = 'Chico',
        score     = 2000,
        version   = '1.8.1',
    }
    print(ALL.players.Marcelo[2], ALL.players.Lucia[2])
end
]]

if false then

    io.write([[
ALL = {
    timestamp = ']]..ALL.timestamp..[[',
    players   = {
]])

    for k, p in pairs(ALL.players) do
        print('        '..k..' = { '..p[1]..', '..p[2]..' },')
    end

print([[
    },
}
]])

else

    print(ALL.n)
    print(string.format('%-10s','Atleta') ..'  '..
          string.format('%5s','Jogos')    ..'  '..
          string.format('%6s','Rating'))
    print('-------------------------')
    for k, p in pairs(ALL.players) do
        print(string.format('%-10s',k)    ..'  '.. string.format('%5s',p[1]-1) ..'  '.. string.format('%6s',math.floor(p[2])))
    end

end
