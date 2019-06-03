<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>

<!--
$ pandoc README.md -H deeplists.tex -o frescogo.pdf
-->

# FrescoGO! (versão 1.8)

*FrescoGO!* é um marcador eletrônico semi-automático para treinamento e
competições de Frescobol.

O aparelho marcador contém dois botões de marcação: um para o atleta à esquerda
e um para o atleta à direita.
A marcação é feita por um juiz, que pressiona o botão correspondente ao atleta
toda vez que ele acerta a bola.
<!--
Golpes de esquerda e direita são distinguidos pelo tempo de pressionamento do
botão.
-->
Com base em uma distância previamente estabelecida e o tempo decorrido entre
dois golpes consecutivos, o aparelho calcula a velocidade média atingida pela
bola a cada troca.
O botão do meio sinaliza início de sequência, queda de bola, fim de
apresentação, etc.
Também há uma entrada USB para visualização do placar e um saída para som para
sinalizar os golpes.

![Foto do marcador](images/device.jpg "Foto do Aparelho")

- Links do projeto:
    - Site: <https://github.com/frescogo/frescogo>
    - E-mail: <go.frescobol@gmail.com>
    - Vídeos: <https://www.youtube.com/channel/UCrc_Ds56Bh77CFKXldIU-9g>
    - Licença: <https://creativecommons.org/publicdomain/mark/1.0/deed.pt_BR>

**O projeto eletrônico, o software e a regra são de domínio público, podendo
ser usados, copiados e modificados livremente.**

-------------------------------------------------------------------------------

## Regra de Pontuação

Configuração sugerida:

- Tempo: 180s cronometrados (3 minutos)
- Distância: 750cm (7.5 metros)

Quesitos de pontuação:

- **Volume:**
    - A velocidade de cada golpe efetuado por um atleta é elevada ao quadrado,
      dividida por 100 e somada ao total do atleta. Não há velocidade mínima.
      Exemplos:
        - 30 kmh vale **09 pontos**: `30x30/100 =  900/100 =  9`.
        - 40 kmh vale **16 pontos**: `40x40/100 = 1600/100 = 16`.
        - 50 kmh vale **25 pontos**: `50x50/100 = 2500/100 = 25`.
        - 60 kmh vale **36 pontos**: `60x60/100 = 3600/100 = 36`.
        - 70 kmh vale **49 pontos**: `70x70/100 = 4900/100 = 49`.
        - 80 kmh vale **64 pontos**: `80x80/100 = 6400/100 = 64`.
        - 90 kmh vale **81 pontos**: `90x90/100 = 8100/100 = 81`.
- **Potência:**
    - As médias dos 7 golpes mais velozes de **direita** e de **esquerda**
      (*forehand* e *backhand*) de cada atleta são contabilizados conforme a
      fórmula de *Volume* e ainda são multiplicados por 21.
      Exemplo:
        - Os golpes mais velozes de um dos atleta foram os seguintes:
            - De direita:  **`87`, `85`, `83`, `82`, `75`, `75`, `73`**, `70`, `69`, ...
            - De esquerda: **`52`, `50`, `50`, `49`, `44`, `35`, ` 0`,** ` 0`, ` 0`, ...
        - A média dos 7 golpes mais velozes de direita foi de 80 kmh
          (`(87+85+83+82+75+75+73)/7 = 80`),
          somando `64x21` = **1344 pontos**.
        - A média dos 7 golpes mais velozes de esquerda foi de 40 kmh
          (`(52+50+50+49+44+35+0)/7 = 40`),
          somando `16x21` = **336 pontos**.
        - Esse atleta vai então obter **1680 pontos** de *Potência* que ainda
          serão somados com os seus pontos de *Volume*.
<!--
    - OBS: Em uma apresentação de 3 minutos, 7 golpes correspondem a
      aproximadamente 10% dos golpes em posição de ataque.
-->

- **Equilíbrio:**
    - A pontuação da dupla será a média de pontos entre os dois atletas.
    - Se o atleta que pontuou menos estiver abaixo de 90% dessa média, então a
      pontuação da dupla será o menor valor.
    - Mais precisamente, a pontuação será o menor valor entre a média dos dois
      e 110% do que pontuou menos:
        - `MIN( (A1+A2)/2, MIN(A1,A2)x1.1 )`
        - Ex., se os atletas pontuaram 4000 e 5000 pontos, pega-se a média
          (`(5000+4000)/2 = 4500`) e 110% do menor (`4000x110% = 4400`).
          A pontuação de equilíbrio será o menor entre os dois valores
          (`4500 vs 4400`): **4400 pontos**.
