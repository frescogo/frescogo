local m = require 'lpeg'
local P, S, C, R, Ct, Cc = m.P, m.S, m.C, m.R, m.Ct, m.Cc

local INP, out = ...
local ts = (((1-P'/')^0 * '/')^0 * 'serial_' * C((1-P'.')^1) * '.txt'):match(INP)
local OUT = out .. '/serial_' .. ts .. '.py'

-------------------------------------------------------------------------------

local SPC  = S'\t\n\r '
local X    = SPC ^ 0
local NUMS = R'09' ^ 1

local SEQ = Ct(
    P'-- Sequencia ' * X * NUMS * X * P'-'^1 * P'\r\n'      *
    (P'    ****' * Cc(true) + P'            ****' * Cc(false))  * X *
    Ct( (P'!'^-1 *X* NUMS *X* P'!'^-1 *X*
        P'(' *X* C(NUMS) *X* P'/' *X* NUMS * P')' * X)^0 )  * X *
    (P'!'^-1 *X* NUMS *X* P'!'^-1)^-1                       * X *
    P'-----   -----' * X * NUMS * X * NUMS                  * X *
    P(0))

local patt =
    P'relatorio'^-1                         * X *
    P'-'^1                                  * X *
    C((1-P' /')^0)                          * X *   -- Joao
    P'/'                                    * X *
    C((1-S'\r\n')^0)                        * X *   -- Maria
    P'-'^0                                  * X *
    P'TOTAL:'  * X * C(NUMS)                * X *   -- 3701 pontos
    P'Tempo:'  * X * C(NUMS) * 'ms (-' * NUMS * 's)'   * X *   -- 180650 (-0s)
    P'Quedas:' * X * C(NUMS)                * X *   -- 6 quedas
    P'Golpes:' * X * C(NUMS)                * X *   -- 286 golpes
    P'Ritmo:'  * X * C(NUMS) *'/'* C(NUMS)  * X *   -- 45/45 kmh
    P'Juiz:'   * X * C((1-S'\r\n')^0)       * X *   -- Arnaldo
    (1-NUMS)^1 * C(NUMS)                    * X *   -- Joao: 5500
    P'[' * Ct((X * C(NUMS))^1) *X* '] =>' *X* C(NUMS) * X *   -- [ ... ]
    P'[' * Ct((X * C(NUMS))^1) *X* '] =>' *X* C(NUMS) * X *   -- [ ... ]
    (1-NUMS)^1 * C(NUMS)                    * X *   -- Maria: 4427
    P'[' * Ct((X * C(NUMS))^1) *X* '] =>' *X* C(NUMS) * X *   -- [ ... ]
    P'[' * Ct((X * C(NUMS))^1) *X* '] =>' *X* C(NUMS) * X *   -- [ ... ]
    P'(CONF: v1.6 / ' * C(NUMS) * 'cm / ' *                -- 750cm
                 C(NUMS) * 's / pot=' *             -- 180s
                 C(NUMS) * ' / equ='  *             -- pot=0/1
                 C(NUMS) * ' / cont=' *             -- equ=0/1
                 C(NUMS) * ' / max='  *             -- cont=3%
                 C(NUMS) * ')'              * X *   -- max=85
    Ct(SEQ^1)                               * X *
    P'----------------------------'         * X *
    P'Atleta    Vol     Esq     Dir   Total' * X*
    (1-NUMS)^0 * C(NUMS) *X* '+' *X* C(NUMS) *X* '+' *X* C(NUMS) *X* '=' *X* C(NUMS) * X *
    (1-NUMS)^0 * C(NUMS) *X* '+' *X* C(NUMS) *X* '+' *X* C(NUMS) *X* '=' *X* C(NUMS) * X *
    P'Media:'      *X* C(NUMS) *X*
    P'Equilibrio:' *X* C(NUMS) *X* '(-)' *X*
    P'Quedas:'     *X* C(NUMS) *X* '(-)' *X*
    P'FINAL:'      *X* C(NUMS) *X*
