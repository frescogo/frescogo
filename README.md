<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>

# FrescoGO!

*FrescoGO!* é um marcador eletrônico semi-automático para jogos de Frescobol.

O aparelho marcador contém dois botões de marcação: um para o atleta à esquerda
e um para o atleta à direita.
A marcação é feita por um juiz, que pressiona o botão correspondente ao atleta
toda vez que ele acerta a bola.
Golpes de esquerda e direita são distinguidos pelo tempo de pressionamento do
botão.
Com base em uma distância previamente estabelecida e o tempo decorrido entre
dois golpes, o aparelho calcula a velocidade atingida pela bola a cada momento.
O botão do meio sinaliza início de sequência, queda de bola, fim de jogo, etc.
Há uma entrada USB para alimentação e um saída para som.

![Foto do marcador](device.jpg "Foto do Aparelho")

- Links do projeto:
    - Site: <https://github.com/frescogo/frescogo>
    - E-mail: <go.frescobol@gmail.com>

**O projeto eletrônico, o software e a regra são de domínio público, podendo
ser usados, copiados e modificados livremente.**

## Regra de Pontuação

Configuração sugerida:

- Tempo: 180s cronometrados (3 minutos)
- Distância: 750cm (7.5m)

Quesitos de pontuação:

- **Volume:**
    - A velocidade de cada golpe efetuado por um atleta é elevada ao quadrado,
      dividida por 100 e somada ao total do atleta.
      Exemplos:
        - 40 kmh vale **16 pontos**: `40x40/100 = 1600/100 = 16`.
        - 50 kmh vale **25 pontos**: `50x50/100 = 2500/100 = 25`.
        - 60 kmh vale **36 pontos**: `60x60/100 = 3600/100 = 36`.
        - 70 kmh vale **49 pontos**: `70x70/100 = 4900/100 = 49`.
        - 80 kmh vale **64 pontos**: `80x80/100 = 6400/100 = 64`.
        - 90 kmh vale **81 pontos**: `90x90/100 = 8100/100 = 81`.
- **Potência:**
    - Cada um dos 10 golpes mais velozes de direita e de esquerda de cada
      atleta são contabilizados conforme a regra acima e ainda são
      multiplicados por 4 antes de serem somados ao total do atleta.
      Exemplo:
        - Se um desses golpes mais velozes foi a 50 kmh, o atleta vai somar
          mais **100 pontos** por aquele golpe (`25x4=100`), além dos 25 pontos
          já somados no quesito de volume.
- **Equilíbrio:**
    - A menor pontuação dentre os dois atletas será considerada como a
      pontuação da dupla. Mais precisamente, será o menor valor entre a média
      dos dois e 110% do que pontuou menos:
        - `MIN( (A1+A2)/2, MIN(A1,A2)x1.1 )`
        - Ex., se os atletas pontuaram 4000 e 5000 pontos, pega-se a média
          (`(5000+4000)/2 = 4500`) e 110% do menor (`4000x110% = 4400`).
          A pontuação de equilíbrio será o menor entre os dois valores
          (`4500 vs 4400`): **4400 pontos**.
- **Continuidade:**
    - Cada queda subtrai 4% da pontuação final da dupla.
      Exemplos:
        - Com 25 quedas, a dupla perderá todos os pontos (`25x4% = 100%`).
        - Com 5 quedas e 4400 pontos após o equilíbrio, a pontuação final
          será de **3520 pontos** (`4400x80%`).

## Fluxo do Jogo

- Um som agudo indica que o atleta pode sacar.
- Após o saque, o juiz pressiona o botão correspondente a cada atleta toda vez
  que ele acerta a bola. O tempo só passa quando há pressionamento do botão.
- Um som identifica a faixa de velocidade do golpe anterior:
    - ` < 40 kmh`: som simples grave
    - ` < 50 kmh`: som simples médio
    - ` < 60 kmh`: som simples agudo
    - ` < 70 kmh`: som duplo   grave
    - ` < 80 kmh`: som duplo   médio
    - ` < 90 kmh`: som duplo   agudo
    - `>= 90 kmh`: som simples muito agudo
- Um golpe do lado não preferencial do atleta acompanha um som grave (após o
  som correspondente à velocidade).
- Quando o jogo está desequilibrado, os ataques do atleta que mais pontuou
  acompanham um som grave.
- Quando a bola cai, o juiz pressiona o botão de queda que emite um som
  característico.
- Os dois últimos golpes são ignorados e o tempo volta ao momento do último
  golpe considerado (i.e., um ataque tem que ser defendido e depois confirmado
  pelo próprio atacante).
- O juiz então pressiona o botão de reinício e o fluxo reinicia.
- Um som agudo é emitido quando faltam 90s, 60s, 30s, 10s, e 5s para o jogo
  terminar.
- Ao fim do jogo é gerado um relatório com todas as medições de golpes.