- **Continuidade:**
    - Cada queda subtrai 3% da pontuação final da dupla.
      Exemplo:
        - Com 5 quedas, a dupla perderá 15% dos pontos, ou seja, se ela pontuou
          4400 após o equilíbrio, a pontuação final será de **3740 pontos**
          (`4400x85%`).
    - Os dois últimos golpes antes da queda também são desconsiderados.
    - A apresentação é encerrada sumariamente na 18a queda.

-------------------------------------------------------------------------------

## Fluxo da Apresentação

- Um som agudo longo indica que a uma nova apresentação irá começar.
- Um som agudo indica que o atleta pode sacar.
- Após o saque, o juiz pressiona o botão correspondente a cada atleta toda vez
  que ele acerta a bola. O tempo só passa quando o botão é pressionado.
- Um som identifica a faixa de velocidade do golpe anterior:
    - ` < 40 kmh`: som simples grave
    - ` < 50 kmh`: som simples médio
    - ` < 60 kmh`: som simples agudo
    - ` < 70 kmh`: som duplo   grave
    - ` < 80 kmh`: som duplo   médio
    - ` < 90 kmh`: som duplo   agudo
    - `>= 90 kmh`: som duplo   muito agudo
- Um golpe do lado não preferencial do atleta acompanha um som grave (após o
  som correspondente à velocidade).
- Quando a apresentação está desequilibrada, os ataques do atleta que mais
  pontuou acompanham um som grave.
- Quando a bola cai, o juiz pressiona o botão de queda que emite um som
  característico.
- Os dois últimos golpes são ignorados e o tempo volta ao momento do último
  golpe considerado (i.e., um ataque tem que ser defendido e depois confirmado
  pelo próprio atacante).
- O juiz então pressiona o botão que habilita o saque e o fluxo reinicia.
- Um som agudo é emitido a cada 1 minuto e também quando faltam 30, 10, e 5
  segundos para a apresentação terminar.
- A apresentação termina após 3 minutos cronometrados ou após a 18a queda.
  Um som grave longo indica que a apresentação terminou.
- Ao fim da apresentação, é gerado um relatório com todas as medições de
  golpes.

### Formatação do Resultado

A seguir são explicados os formatos de exibição do resultado da apresentação.

- Placar (a cada sequência)

```
-----------------------------------------------
                  Joao / Maria                  <-- nome dos atletas
-----------------------------------------------

TOTAL ........ 604 pts                          <-- total de pontos
Tempo ........ 19630ms (-160s)                  <-- tempo passado e restante
Quedas ....... 5                                <-- número de quedas
Golpes ....... 28                               <-- quantidade de golpes
Ritmo ........ 46/48 kmh                        <-- ritmo em km/h (média simples/quadrática)
Juiz ......... Arnaldo                          <-- nome do juiz

-----------------------------------------------

    Joao: 1024 pts                              <-- total do atleta à esquerda
 esq  [  39  36  36  36  34  33  31 ] => 35 kmh <-- 7 golpes de esquerda mais fortes => média simples
 dir  [  65  56  55  54  52  51  50 ] => 54 kmh <-- 7 golpes de direita  mais fortes => média simples

-----------------------------------------------

   Maria: 646 pts                               <-- total do atleta à direita
 esq  [  67   0   0   0   0   0   0 ] => 9 kmh  <-- 7 golpes de esquerda mais fortes => média simples
 dir  [  71  67  65  64  64  57  36 ] => 60 km  <-- 7 golpes de direita  mais fortes => média simples

-----------------------------------------------

(CONF: v1.8.0 / 750cm / 180s / pot=1 / equ=1 /  <-- configurações
       cont=30 / fim=18 / max=85)
        \-- versão do software
                 \-- distância entre os ateltas
                         \-- tempo máximo de apresentação
                                \-- pontuação de potência (0=desligada, 1=ligada)
                                        \-- pontuação de equilíbrio
        \-- desconto por queda (30 = 3.0%)
                  \-- número máximo de quedas
                           \-- velocidade máxima a detectar

-----------------------------------------------
```

