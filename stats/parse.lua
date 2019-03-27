local m = require 'lpeg'
local P, S, C, R, Ct = m.P, m.S, m.C, m.R, m.Ct

local SPC  = S'\t\n\r '
local X    = SPC ^ 0
local NUMS = R'09' ^ 1

local patt =
    P'relatorio'^-1                         * X *
    P'-'^0                                  * X *
    C((1-SPC)^0)                            * X *   -- Joao
    P'/'                                    * X *
    C((1-SPC)^0)                            * X *   -- Maria
    P'(' * C(NUMS) * P'cm'                  * X *   -- 750cm
    P'-'                                    * X *
    C(NUMS) * P's' * P')'                   * X *   -- 180s
    P'-'^0                                  * X *
    P'TOTAL:'  * X * C(NUMS)                * X *   -- 3701 pontos
    P'Tempo:'  * X * C(NUMS) * P'ms (-0s)'  * X *   -- 180650 (-0s)
    P'Quedas:' * X * C(NUMS)                * X *   -- 6 quedas
    P'Golpes:' * X * C(NUMS)                * X *   -- 286 golpes
    P'Ritmo:'  * X * C(NUMS)                * X *   -- 45 kmh
    (1-NUMS)^1 * C(NUMS)                    * X *   -- Joao: 5500
    P'[' * Ct((X * C(NUMS))^1) * X * P']'   * X *   -- [ ... ]
    P'[' * Ct((X * C(NUMS))^1) * X * P']'   * X *   -- [ ... ]
    (1-NUMS)^1 * C(NUMS)                    * X *   -- Maria: 4427
    P'[' * Ct((X * C(NUMS))^1) * X * P']'   * X *   -- [ ... ]
    P'[' * Ct((X * C(NUMS))^1) * X * P']'   * X *   -- [ ... ]
    P(0)
             

print(patt:match(assert(io.open(...)):read'*a'))
