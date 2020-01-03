<!--
pandoc simples-150.md -H deeplists.tex -o /tmp/x.pdf
pdftoppm /tmp/x.pdf /tmp/x -png
convert /tmp/x-1.png -trim /tmp/x.png
convert /tmp/x.png -bordercolor White -border 8 simples-150.png
eog simples-150.png
-->

\pagenumbering{gobble}

## FrescoGO! - Regra para 2:30 minutos

Cada atleta é avaliado individualmente em três médias de velocidade:

- Volume:
    média de todos os golpes (ataques ou defesas).
- Máximas:
    - Normal:
        média dos `12` golpes mais fortes.
    - Revés:
        média dos `8` golpes mais fortes de revés (opcional).

As três médias constituem a nota de cada atleta com os seguintes pesos:

- `60%` Volume, `25%` Normal, `15%` Revés.

A pontuação final da dupla será a média entre as duas notas.

Caso a nota de um dos atletas esteja muito abaixo da média (margem de `5%`),
então a pontuação será a menor nota.

Cada queda ainda desconta `4%` da pontuação final.