- Relatório (ao final da apresentação)

```
-- Sequencia  1 ----------------        <-- Início da primeira sequência.
            ****                        <-- Maria efetuou o primeiro golpe.
     800            ( 33 / 1089)        <-- João golpeou 800ms depois.
             440    ( 61 / 3721)            A velocidade atingida foi de 33 kmh,
     820            ( 32 / 1024)            somando 1089 pontos para Maria
             350    ( 77 / 5929)            (10 pontos após a divisão por 100).
     790            ( 34 / 1156)
     ...
             930 !  ( 29 /  841)        <-- João golpeou de esquerda (!).
     550            ( 49 / 2401)        <-- Maria golpeou 550ms depois.
             610    ( 44 / 1936)            A velocidade atingida foi de 44 kmh,
     820            ( 32 / 1024)            somando 1936 pontos para João
             360    ( 75 / 5625)            (19 pontos após a divisão por 100).
     700            ( 38 / 1444)
             370
   -----   -----                        <-- Queda.
     256     195                        <-- Soma acumulada por João e por Maria
                                            (após a divisão por 100).
-- Sequencia XX ----------------        <-- Outras sequências...
...

-----------------------------------------------

    Atleta    Vol     Esq     Dir   Total       <-- Volume e Potência (esq/dir)
Atleta ESQ:   297 +   201 +   525 =  1024 pts   <-- Pontuação de João
Atleta DIR:   374 +     0 +   272 =   646 pts   <-- Pontuação de Maria

-----------------------------------------------

Media ........   835 pts                        <-- Média entre os dois
Equilibrio ...   124 (-)                        <-- Desconto de equilíbrio
Quedas .......   106 (-)                        <-- Desconto de quedas
TOTAL ........   604 pts                        <-- Pontuação final da dupla
```

-------------------------------------------------------------------------------

## Instruções para o Juiz

### Ligação dos Cabos

- Celular (micro USB) -> Aparelho (mini USB)
    - É necessário um adaptador micro USB para USB
- Aparelho (PS2/AUX) -> Som (PS2/AUX)

![Ligação dos Cabos](images/all.jpg "Ligação dos Cabos")

### Aparelho marcador:

- Nova apresentação:
    - Pressione o botão do meio e em seguida o da direita por 3 segundos.
    - Resposta: um som médio de dois segundos.
- Início de sequência:
    - Pressione o botão do meio por um segundo, até escutar um som.
    - Resposta: um som agudo de meio segundo.
- Golpes dos atletas:
    - Pressione o botão da esquerda ou direita quando, respectivamente, o
      atleta à esquerda ou à direita golpearem a bola. Se o golpe for um
      backhand (ou o lado não preferencial do atleta), o pressionamento deve
      ser um pouco mais demorado.
    - Resposta: depende da velocidade (ver a seção "Fluxo da Apresentação").
- Queda de bola:
    - Pressione o botão do meio por um segundo, até escutar um som.
    - Resposta: três sons cada vez mais graves por meio segundo.
- Fim da apresentação:
    - Automático, quando o tempo do cronômetro expirar. <!-- ou após a 25a queda. -->
    - Resposta: um som grave por dois segundos.
- Desfazer última sequência:
    - Pressione o botão do meio e em seguida o da esquerda por 3 segundos.
    - Resposta: três sons cada vez mais agudos por meio segundo.
- Reconfiguração de fábrica:
    - Pressione o botão do meio e em seguida os da esquerda e direita por
      3 segundos.
    - Resposta: um som médio de dois segundos.
    - **Em princípio, esse procedimento nunca deverá ser necessário.**

### Aplicativo Android

- Instalação (apenas uma vez):
    - Instalar o app `Serial USB Terminal` (by Kai Morich).
        - <https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal>

- Configuração (apenas uma vez):
    - Tocar nas três barras paralelas no canto superior esquerdo e selecionar
      `Settings`.
    - Trocar `Baud rate` para `9600`.
    - Trocar `Font size` para `10`.
    - Trocar `Font style` para `Monospace`.
    - Desabilitar `Show timestamps`.
    - Trocar `Buffer size` para `Unlimited`.
    - Habilitar `Clear input on send`.
    - Habilitar `Keep screen on when connected`.