## Instruções para o Juiz

- Ligação dos Cabos
    - TODO

- Aplicativo Android
    - Configuração
        - TODO
    - Visualização
        - TODO

- Aparelho marcador:
    - Novo jogo:
        - Pressione o botão do meio e em seguida o da direita por 5 segundos.
        - Resposta: um som médio de dois segundos.
    - Início de sequência:
        - Pressione o botão do meio por um segundo, até escutar um som.
        - Resposta: um som agudo de meio segundo.
    - Golpes dos atletas:
        - Pressione o botão da esquerda ou direita quando, respectivamente, o
          atleta à esquerda ou à direita golpearem a bola. Se o golpe for um
          backhand, o pressionamento deve ser um pouco mais demorado.
        - Resposta: depende da velocidade (ver a seção "Fluxo do Jogo").
    - Queda de bola:
        - Pressione o botão do meio por um segundo, até escutar um som.
        - Resposta: três sons cada vez mais graves por meio segundo.
    - Fim do jogo:
        - Automático, quando o tempo do cronômetro expirar.
        - Resposta: um som grave por dois segundos.
    - Reconfiguração de fábrica:
        - Pressione o botão do meio e em seguida os da esquerda e direita por
          5 segundos.
        - Resposta: um som médio de dois segundos.
        - **Em princípio, esse procedimento nunca deverá ser necessário.**

## Perguntas e Respostas

- Esse aparelho é um radar? Como o aparelho mede a velocidade da bola?
    - O aparelho não é um radar e mede a velocidade de maneira aproximada:
        - Os atletas devem estar a uma distância fixa predeterminada.
        - O juiz deve pressionar o botão no momento exato dos golpes (ou o mais
          próximo possível).
        - O aparelho divide a distância pelo tempo entre dois golpes
          consecutivos para calcular a velocidade.
        - Exemplo: se os atletas estão a 8 metros de distância e em um momento
          a bola leva 1 segundo para se deslocar entre os dois, então a
          velocidade foi de 8m/s (29 kmh).

- Quais as desvantagens em relação ao radar?
    - A principal desvantagem é que a medição não é tão precisa pois os atletas
      se movimentam e o juiz inevitavelmente irá atrasar ou adiantar as
      medições.
    - OBS:
      O radar também não é perfeito, tendo erro estimado entre +1/-2 kmh.
      Além disso, qualquer angulação entre a trajetória da bola e a posição do
      radar afeta negativamente as medições (ex., um ângulo de 25 graus diminui
      as medições em 10%).
        - Fonte: <https://www.stalkerradar.com/stalker-speed-sensor/faq/stalker-speed-sensor-FAQ.shtml>

- Por quê as velocidades são elevadas ao quadrado?
    - Para bonificar os golpes mais potentes.
      Quanto maior a velocidade, maior ainda será o quadrado dela.
      Uma bola a 50 kmh vale `50x50=2500`, uma a 70 kmh vale `70x70=4900`,
      praticamente o dobro (25 vs 49 pontos, após a divisão por 100).

- Tem como o juiz "roubar"?
    - Ao atrasar a marcação de um golpe "A", consequentemente o golpe "B"
      seguinte será adiantado.
      O golpe "A" terá a velocidade reduzida e o golpe "B" terá a velocidade
      aumentada.
      Como a regra usa o quadrado das velocidades, esse atraso e adiantamento
      (se forem sistemáticos) podem afetar a pontuação final.

- Tem como o atleta "roubar" ou "tirar vantagem" da regra?
    - O atleta pode projetar o corpo para frente e adiantar ao máximo os golpes
      para aumentar a medição das velocidades.

- Tem alguma vantagem em relação ao radar?
    - **Custo**:
        Os componentes do aparelho somados custam menos de R$100.
        O radar custa em torno de US$1000 e não inclui o software para
        frescobol.
    - **Licença de uso**:
        Além do custo ser menor, não há nenhuma restrição legal sobre o uso
        do aparelho, software ou regra.
    - **Infraestrutura**:
        Além do aparelho, é necessário apenas um celular com um software
        gratuito (para obter as informações do jogo) e uma caixa de som
        potente (de preferência com bateria interna).
        Não é necessário computador, ponto de luz elétrica, área protegida ou
        outros ajustes finos para a medição do jogo.
    - **Transparência das medições**:
        Apesar de serem menos precisas, as medições são audíveis e qualquer
        erro grosseiro pode ser notado imediatamente.
        O radar só mede bolas acima de 40 kmh e não é possível identificar se
        as medições estão sempre corretas (o posicionamento dos atletas, vento
        e outros fatores externos podem afetar as medições).
    - **Verificabilidade das medições**:
        Os atletas podem verificar se a pontuação final foi justa.
        Os jogos podem ser medidos por um aparelho igual durante as
        apresentaçõs ou podem ser gravados para medição posterior pelo vídeo.
