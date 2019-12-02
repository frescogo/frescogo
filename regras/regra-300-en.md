<!--
pandoc regra-300-en.md -H deeplists.tex -o /tmp/x.pdf
pdftoppm /tmp/x.pdf /tmp/x -png
convert /tmp/x-1.png -trim /tmp/x.png
convert /tmp/x.png -bordercolor White -border 8 regra-300-en.png
eog regra-300-en.png
-->

\pagenumbering{gobble}

## Rules for 5:00 minutes

- **Shots:**
    - Each athlete is evaluated individually considering the average speeds of
      the criteria that follows:
        - **Volume:**
            - *Quadratic* mean of all of his shots.
                - (The quadratic means considers the squares of the speeds.)
        - **Maximums - Normal:**
            - Simple mean of his `24` strongest shots from his preferable side
              ("normal side").
        - **Maximums - Reverse:**
            - Simple mean of his `16` strongest shots from his non-preferable
              side ("reverse side").
            - A reverse shot is valid only if it surpasses in 10% his partner's
              previous shot.
    - The athlete's total is the weighted mean among the three criteria:
        - `ATHLETE = 60% Volume + 25% Normal + 15% Reverse`
- **Equilibrium:**
    - The equilibrium score of the double is the average of the athletes'
      scores.
    - If an athlete is below this average (with a `5%` margin), then the
      equilibrium score becomes the smallest score:
        - `EQUILIB = MIN( (ATL1+ATL2)/2, MIN(ATL1,ATL2)x1.05 )`
- **Continuity:**
    - The performance is summarily finished on the `20th` fall.
    - The last two shots before each fall are always disconsidered.
    - Each fall subtracts `2%` from the score after calculating the
      equilibrium:
        - `FINAL = EQUILIB x (100 - 2x FALLS) / 100`

```
  ATHLETE = 60% Volume + 25% Normal + 15% Reverse
  EQUILIB = MENOR( (ATHLETE1+ATHLETE2)/2, MENOR(ATHLETE1,ATHLETE2)x1.05 )
  FINAL   = EQUILIB x (100 - 2x FALLS) / 100
```