--[[
]]
    P(0)
             
local esquerda, direita, total, _, quedas, golpes, ritmo1, ritmo2, _,
      p0, esqs0,esq0,dirs0,dir0, p1, esqs1,esq1,dirs1,dir1,
      distancia, tempo, potencia, equilibrio, continuidade, _,
      seqs,
      _vol0, _esq0, _dir0, _tot0,
      _vol1, _esq1, _dir1, _tot1,
      _media, _equilibrio, _quedas, _final = patt:match(assert(io.open(INP)):read'*a')

print(esquerda, direita, total, quedas, ritmo1)
--[[
print(esquerda, direita, total, ritmo2, dir1, distancia, continuidade, seqs)
for i,seq in ipairs(seqs) do
    print(i,seq)
end
error'ok'
]]

-------------------------------------------------------------------------------

assert(total==_final and p0==_tot0 and p1==_tot1)

local nomes  = { esquerda, direita }
local pontos = { {_tot0,_vol0,_esq0,_dir0}, {_tot1,_vol1,_esq1,_dir1} }
local ritmos = { {0,esq0,dir0}, {0,esq1,dir1} }
local lefts  = { esqs0, esqs1 }
local rights = { dirs0, dirs1 }
local hits = { {}, {} }
    for _,seq in ipairs(seqs) do
        local isesq, vels = table.unpack(seq)
        for i,vel in ipairs(vels) do
            local idx do
                if isesq then
                    if i%2 == 1 then
                        idx = 1
                    else
                        idx = 2
                    end
                else
                    if i%2 == 1 then
                        idx = 2
                    else
                        idx = 1
                    end
                end
            end
            ritmos[idx][1] = ritmos[idx][1] + vel*vel
            hits[idx][#hits[idx]+1] = vel
        end
    end
    ritmos[1][1] = math.floor(math.sqrt(ritmos[1][1]/#hits[1]))
    ritmos[2][1] = math.floor(math.sqrt(ritmos[2][1]/#hits[2]))
assert(tonumber(golpes) == (#hits[1]+#hits[2]))

assert(#seqs == quedas+1)

function player (i)
    local ret = "{\n"
    ret = ret .. "\t\t'nome'   : '"..nomes[i].."',\n"
    ret = ret .. "\t\t'golpes' : "..#hits[i]..",\n"
    ret = ret .. "\t\t'pontos' : ("..table.concat(pontos[i],',').."),\n"
    ret = ret .. "\t\t'ritmo'  : ("..table.concat(ritmos[i],',').."),\n"
    ret = ret .. "\t\t'left'   : ("..table.concat(lefts[i],',').."),\n"
    ret = ret .. "\t\t'right'  : ("..table.concat(rights[i],',').."),\n"
    ret = ret .. "\t\t'hits'   : ("..table.concat(hits[i],',').."),\n"
    ret = ret .. "\t}\n"
    return ret
end

local out = assert(io.open(OUT,'w'))
out:write("GAME = {\n")
out:write("\t'timestamp' : '"..ts.."',\n")
out:write("\t'conf'      : {\n")
out:write("\t\t'distancia'    : "..distancia..",\n")
out:write("\t\t'tempo'        : "..tempo..",\n")
out:write("\t\t'potencia'     : "..potencia..",\n")
out:write("\t\t'equilibrio'   : "..equilibrio..",\n")
out:write("\t\t'continuidade' : "..continuidade..",\n")
out:write("\t},\n")
out:write("\t'pontos'    : (".._final..",".._media..",".._equilibrio..",".._quedas.."),\n")
out:write("\t'ritmo'     : ("..ritmo1..","..ritmo2.."),\n")
out:write("\t'golpes'    : "..golpes..",\n")
out:write("\t'quedas'    : "..quedas..",\n")
out:write("\t0           : "..player(1)..",\n")
out:write("\t1           : "..player(2)..",\n")
out:write("}\n")
out:close()
