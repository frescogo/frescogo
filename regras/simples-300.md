<!--
pandoc simples-300.md -H deeplists.tex -o /tmp/x.pdf
pdftoppm /tmp/x.pdf /tmp/x -png
convert /tmp/x-1.png -trim /tmp/x.png
convert /tmp/x.png -bordercolor White -border 8 simples-300.png
eog simples-300.png
-->

\pagenumbering{gobble}

## FrescoGO! - Regra para 5:00 minutos

Cada atleta é avaliado individualmente em três médias de velocidade:

- Volume:
    média de velocidade de todos os golpes (ataques ou defesas).
- Máximas:
    - Normal:
        média de velocidade dos `24` golpes mais fortes.
    - Revés:
        média de velocidade dos `16` golpes mais fortes de revés. (opcional)

As três médias constituem a nota de cada atleta com os seguintes pesos:

- `60%` Volume, `25%` Normal, `15%` Revés (quando o revés está habilitado)
- `75%` Volume, `25%` Normal, `0%` Revés (quando o revés está desabilitado)

A pontuação final da dupla será a média entre as notas dos dois atletas.

Caso a nota de um dos atletas esteja muito abaixo da média (margem de `5%`),
então a pontuação final da dupla será essa menor nota.

Cada queda ainda desconta `1%` da pontuação final.
Com 20 quedas, a apresentação é encerrada.
