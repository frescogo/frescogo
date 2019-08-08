void Serial_Score (void) {
    Serial.println();
    Serial.println(F("-----------------------------------------------"));
    sprintf_P(STR, PSTR("%22s"), S.names[0]);
    Serial.print(STR);
    Serial.print(F(" / "));
    sprintf_P(STR, PSTR("%s"), S.names[1]);
    Serial.print(STR);
    Serial.println();

    Serial.println(F("-----------------------------------------------"));
    Serial.println();

    Serial.print(F("TOTAL ........ "));
    Serial.print(G.total);
    Serial.println(F(" pts"));

    Serial.print(F("Tempo ........ "));
    Serial.print(G.time);
    Serial.print(F("ms"));
    Serial.print(F(" (-"));
    Serial.print(G.time > S.timeout ? 0 : (int)ceil((S.timeout-G.time)/(float)1000));
    Serial.println(F("s)"));

    Serial.print(F("Quedas ....... "));
    Serial.println(Falls());

    Serial.print(F("Golpes ....... "));
    Serial.println(G.hits);

    Serial.print(F("Ritmo ........ "));
    if (G.time > 5000) {
        Serial.print((int)G.pace[0]);
        Serial.print(F("/"));
        Serial.print((int)G.pace[1]);
        Serial.println(F(" kmh"));
    } else {
        Serial.println(F("---"));
    }

    Serial.print(F("Juiz ......... "));
    Serial.println(S.juiz);

    for (int i=0; i<2; i++) {
        Serial.println();
        Serial.println(F("-----------------------------------------------"));
        Serial.println();

        sprintf_P(STR, PSTR("%10s: "), S.names[i]);
        Serial.print(STR);
        Serial.print(G.ps[i]/100);
        Serial.println(F(" pts"));
        for (int j=0; j<2; j++) {
            int sum = 0;
            Serial.print( (j==1) ? F(" dir ") : F(" esq ") );
            Serial.print(F(" [ "));
            for (int k=0; k<HITS_BESTS; k++) {
                sum += G.bests[i][j][k];
                sprintf_P(STR, PSTR("%3d "), (int)G.bests[i][j][k]);
                Serial.print(STR);
            }
            Serial.print(F("] => "));
            if (HITS_BESTS == 0) {
                Serial.print(F("--"));
            } else {
                Serial.print(sum/HITS_BESTS);
            }
            Serial.println(F(" kmh"));
        }
    }

    Serial.println();
    Serial.println(F("-----------------------------------------------"));
    Serial.println();

    //sprintf_P(STR, PSTR("(CONF: v%d.%d / %dcm / %ds / max=%d / equ=%d / cont=%d / max=%d)"),
    sprintf_P(STR, PSTR("(v%d%d%d/%dcm/%ds/max%d,%d/equ%d/cont%d/fim%d/sens%d)"),
                MAJOR, MINOR, REVISION,
                S.distancia,
                (int)(S.timeout/1000),
                (int)S.maximas,
                (int)S.maxima,
                (int)S.equilibrio,
                (int)CONT_PCT,
                (int)ABORT_FALLS,
                (int)S.sensibilidade);
    Serial.println(STR);
}

void Serial_Log (void) {
    Serial.println();
    Serial.println(F("-----------------------------------------------"));
    Serial.println();

    int ball  = 0;
    u32 ps[2] = {0,0};
    for (int i=0 ; i<S.hit ; i++) {
        s8  dt  = S.dts[i];
        u8  kmh = G.kmhs[i];

        if (dt == HIT_SERV) {
            ball = ball + 1;
            Serial.print(F("-- Sequencia "));
            sprintf_P(STR, PSTR("%2d"), ball);
            Serial.print(STR);
            Serial.println(F(" ----------------"));
        }

        if (dt == HIT_NONE) {
            continue;
        }

        Serial.print(F("  "));
        if (i%2 == 0) {
            if (dt < 0) {
                Serial.print(F("! "));
            } else {
                Serial.print(F("  "));
            }
        } else {
            Serial.print(F("          "));
        }

        int back2 = (i%2==1 and dt<0);

        if (dt == HIT_SERV) {
            Serial.print(F("****"));
        } else {
            sprintf_P(STR, PSTR("%4d"), (dt>0?dt:-dt)*10);
            Serial.print(STR);
            if (back2) {
                Serial.print(F(" !"));
            }
        }

        if (i==S.hit-1 || S.dts[i+1]==HIT_NONE || S.dts[i+1]==HIT_SERV) {
            Serial.println();
            Serial.println(F("   -----   -----"));
            sprintf_P(STR, PSTR("   %5ld   %5ld"), ps[0]/100, ps[1]/100);
            Serial.println(STR);
        } else if (dt == HIT_SERV) {
            // ignore
        } else {
            u16 pt = ((u16)kmh)*((u16)kmh);
            ps[1 - i%2] += pt;

            Serial.print(F("  "));
            if (!back2) {
                Serial.print(F("  "));
            }
            if (i%2 == 0) {
                Serial.print(F("        "));
            }
            sprintf_P(STR, PSTR("(%3d / %4d)"), kmh, pt);
            Serial.print(STR);
        }
        Serial.println();
        delay(50);
    }
    //Serial.println();

    u32 bests[2][2] = { {0,0}, {0,0} };
    if (S.maximas) {
        for (int i=0; i<2; i++) {
            for (int j=0; j<2; j++) {
                int sum = 0;
                for (int k=0; k<HITS_BESTS; k++) {
                    s8 v = G.bests[i][j][k];
                    if (!S.maximas) {
                        v = POT_VEL;
                    }
                    sum += v;
                }
                int avg = sum / HITS_BESTS;
                bests[i][j] = avg*avg * POT_BONUS * HITS_BESTS;
            }
        }
    }

    u32 p0 = ps[0] + bests[0][0] + bests[0][1];
    u32 p1 = ps[1] + bests[1][0] + bests[1][1];

    Serial.println(F("-----------------------------------------------"));
    Serial.println();

    Serial.println(F("    Atleta    Vol     Esq     Dir   Total"));
    sprintf_P(STR, PSTR("%10s: %5ld + %5ld + %5ld = %5ld pts"),
        S.names[0], ps[0]/100, bests[0][0]/100, bests[0][1]/100, p0/100);
    Serial.println(STR);
    sprintf_P(STR, PSTR("%10s: %5ld + %5ld + %5ld = %5ld pts"),
        S.names[1], ps[1]/100, bests[1][0]/100, bests[1][1]/100, p1/100);
    Serial.println(STR);

    Serial.println();
    Serial.println(F("-----------------------------------------------"));
    Serial.println();

    u32 avg   = (p0 + p1) / 2;
    u32 total = (S.equilibrio ? min(avg, min(p0,p1)*11/10) : avg);
/*
    sprintf_P(STR, PSTR("MED: %ld | MIN+10%%: %ld | MIN: %ld"),
                    (p0 + p1) / 2 / 100,
                     min(p0,p1)*11 / 1000,
                     total / 100);
    Serial.println(STR);
*/

    u32 pct = min(990, Falls()*CONT_PCT);
    //sprintf_P(STR, PSTR(">>> %ld x %d%% = %ld"), total/100, pct, total*pct/10000);
    sprintf_P(STR, PSTR("Media ........ %5ld"), avg/100);
    Serial.print(STR);
    Serial.println(F(" pts"));
    sprintf_P(STR, PSTR("Equilibrio ... %5ld (-)"), (S.equilibrio ? (avg/100)-(total/100) : 0));
    Serial.println(STR);
    sprintf_P(STR, PSTR("Quedas ....... %5ld (-)"), total*pct/100000);
    Serial.println(STR);
    sprintf_P(STR, PSTR("TOTAL ........ %5ld"), total*(1000-pct)/100000);
    Serial.print(STR);
    Serial.println(F(" pts"));
}

