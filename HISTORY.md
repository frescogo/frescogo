v5.0 (jul/22)
-------------

- Modo `CTRL-M` de depuração.
- Algumas operações por teclado requerem 3s de pressionamento.
- Regra de ataques dos dois lados (dir/esq).
  - Árbitro pressiona Z ou M para golpe não preferencial, que acompanha um som.
- Nova fórmula de bônus.
- Exibe médias de velocidade ao final do jogo (máx/min, 75+/75-, dir/esq).
- Exibição dos atletas não mais espelhada.
- Relatório foi simplificado.

v4.3 (jan/22)
-------------

- Nova faixa de som acima de 100kmh.
- Toca som ao adicionar/diminuir quedas.
- Aplica desequilibrio paulatinamente entre 0-100s.
- Exibe a soma bruta no placar.
- Limita pontuacao a 300 pontos (equivalente a 100kmh).

v4.2 (dez/21)
-------------

- Mostra o percentual de desequilíbrio.
- Permite voltar bola mesmo após término do jogo.
- Conta golpes manuais no modo radar sempre com a velocidade 30km/h.

v4.1 (nov/21)
-------------

- Fundo vermelho para atleta que está atrás (e não o contrário).
- Som diferente para velocidade >100kmh.
- Corrige backspace que não funcionava na edição dos nomes.
- Nova aferição para evitar duplicatas.
    - Também permite configurar tempo mínimo entre golpes.
- Launcher para Linux.

v4.0 (jun/21)
-------------

- Nova regra.
- Novos sons. Faixas de 10 em 10km/h.
- v4.0.2
    - Nova configuração `RADAR_REPS` para calibre do radar.

v3.1 (ago/20)
-------------

- Suporte ao radar.
- Operação manual pelo teclado.
- Jogo de trincas.
- Máximo de 100 ataques por atleta.
- v3.1.2
    - Modo "esquenta" de aquecimento.
    - Não toca sons abaixo de 50 (mas continua exibindo).
    - Corrige CTRL-BACKSPACE.
- v3.1.3
    - Relatório geral da competição em "resultados.csv".
    - Correções no tempo de aquecimento.
    - Tempo máximo de descanso com contagem regressiva.
    - Som acima de 90 km/h.
    - Tempo passa a cada segundo.
    - Tamanho das fontes maiores.

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
