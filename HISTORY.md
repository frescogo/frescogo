v2.0 (out/19)
-------------

- Regra totalmente remodelada.

v1.12 (set/19)
-------------

- A regra passa a usar o jogo de 5 minutos (300 segundos) como referência.
- O quesito de "Máximas" foi remodelado:
    - 36 golpes mais velozes: 24 normais e 12 revés (eram 11 para ambos).
    - Multiplicador de `x2` (era `x3`).
- As Máximas vêm habilitadas, mas sem o revés por padrão.
- O fator de queda passa a ser `2%` (era `1.8%`).
- O máximo de quedas passa a ser `20` (era `30`).
- Placar:
    - Exibe total de golpes.
    - Exibe e permite alterar o Juiz.

v1.11 (ago/19)
-------------

- Suporte ao modo "PC" para exibição de placar em tempo real.

v1.10 (jun/19)
-------------

- O quesito "Potência" foi renomeado para "Máximas".

v1.9 (jun/19)
-------------

- Configuração para sensibilidade da marcação dos backhands.
- A regra de potência desligada soma 0 pontos (antes assumia 50kmh para todos
  os golpes).

v1.8 (mai/19)
-------------

- A regra de potência agora considera a média dos 7 golpe mais rápidos, em vez
  de considerar cada um individualmente.
- Após a 18a queda, a apresentação é encerrada sumariamente.

v1.7 (abr/19)
-------------

- Percentual de quedas fixo em 3% para jogo de 3 minutos.
- Percentual de quedas e número de golpes de potência sempre proporcionais
  aos valores de referência para 3 minutos.
- Não há mais limite de quedas (eram 25).

v1.6
----

- O bônus de potência diminuiu de `x4` para `x3`.

v1.5
----

- CORREÇÃO: A pontuação final que vale é a que aparece em `placar` e não a de
            `relatorio`.

v1.4
----