![Android App](images/app.jpg "Android App")
![Conexão USB](images/connected.jpg "Conexão USB")
![Configurações](images/app1.jpg "Configurações 1/5")
![Configurações](images/app2.jpg "Configurações 2/5")
![Configurações](images/app3.jpg "Configurações 3/5")
![Configurações](images/app4.jpg "Configurações 4/5")
![Configurações](images/app5.jpg "Configurações 5/5")

- Conexão (sempre que abrir o aplicativo):
    - Conectar o aparelho ao celular via cabo USB.
    - Tocar no ícone com dois cabos desconectados no centro superior direito da
      tela.
        - O ícone deve mudar para um com cabos conectados.
        - A área central do app deve exibir `Connected to...` e o placar da
          última apresentação.

<!--
    - O placar da última apresentação deve ser exibido no centro da tela.
    - A cada queda e reinício, é exibido o placar da apresentação em andamento.
-->

- Comandos (durante as apresentações):
    - Digitar o comando completo na área de texto na base inferior e em seguida
      tocar no ícone com uma seta no canto inferior direito.
    - Exibição:
        - `placar`
            - exibe o placar da apresentação
        - `relatorio`
            - exibe o relatório completo da apresentação
        - `velocidades SIM/NAO`
            - exibe (ou não) a velocidade de cada golpe em tempo real
            - Exemplo:
                - `velocidades nao`
                - desabilita a exibição dos golpes em tempo real
    - Ação:
        - `reiniciar`
            - reinicia a apresentação imediatamente
        - `terminar`
            - termina a apresentação imediatamente
        - `desfazer`
            - desconsidera por inteiro a sequência anterior
        - `restaurar`
            - restaura a configuração de fábrica
    - Configuração:
        - `tempo SEGS`
            - altera o tempo total das apresentações para `SEGS`, que deve ser
              um número em segundos
            - Exemplo:
                - `tempo 300`
                - altera o tempo de apresentação para 5 minutos
        - `distancia CMS`
            - altera a distância das apresentações para `CMS`, que deve ser um
              número em centímetros
            - Exemplo:
                - `distancia 800`
                - altera a distância para 8 metros
        - `maxima VEL`
            - altera a velocidade máxima a ser considerada para `VEL`, que deve
              ser um número em kmh (bolas acima de `VEL` serão consideradas
              como `VEL`)
            - Exemplo:
                - `maxima 90`
                - bolas acima de 90 kmh serão interpretadas como 90 kmh
        - `potencia SIM/NAO`
            - liga ou desliga a pontuação de potência (`nao=desligada`, `sim=ligada`)
            - caso desligada, a apresentação já iniciará com as 7 bolas de
              esquerda e direita premarcadas a 50kmh (e nunca serão modificadas)
            - Exemplo:
                - `potencia sim`
                - habilita a pontuação de potência
        - `equilibrio SIM/NAO`
            - liga ou desliga a pontuação de equlíbrio (`nao=desligada`, `sim=ligada`)
            - Exemplo:
                - `equilibrio nao`
                - desabilita a pontuação de equilibrio
        - `esquerda NOME`
            - altera o nome do atleta à esquerda para `NOME`, que deve ter até
              15 caracteres
            - Exemplo:
                - `esquerda Maria`
                - altera o nome do atleta à esquerda para *Maria*
        - `direita NOME`
            - altera o nome do atleta à direita para `NOME`, que deve ter até
              15 caracteres
            - Exemplo:
                - `direita Joao`
                - altera o nome do atleta à direita para *Joao*
        - `juiz NOME`
            - altera o nome do juiz para `NOME`, que deve ter até 15 caracteres
            - Exemplo:
                - `juiz Arnaldo`
                - altera o nome do juiz para *Arnaldo*

