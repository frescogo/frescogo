<!--
pandoc regra-300.md -H deeplists.tex -o /tmp/x.pdf
pdftoppm /tmp/x.pdf /tmp/x -png
convert /tmp/x-1.png -trim /tmp/x.png
convert /tmp/x.png -bordercolor White -border 8 regra-300.png
eog regra-300.png
-->

\pagenumbering{gobble}

## Regra para 5:00 minutos

- **Golpes:**
    - Cada atleta é avaliado individualmente pelas médias de velocidade dos
      seguintes quesitos:
        - **Volume:**
            - Média *quadrática* da velocidade de todos os seus golpes.
                - (A média quadrática considera as velocidades elevadas ao
                   quadrado.)
        - **Máximas - Normal:**
            - Média simples da velocidade dos `24` golpes mais fortes efetuados
              pelo seu lado preferencial ("lado normal").
        - **Máximas - Revés:**
            - Esse quesito é opcional e pode ser desabilitado.
            - Média simples da velocidade dos `16` golpes mais fortes efetuados
              pelo seu lado não preferencial ("lado revés").
            - O revés só é válido quando supera em 10% a velocidade do golpe
              anterior do parceiro.
    - O total do atleta é a média ponderada entre os três quesitos.
        - Revés habilitado:
            - `ATLETA = 60% Volume + 25% Normal + 15% Revés`
        - Revés desabilitado:
            - `ATLETA = 75% Volume + 25% Normal + 0% Revés`
- **Equilíbrio:**
    - A pontuação de equilíbrio da dupla é a média entre os totais dos dois
      atletas.
    - Se um atleta estiver abaixo dessa média (com uma margem de 5%), então a
      pontuação de equilíbrio será o menor total:
        - `EQUILIB = MENOR((ATL1+ATL2)/2, MENOR(ATL1,ATL2)x1.05)`
- **Continuidade:**
    - A apresentação é encerrada sumariamente na `20a` queda.
    - Os dois últimos golpes antes de cada queda são sempre desconsiderados.
    - Cada queda subtrai `1%` da pontuação após calcular o equilíbrio da dupla:
        - `FINAL = EQUILIB x (100 - 1x QUEDAS) / 100`

```
  ATLETA  = 60% Volume + 25% Normal + 15% Revés     (75% Volume com o Revés desabilitado)
  EQUILIB = MENOR( (ATLETA1+ATLETA2)/2, MENOR(ATLETA1,ATLETA2)x1.05 )
  FINAL   = EQUILIB x (100 - 1x QUEDAS) / 100
```

Em caso de empate entre duplas, os seguintes quesitos serão usados para
desempate: (1) menor quantidade de quedas, (2) maior quantidade de golpes, (3)
sorteio.
