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
        --Marcelo = { n=0,  r=3000 },
        --Lucia   = { n=33, r=4000 },
    },
}

function VAR (p)
    -- soma dos quadrados da quantidade de jogos com cada parceiro
    -- divididos pelo total de parceiros ao cubo
    -- depois tira a raiz do resultado
    --      p1^2 + p2^2 + ... + pn^2   |  4^2 + 4^2 + 4^2 + 4^2   4^3
    --      ------------------------   |  --------------------- = --- = 1 => 1^(1/2) = 1
    --                n^3              |          4^3             4^3
    --
    --                                 |         16^2             256
    --                                          ------          = --- = 256 => 256^(1/2) = 16
    --                                            1^3              1
    --
    --                                 |  1^2 + 1^2 + ... + 1^2    16     1      1           1
    --                                 |  --------------------- = ---- = --- => ---^(1/2) = --
    --                                 |         16^3             16^3   256    256         16
    local tot = 0
    local sum = 0
    for _,n in pairs(p.ms) do
        sum = sum + n*n
        tot = tot + 1
    end
    local ret = math.sqrt(sum/(tot*tot*tot))
    return math.floor(ret * 100)
end

function MATCH (t)
    local k1, k2 = table.unpack(t.players)
    if not (X[k1] and X[k2]) then
        --return
    end

    if not ALL.players[k1] then ALL.players[k1]={k=k1,n=0,r=3000,ms={}} end
    if not ALL.players[k2] then ALL.players[k2]={k=k2,n=0,r=3000,ms={}} end

    local p1 = assert(ALL.players[k1], k1)
    local p2 = assert(ALL.players[k2], k2)

    p1.ms[k2] = (p1.ms[k2] or 0) + 1
    p2.ms[k1] = (p2.ms[k1] or 0) + 1

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

    local xp  = (p1.r + p2.r) / 2
    local dif = t.score - xp

    p1.n = p1.n + 1
    p2.n = p2.n + 1

--[[
    local dx = math.max(p1.n,p2.n) / math.min(p1.n,p2.n)
    local n1, n2 do
        if p1.n > p2.n then
            n1 = p1.n * dx
            n2 = p2.n / dx
        else
            n1 = p1.n / dx
            n2 = p2.n * dx
        end
    end
    n1 = math.max(1, n1)
    n2 = math.max(1, n2)
    --print(dif, p1.n,n1, p2.n,n2) --math.sqrt(p1.n), math.sqrt(p2.n))
]]
    local n1, n2 = p1.n, p2.n

    p1.r = p1.r + dif/math.sqrt(n1)
    p2.r = p2.r + dif/math.sqrt(n2)

--[[
    print('---', t.score, xp)
    print(k1, table.unpack(p1))
    print(k2, table.unpack(p2))
]]

end

--dofile (...)
--dofile '/tmp/matches.lua'
--[[
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
]]

--[=[
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
dofile '../Jogos/ms/m13.lua'
dofile '../Jogos/ms/m14.lua'
dofile '../Jogos/ms/m15.lua'
dofile '../Jogos/ms/m16.lua'
]=]

dofile '../Jogos/Rio_das_Ostras/20190616/matches.lua'

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

    local T = {}
    for k, p in pairs(ALL.players) do
        --if p.n >= 10 then
            T[#T+1] = { k, p.n, VAR(p), math.floor(p.r) }
        --end
    end
    table.sort(T, function (v1,v2) return v1[4]>v2[4] end)

    --print(ALL.n)
    print(string.format('%-15s','Atleta') ..'  '..
          string.format('%5s','Jogos')    ..'  '..
          string.format('%5s','Par')      ..'  '..
          string.format('%6s','Pontos'))
    print('-------------------------------------')
    for _, t in pairs(T) do
        print(string.format('%-15s',t[1]) ..'  '..
              string.format('%5s',  t[2]) ..'  '..
              string.format('%5d',  t[3]) ..'  '..
              string.format('%6s',  t[4]))
    end

end
