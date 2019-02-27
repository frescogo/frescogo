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
    sprintf_P(STR, PSTR("%15s"), S.names[0]);
    Serial.print(STR);
    Serial.print(F(" / "));
    sprintf_P(STR, PSTR("%s"), S.names[1]);
    Serial.print(STR);
    Serial.println();

    Serial.print(F("         ("));
    Serial.print(S.distance);
    Serial.print(F("cm - "));
    Serial.print(S.timeout/1000);
    Serial.println(F("s)"));

    Serial.println(F("--------------------------------"));
    Serial.println();

    sprintf_P(STR, PSTR("%15S: "), F("TOTAL"));
    Serial.print(STR);
    Serial.println(G.total);

    sprintf_P(STR, PSTR("%15S: "), F("Tempo"));
    Serial.print(STR);
    Serial.print(G.time);
    Serial.println(F("ms"));

    sprintf_P(STR, PSTR("%15S: "), F("Quedas"));
    Serial.print(STR);
    Serial.println(Falls());

    sprintf_P(STR, PSTR("%15S: "), F("Golpes"));
    Serial.print(STR);
    Serial.println(G.hits);

    sprintf_P(STR, PSTR("%15S: "), F("Ritmo"));
    Serial.print(STR);
    if (G.time > 5000) {
        Serial.println((int)G.pace);
    } else {
        Serial.println("---");
    }

    Serial.println();
    for (int i=0; i<2; i++) {
        sprintf_P(STR, PSTR("%15s: "), S.names[i]);
        Serial.print(STR);
        Serial.println(G.ps[i]/100);
        for (int j=0; j<2; j++) {
            Serial.print(F(" [ "));
            for (int k=0; k<HITS_BESTS; k++) {
                Serial.print((int)G.bests[i][j][k]);
                Serial.print(" ");
            }
            Serial.println(F("]"));
        }
        Serial.println();
    }
}

void Serial_Log (void) {
    int i = -1;
    int bola = 0;
    while (1) {
        i = i + 1;
        if (i == S.hit) {
            break;
        }
        s8  dt  = S.dts[i];
        u8  kmh = G.kmhs[i];

        if (dt == HIT_SERV) {
            bola = bola + 1;
            Serial.print(F("-- Sequencia "));
            sprintf_P(STR, PSTR("%2d"), bola);
            Serial.print(STR);
            Serial.println(F(" ----------------"));
        }

        if (dt == HIT_NONE) {
            continue;
        }

        if (i%2 == 0) {
            Serial.print(F("         "));
            if (dt < 0) {
                Serial.print(F("! "));
            } else {
                Serial.print(F("  "));
            }
        } else {
            Serial.print(F("                 "));
        }

        if (dt == HIT_SERV) {
            Serial.println(F("****"));
        } else {
            sprintf_P(STR, PSTR("%4d"), dt*10);
            Serial.print(STR);
            if (i%2==1 and dt<0) {
                Serial.print(F(" !"));
            }
            Serial.println();
        }
    }
    Serial.println();
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

    if (strncmp_P(CMD, PSTR("reinicio"), 8) == 0) {
        return IN_TIMEOUT;
    } else if (strncmp_P(CMD, PSTR("placar"), 6) == 0) {
        Serial_Score();
        return IN_NONE;
    } else if (strncmp_P(CMD, PSTR("relatorio"), 9) == 0) {
        Serial_Score();
        Serial_Log();
        return IN_NONE;
    } else if (strncmp_P(CMD, PSTR("tempo "), 6) == 0) {
        String str = &CMD[6];
        S.timeout = str.toInt() * 1000;
    } else if (strncmp_P(CMD, PSTR("dist "), 5) == 0) {
        String str = &CMD[5];
        S.distance = str.toInt();
    } else if (strncmp_P(CMD, PSTR("esq "), 4) == 0) {
        if (strlen(&CMD[4]) < 15) {
            strcpy(S.names[0], &CMD[4]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("dir "), 4) == 0) {
        if (strlen(&CMD[4]) < 15) {
            strcpy(S.names[1], &CMD[4]);
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
