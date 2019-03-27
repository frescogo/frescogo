local m = require 'lpeg'
local P, S, C, R = m.P, m.S, m.C, m.R

local SPC  = S'\t\n\r '
local SPCS = S'\t\n\r ' ^ 0

local patt = P'relatorio'^-1                                * SPCS *
             P'-'^0                                         * SPCS *
             C((1-SPC)^0)                                   * SPCS *    -- Joao
             P'/'                                           * SPCS *
             C((1-SPC)^0)                                   * SPCS *    -- Maria
             P'(' * C(R'09'^1) * P'cm'                      * SPCS *    -- 750cm
             P'-'                                           * SPCS *
             C(R'09'^1) * P's' * P')'                       * SPCS *    -- 180s
             P'-'^0                                         * SPCS *
             P'TOTAL:' * SPCS * C(R'09'^1)                  * SPCS *    -- 5000 pontos
             P'Tempo:' * SPCS * C(R'09'^1) * P'ms (-0s)'    * SPCS *    -- 180650 (-0s)
             P(0)
             

print(patt:match(assert(io.open(...)):read'*a'))
