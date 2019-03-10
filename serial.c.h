void Serial_Hit (char* name, u32 kmh, bool is_back) {
    Serial.print(F("> "));
    Serial.print(name);
    Serial.print(F(": "));
    Serial.print(kmh);
    if (is_back) {
        Serial.print(F(" !"));
    }
    Serial.println();
}

void Serial_Score (void) {
    Serial.println();
    Serial.println(F("--------------------------------"));
    sprintf_P(STR, PSTR("%10s"), S.names[0]);
    Serial.print(STR);
    Serial.print(F(" / "));
    sprintf_P(STR, PSTR("%s"), S.names[1]);
    Serial.print(STR);
    Serial.println();

    Serial.print(F("    ("));
    Serial.print(S.distance);
    Serial.print(F("cm - "));
    Serial.print(S.timeout/1000);
    Serial.println(F("s)"));

    Serial.println(F("--------------------------------"));
    Serial.println();

    sprintf_P(STR, PSTR("%10S: "), F("TOTAL"));
    Serial.print(STR);
    Serial.println(G.total);

    sprintf_P(STR, PSTR("%10S: "), F("Tempo"));
    Serial.print(STR);
    Serial.print(G.time);
    Serial.println(F("ms"));

    sprintf_P(STR, PSTR("%10S: "), F("Quedas"));
    Serial.print(STR);
    Serial.println(Falls());

    sprintf_P(STR, PSTR("%10S: "), F("Golpes"));
    Serial.print(STR);
    Serial.println(G.hits);

    sprintf_P(STR, PSTR("%10S: "), F("Ritmo"));
    Serial.print(STR);
    if (G.time > 5000) {
        Serial.println((int)G.pace);
    } else {
        Serial.println("---");
    }

    Serial.println();
    for (int i=0; i<2; i++) {
        sprintf_P(STR, PSTR("%10s: "), S.names[i]);
        Serial.print(STR);
        Serial.println(G.ps[i]/100);
        for (int j=0; j<2; j++) {
            Serial.print(F(" [ "));
            for (int k=0; k<HITS_BESTS; k++) {
                sprintf_P(STR, PSTR("%3d "), (int)G.bests[i][j][k]);
                Serial.print(STR);
            }
            Serial.println(F("]"));
        }
        Serial.println();
    }
}

void Serial_Log (void) {
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
            sprintf_P(STR, PSTR("(%3d^2 = %5d)"), kmh, pt);
            Serial.print(STR);
        }
        Serial.println();
    }
    Serial.println();

    u32 bests[2][2] = { {0,0}, {0,0} };
    for (int i=0; i<2; i++) {
        for (int j=0; j<2; j++) {
            u32 sum = 0;
            for (int k=0; k<HITS_BESTS; k++) {
                u32 pt = G.bests[i][j][k];
                sum += pt*pt*4;
            }
            bests[i][j] = sum;
        }
    }

    u32 p0 = ps[0] + bests[0][0] + bests[0][1];
    u32 p1 = ps[1] + bests[1][0] + bests[1][1];

    Serial.println(F("--------------------------------"));
    Serial.println();
    sprintf_P(STR, PSTR("%10s: %5ld + %5ld +%5ld = %5ld"),
        S.names[0], ps[0]/100, bests[0][0]/100, bests[0][1]/100, p0/100);
    Serial.println(STR);
    sprintf_P(STR, PSTR("%10s: %5ld + %5ld +%5ld = %5ld"),
        S.names[1], ps[1]/100, bests[1][0]/100, bests[1][1]/100, p1/100);
    Serial.println(STR);

    u32 avg   = (p0 + p1) / 2;
    u32 total = min(avg, min(p0,p1)*11/10);
    sprintf_P(STR, PSTR("MED: %ld | MIN+10%%: %ld | MIN: %ld"),
                    (p0 + p1) / 2 / 100,
                     min(p0,p1)*11 / 1000,
                     total / 100);
    Serial.println(STR);
    int pct = 100 - min(100, Falls()*3);
    sprintf_P(STR, PSTR(">>> %ld x %d%% = %ld"), total/100, pct, total*pct/10000);
    Serial.println(STR);
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

    if (strncmp_P(CMD, PSTR("fim"), 3) == 0) {
        return IN_TIMEOUT;
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
        S.distance = atoi(&CMD[10]);
    } else if (strncmp_P(CMD, PSTR("esquerda "), 4) == 0) {
        if (strlen(&CMD[9]) < 15) {
            strcpy(S.names[0], &CMD[9]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("direita "), 4) == 0) {
        if (strlen(&CMD[8]) < 15) {
            strcpy(S.names[1], &CMD[8]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("-seq"), 4) == 0) {
        if (S.hit == 0) {
            goto ERR;
        }
        while (1) {
            S.hit -= 1;
            if (S.hit == 0) {
                break;
            } else if (S.dts[S.hit] == HIT_SERV) {
                if (S.dts[S.hit-1] == HIT_NONE) {
                    S.hit -= 1;
                }
                break;
            }
        }
    } else if (strncmp_P(CMD, PSTR("+seq"), 4) == 0) {
        if (S.dts[S.hit] == HIT_MARK) {
            goto ERR;
        }
        while (1) {
            S.hit += 1;
            if (S.dts[S.hit]==HIT_MARK or S.dts[S.hit]==HIT_SERV) {
                break;
            }
        }
    } else if (strncmp_P(CMD, PSTR("-1"), 2) == 0) {
        if (S.hit > 0) {
            S.hit -= 1;
            if (S.dts[S.hit] == HIT_NONE) {
                S.hit -= 1;
            }
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("+1"), 2) == 0) {
        if (S.dts[S.hit] != HIT_MARK) {
            S.hit += 1;
            if (S.dts[S.hit] == HIT_SERV) {
                S.hit += 1;    // skip HIT_NONE
            }
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
    TV_All("CMD", 0, 0, 0);

    return IN_NONE;
}