- Envio de relatórios:
    <a name="envio-de-relatorios"></a>
    - No aplicativo `Serial USB Terminal`:
        - Aguarde o término da apresentação
        - Aperte na lata de lixo no canto superior direito para apagar a tela
        - Digite `relatorio`
            - Aguarde o término da operação
        - Aperte nos 3 pontinhos no canto superior direito
            - Selecione `Save Data`
                - Vai aparecer uma mensagem tal como `Saved serial_20190602_160522.txt`
    - No `WhatsApp`:
        - Selecione o contato a enviar o relatório
        - Aperte no clipe para anexar um arquivo
            - Selecione `Documento`
        - Aperte em `Procurar outros documentos...`
        - Aperte nos 3 pontinhos no canto superior direito
            - Selecione `Mostrar armazenamento interno`
                - (Esse procedimento só é necessário uma vez)
        - Aperte nas 3 barrinhas no canto superior esquerdo
            - Selecione o modelo do seu celular, ex.: `Moto E(4)`
            - Selecione `Android`
            - Selecione `data`
            - Selecione `de.kai_morich.serial_usb_terminal`
            - Selecione `files`
            - Selecione o arquivo a ser anexado, ex.:  `serial_20190602_160522.txt`
            - Selecione `Enviar`
                - Confirme

<!--
        - `continuidade PCT`
            - altera o percentual de perda por queda de bola para `PCT`, que
              deve ser um número
-->

-------------------------------------------------------------------------------

## Perguntas e Respostas

- Qual é o objetivo desse projeto?
    - Oferecer uma maneira objetiva, simples e barata de avaliar apresentações
      de frescobol.
    - Estar disponível no maior número de arenas de frescobol que for possível.
    - Auxiliar no desenvolvimento técnico de atletas, estimular a formação de
      novos atletas e contribuir para o crescimento do Frescobol de competição.

- Como eu consigo um aparelho desses?
    - Entre em contato conosco por e-mail:
        - <go.frescobol@gmail.com>

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
      Um golpe a 50 kmh vale `50x50=2500`, uma a 70 kmh vale `70x70=4900`,
      praticamente o dobro (25 vs 49 pontos, após a divisão por 100).

- Qual é o objetivo do quesito Potência?
    - Ao bonificar os 7 golpes mais velozes tanto de esquerda quanto de
      direita, a regra incentiva que o atleta ataque acima do seu limite.
      Os 7 golpes correspondem a mais ou menos 10% dos ataques de um atleta em
      uma apresentação de 3 minutos (20% considerando esquerda e direita).
<!--
    - E por quê a regra não considera todos os 7 golpes mais velozes (no lugar
      de considerar apenas o 7o)?
        - Para minimizar a imprecisão da marcação do juiz.
          É possível que o juiz acelere a marcação de alguns golpes, mas é
          pouco provável que isso afete sensivelmente a 7a bola mais veloz.
-->

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
        Os componentes do aparelho somados custam menos de R$50.
        O radar custa em torno de US$1000 e não inclui o software para
        frescobol.
    - **Licença de uso**:
        Além do custo ser menor, não há nenhuma restrição legal sobre o uso
        do aparelho, software ou regra por terceiros.
    - **Infraestrutura**:
        Além do aparelho, é necessário apenas um celular com um software
        gratuito (para obter o placar das apresentações) e uma caixa de som
        potente (de preferência com bateria interna).
        Não é necessário computador, ponto de luz elétrica, área protegida ou
        outros ajustes finos para a medição da apresentação.
        Essa simplicidade permite que múltiplas arenas funcionem ao mesmo
        tempo.
    - **Transparência das medições**:
        Apesar de serem menos precisas, as medições são audíveis e qualquer
        erro grosseiro pode ser notado imediatamente.
        O radar só mede bolas acima de 40 kmh e não é possível identificar se
        as medições estão sempre corretas (o posicionamento dos atletas, vento
        e outros fatores externos podem afetar as medições).
    - **Verificabilidade das medições**:
        Os atletas podem verificar/auditar se a pontuação final foi justa.
        As apresentações podem ser medidas por um aparelho igual durante as
        apresentaçõs ou podem ser gravados para medição posterior pelo vídeo.

- Eu posso usar o marcador em competições? Quanto custa? A quem devo pedir
  permissão?
    - Não há nenhuma restrição de uso.
    - Não há custos.
    - Não é necessário pedir autorização.
      Não é nem mesmo necessário mencionar o nome do sistema ou autores.

- Como eu posso contribuir?
    - Adotando o sistema no dia a dia da sua arena.
        - Principalmente com jogadores iniciantes.
    - Promovendo competições.
    - Produzindo vídeos.
    - **Enviando os relatórios dos jogos para nós.**

<!--
- Como eu posso contribuir financeiramente?
    -
-->
