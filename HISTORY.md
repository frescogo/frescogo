v3.1 (ago/20)
-------------

- Suporte ao radar.
- Operação manual pelo teclado.
- Jogo de trincas.
- Máximo de 100 ataques por atleta.
- v3.1.2
    - Modo "esquenta".
    - Não toca sons abaixo de 50 (mas continua exibindo).
    - Corrige CTRL-BACKSPACE.

v3.0 (jul/20)
-------------

- Regra nova com limite de golpes e velocidade mínima de ataque.
- Base: 4 minutos, 50 km/h, 120 ataques (108+12).

v2.4 (fev/20)
-------------

- Padrão de tempo é de 4:00 (era 5:00).
    - 1.5% por queda (1.2% para 5:00).
    - 20 normais (25 para 5:00).
    - 12 revés (15 para 5:00).
- Marca tempo de descanso.
- Aparelho detecta se placar está conectado.
- Acumulativo dá valores comparáveis em qualquer tempo.

v2.3 (jan/20)
-------------

- Envia configuração do aparelho para o placar.
- Move os 15% da pontuação de Revés para o Volume, caso esteja desabilitado.


v2.2 (jan/20)
-------------

- Além da pontuação média, calcula uma pontuação acumulativa.
- Não limita desconto de quedas durante o jogo, somente no fim.

v2.1 (jan/20)
-------------

- Desconto de 1% por queda (era 2%).

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
