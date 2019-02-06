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
    u32 avg  = (GAME.ps[0] + GAME.ps[1]) / 2;
    u32 pace = avg * 10 / GAME.time;

    // BEFORE GET_TOTAL: pace

    u32 total = PT_Total(GAME.falls) / 100;

    // AFTER GET_TOTAL: p0/p1

    Serial.println();
    Serial.println(F("--------------------------------"));
    sprintf_P(STR, PSTR("%15s"), NAMES[0]);
    Serial.print(STR);
    Serial.print(F(" / "));
    sprintf_P(STR, PSTR("%s"), NAMES[1]);
    Serial.print(STR);
    Serial.println();

    Serial.print(F("         ("));
    Serial.print(DISTANCE);
    Serial.print(F("cm - "));
    Serial.print(TIMEOUT/1000);
    Serial.println(F("s)"));

    Serial.println(F("--------------------------------"));
    Serial.println();

    sprintf_P(STR, PSTR("%15S: "), F("TOTAL"));
    Serial.print(STR);
    Serial.print(total);
    if (GAME.time > 5000) {
        Serial.print(F(" ("));
        Serial.print(pace);
        Serial.print(F(")"));
    }
    Serial.println();

    sprintf_P(STR, PSTR("%15S: "), F("Tempo"));
    Serial.print(STR);
    Serial.print(GAME.time);
    Serial.println(F("ms"));

    sprintf_P(STR, PSTR("%15S: "), F("Quedas"));
    Serial.print(STR);
    Serial.println(GAME.falls);

    sprintf_P(STR, PSTR("%15S: "), F("Golpes"));
    Serial.print(STR);
    Serial.println(GAME.hits);
}

void Serial_Log (void) {
    for (int i=0; i<2; i++) {
        sprintf_P(STR, PSTR("%15s: "), NAMES[i]);
        Serial.print(STR);
        Serial.println(GAME.ps[i]/100);
        for (int j=0; j<2; j++) {
            Serial.print(F(" [ "));
            for (int k=0; k<HITS_BESTS; k++) {
                Serial.print((int)GAME.bests[i][j][k]);
                Serial.print(" ");
            }
            Serial.println(F("]"));
        }
        Serial.println();
    }

    int i = -1;
    int bola = 0;
    while (1) {
        i = i + 1;
        if (i == HIT) {
            break;
        }
        Hit v = HITS[i];

        if (v.dt == HIT_SERVICE) {
            bola = bola + 1;
            Serial.print(F("-- Sequencia "));
            sprintf_P(STR, PSTR("%2d"), bola);
            Serial.print(STR);
            Serial.println(F(" ----------------"));
        }

        if (v.dt == HIT_NONE) {
            continue;
        }

        if (i%2 == 0) {
            Serial.print(F("         "));
            if (v.kmh < 0) {
                Serial.print(F("! "));
            } else {
                Serial.print(F("  "));
            }
        } else {
            Serial.print(F("                 "));
        }

        if (v.dt == HIT_SERVICE) {
            Serial.println(F("****"));
        } else {
            sprintf_P(STR, PSTR("%4d"), v.dt*10);
            Serial.print(STR);
            if (i%2==1 and v.kmh<0) {
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
                goto COMPLETE;   // complete
            }
        } else {
            CMD[i++] = c;       // continue
        }
    }
    return 0;
COMPLETE:
    i = 0;

    if (strncmp_P(CMD, PSTR("reinicio"), 8) == 0) {
        return -1;
    } else if (strncmp_P(CMD, PSTR("placar"), 6) == 0) {
        Serial_Score();
    } else if (strncmp_P(CMD, PSTR("relatorio"), 9) == 0) {
        Serial_Score();
        Serial_Log();
    } else if (strncmp_P(CMD, PSTR("tempo "), 6) == 0) {
        String str = &CMD[6];
        TIMEOUT = str.toInt() * 1000;
    } else if (strncmp_P(CMD, PSTR("dist "), 5) == 0) {
        String str = &CMD[5];
        DISTANCE = str.toInt();
    } else if (strncmp_P(CMD, PSTR("esq "), 4) == 0) {
        if (strlen(&CMD[4]) < 15) {
            strcpy(NAMES[0], &CMD[4]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("dir "), 4) == 0) {
        if (strlen(&CMD[4]) < 15) {
            strcpy(NAMES[1], &CMD[4]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("-seq"), 4) == 0) {
        if (HIT == 0) {
            goto ERR;
        }
        while (1) {
            HIT -= 1;
            if (HIT == 0) {
                break;
            } else if (HITS[HIT].dt == HIT_SERVICE) {
                if (HITS[HIT-1].dt == HIT_NONE) {
                    HIT -= 1;
                }
                break;
            }
        }
    } else if (strncmp_P(CMD, PSTR("+seq"), 4) == 0) {
        if (HITS[HIT].dt == HIT_MARK) {
            goto ERR;
        }
        while (1) {
            HIT += 1;
            if (HITS[HIT].dt==HIT_MARK or HITS[HIT].dt==HIT_SERVICE) {
                break;
            }
        }
    } else if (strncmp_P(CMD, PSTR("-1"), 2) == 0) {
        if (HIT > 0) {
            HIT -= 1;
            if (HITS[HIT].dt == HIT_NONE) {
                HIT -= 1;
            }
        } else {
            goto ERR;
        }
    } else if (strncmp_P(CMD, PSTR("+1"), 2) == 0) {
        if (HITS[HIT].dt != HIT_MARK) {
            HIT += 1;
            if (HITS[HIT].dt == HIT_SERVICE) {
                HIT += 1;    // skip HIT_NONE
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
    PT_All();
    TV_All("CMD", 0, 0, 0);

    return 0;
}
