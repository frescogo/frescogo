ALL = {
    timestamp = '00000000_000000',
    players   = {
        --Chico   = { 1, 0 },
        --Antonio = { 1, 0 },
    },
}

function MATCH (t)
    local k1, k2 = table.unpack(t.players)

    if not ALL.players[k1] then ALL.players[k1]={1,0} end
    if not ALL.players[k2] then ALL.players[k2]={1,0} end

    local p1 = assert(ALL.players[k1], k1)
    local p2 = assert(ALL.players[k2], k2)

    assert(t.timestamp > ALL.timestamp)

    local xp  = (p1[2] + p2[2]) / 2
    local dif = t.score - xp

    p1[2] = p1[2] + dif/p1[1]
    p2[2] = p2[2] + dif/p2[1]

    p1[1] = p1[1] + 1
    p2[1] = p2[1] + 1
end

--dofile (...)
dofile '../Jogos/Bolivar/20190319/matches.lua'
dofile '../Jogos/Bolivar/20190323/matches.lua'
dofile '../Jogos/Bolivar/20190325/matches.lua'
dofile '../Jogos/Bolivar/20190327/matches.lua'
dofile '../Jogos/Bolivar/20190328/matches.lua'
dofile '../Jogos/Bolivar/20190330/matches.lua'
dofile '../Jogos/Bolivar/20190403/matches.lua'
dofile '../Jogos/Bolivar/20190403/matches.lua'
dofile '../Jogos/Bolivar/20190405/matches.lua'
dofile '../Jogos/Bolivar/20190407/matches.lua'
dofile '../Jogos/Bolivar/20190414/matches.lua'
dofile '../Jogos/Bolivar/20190501/matches.lua'
dofile '../Jogos/Bolivar/20190601/matches.lua'

for k, p in pairs(ALL.players) do
    print(k, string.len(k), table.unpack(p))
end