int Serial_Check (void) {
    static char CMD[32];
    static int  i = 0;

    char c;
    while (Serial.available()) {
        c = Serial.read();
        if (c=='\n' || c=='\r' || c=='$' ) {
            if (i == 0) {
                                // skip
            } else {
                CMD[i] = '\0';
                goto _COMPLETE;   // complete
            }
        } else {
            CMD[i++] = c;       // continue
        }
    }
    return IN_NONE;
_COMPLETE:
    i = 0;

    if (strncmp_P(CMD, PSTR("modo cel"), 8) == 0) {
        S.modo = MODE_CEL;
    } else if (strncmp_P(CMD, PSTR("modo pc"), 7) == 0) {
        S.modo = MODE_PC;
    } else if (strncmp_P(CMD, PSTR("restaurar"), 9) == 0) {
        return IN_RESET;
    } else if (strncmp_P(CMD, PSTR("reiniciar"), 9) == 0) {
        return IN_RESTART;
    } else if (strncmp_P(CMD, PSTR("terminar"), 8) == 0) {
        return IN_TIMEOUT;
    } else if (strncmp_P(CMD, PSTR("desfazer"), 8) == 0) {
        return IN_UNDO;
    } else if (strncmp_P(CMD, PSTR("placar"), 6) == 0) {
        Serial_Score();
        return IN_NONE;
    } else if (strncmp_P(CMD, PSTR("relatorio"), 9) == 0) {
        Serial_Score();
        Serial_Log();
        return IN_NONE;
    } else if (strncmp_P(CMD, PSTR("tempo "), 6) == 0) {
        S.timeout = ((u32)atoi(&CMD[6])) * 1000;
    } else if (strncmp_P(CMD, PSTR("distancia "), 5) == 0) {
        S.distancia = atoi(&CMD[10]);
    } else if (strncmp_P(CMD, PSTR("maximas sim"), 11) == 0) {
        S.maximas = 1;
    } else if (strncmp_P(CMD, PSTR("maximas nao"), 11) == 0) {
        S.maximas = 0;
    } else if (strncmp_P(CMD, PSTR("equilibrio sim"), 14) == 0) {
        S.equilibrio = 1;
    } else if (strncmp_P(CMD, PSTR("equilibrio nao"), 14) == 0) {
        S.equilibrio = 0;
    } else if (strncmp_P(CMD, PSTR("maxima "), 7) == 0) {
        S.maxima = atoi(&CMD[7]);
    } else if (strncmp_P(CMD, PSTR("sensibilidade "), 13) == 0) {
        S.sensibilidade = min(SENS_MAX, atoi(&CMD[13]));
/*
    } else if (strncmp_P(CMD, PSTR("continuidade "), 13) == 0) {
        S.continuidade = atoi(&CMD[13]);
*/
    } else if (strncmp_P(CMD, PSTR("esquerda "), 9) == 0) {
        if (strlen(&CMD[9]) < 15) {
            strcpy(S.names[0], &CMD[9]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("direita "), 8) == 0) {
        if (strlen(&CMD[8]) < 15) {
            strcpy(S.names[1], &CMD[8]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("juiz "), 5) == 0) {
        if (strlen(&CMD[5]) < 15) {
            strcpy(S.juiz, &CMD[5]);
        } else {
            goto ERR;
        }
    } else {
        goto ERR;
    }

    if (0) {
ERR:;
        Serial.println(F("err"));
OK:;
    } else {
        Serial.println(F("ok"));
    }
    EEPROM_Save();
    PT_All();
    Serial_Score();

    return IN_NONE;
}
