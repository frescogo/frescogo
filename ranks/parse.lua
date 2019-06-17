--lua5.3 parse.lua <file> /tmp

local m = require 'lpeg'
local P, S, C, R, Ct, Cc = m.P, m.S, m.C, m.R, m.Ct, m.Cc

local INP = ...
local timestamp = (((1-P'/')^0 * '/')^0 * 'serial_' * C((1-P'.')^1) * '.txt'):match(INP)

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
    C((1-X*P'/')^0)                         * X *   -- Joao
    P'/'                                    * X *
    C((1-P' '^0*S'\r\n')^0)                 * X *   -- Maria
    P'(750cm - 180s)'^-1                    * X *
    P'-'^0                                  * X *
    P'TOTAL'  *X* (P':'+P'.'^1) *X* C(NUMS) * P' pts'^-1 * X *   -- 3701 pontos
    P'Tempo'  *X* (P':'+P'.'^1) *X* P(NUMS) * 'ms (-' * NUMS * 's)'   * X *   -- 180650 (-0s)
    P'Quedas' *X* (P':'+P'.'^1) *X* C(NUMS) * X *   -- 6 quedas
    P'Golpes' *X* (P':'+P'.'^1) *X* P(NUMS) * X *   -- 286 golpes
    P'Ritmo'  *X* (P':'+P'.'^1) *X* P(NUMS) * ('/'* P(NUMS))^-1 * X *   -- 45/45 kmh
    (P'Juiz'  *X* (P':'+P'.'^1) *X* C((1-S'\r\n')^0) + Cc'Chico')       * X *   -- Arnaldo
    (1-NUMS)^1 * P(NUMS) * P' pts'^-1                    * X *   -- Joao: 5500
    P'esq'^-1 *X* P'[' * Ct((X * C(NUMS))^1) *X* ']' *X* (P'=>' *X* P(NUMS) * P' kmh'^-1)^-1 * X *
    P'dir'^-1 *X* P'[' * Ct((X * C(NUMS))^1) *X* ']' *X* (P'=>' *X* P(NUMS) * P' kmh'^-1)^-1 * X *
    (1-NUMS)^1 * P(NUMS) * P' pts'^-1                    * X *   -- Maria: 4427
    P'esq'^-1 *X* P'[' * Ct((X * C(NUMS))^1) *X* ']' *X* (P'=>' *X* P(NUMS) * P' kmh'^-1)^-1 * X *
    P'dir'^-1 *X* P'[' * Ct((X * C(NUMS))^1) *X* ']' *X* (P'=>' *X* P(NUMS) * P' kmh'^-1)^-1 * X *
    (P'---'*P'-'^0)^-1 * X *
    (
        P'(CONF: v' * C(NUMS) * '.' * C(NUMS) * ('.' * C(NUMS) + Cc'?') * ' / ' * -- v1.5
                      C(NUMS) * 'cm / '    *X*
                      C(NUMS) * 's / pot=' *X*
                      C(NUMS) * ' / equ='  *X*
                      C(NUMS) * ' /' *X* 'cont=' *X*
                      (P(NUMS) * ' / fim='  *X)^-1*
                      P(NUMS) * ' / max='  *X*
                      (P(NUMS) * ' / sens='  *X)^-1*
                      P(NUMS) * ')'
    +
        P'(CONF: ' * Cc'0' * Cc'?' * Cc'?' *
                      C(NUMS) * 'cm / '    *            -- 750cm
                      C(NUMS) * 's / pot=' *            -- 180s
                      C(NUMS) * ' / equ='  *            -- pot=0/1
                      C(NUMS) * ' / cont=' *            -- equ=0/1
                      P(NUMS) * ')'
    +
        Cc'0'*Cc'?'*Cc'?'*Cc'750'*Cc'180'*Cc'1'*Cc'1'
    ) * X *  -- max=85
    (P'---'*P'-'^0)^-1 * X *
    Ct(SEQ^1)                               * X *
    P'----------------------------'         * X *
    P'Atleta    Vol     Esq     Dir   Total'^-1 * X*
    (1-NUMS)^0 * C(NUMS) *X* '+' *X* P(NUMS) *X* '+' *X* P(NUMS) *X* '=' *X* P(NUMS) * X *
    (1-NUMS)^0 * C(NUMS) *X* '+' *X* P(NUMS) *X* '+' *X* P(NUMS) *X* '=' *X* P(NUMS) * X *
--[[
]]
    P(0)
             
local k1,k2, total,quedas,juiz, e1,d1, e2,d2, v1,v2,v3,_d,_t,_p,_e, _, p1,p2
        = patt:match(assert(io.open(INP)):read'*a')

--print(k1,k2, quedas,juiz, e1,d1, e2,d2, v1,v2,v3,_d,_t,_p,_e, p1,p2)

assert(_d == '750')
assert(_t == '180')
assert(_p == '1', timestamp)
assert(_e == '1')

local version = v1..'.'..v2..'.'..v3
local arena = 'Bolivar'

function pot (t)
    if _p == '0' then
        return 0
    end
    local sum = 0
    for i=1, 7 do
        sum = sum + t[i]
    end
    local avg = math.floor(sum/7)
    local ret = avg*avg * 21 / 100
    --print(avg,ret)
    return ret
end

function min (x,y)
    return x<y and x or y
end

local s1 = p1 + pot(e1) + pot(d1)
local s2 = p2 + pot(e2) + pot(d2)
local score = min( (s1+s2)/2, min(s1,s2)*1.1 )
--print(p2, s2)

score = score * (1 - quedas*0.03)
score = math.floor(score)

if version >= '1.8.1' then
    --print(score,total)
    io.stderr:write(timestamp..' '..version..' '..score..' '..total..'\n')
    assert(score==tonumber(total) or score==tonumber(total)-1)
end

print([[
MATCH {
    timestamp = ']]..timestamp..[[',
    players   = { ']]..k1..[[', ']]..k2..[[' },
    arena     = ']]..arena..[[',
    referee   = ']]..juiz..[[',
    score     = ]]..total..[[,
    version   = ']]..version..[[',
}
]])
