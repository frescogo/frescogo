void Serial_Hit (char* name, u32 kmh, bool is_back) {
    pserial.print(F("> "));
    pserial.print(name);
    pserial.print(F(": "));
    pserial.print(kmh);
    if (is_back) {
        pserial.print(F(" !"));
    }
    pserial.println();
}

void Serial_All (void) {
    u32 avg  = (GAME.ps[0] + GAME.ps[1]) / 2;
    u32 pace = avg * 10 / GAME.time;

    // BEFORE GET_TOTAL: pace

    u32 total = Get_Total(GAME.falls) / 100;

    // AFTER GET_TOTAL: p0/p1

    pserial.println();
    pserial.println(F("--------------------------------"));
    sprintf_P(STR, PSTR("%15s"), NAMES[0]);
    pserial.print(STR);
    pserial.print(F(" / "));
    sprintf_P(STR, PSTR("%s"), NAMES[1]);
    pserial.print(STR);
    pserial.println();

    pserial.print(F("         ("));
    pserial.print(DISTANCE);
    pserial.print(F("cm - "));
    pserial.print(TIMEOUT/1000);
    pserial.println(F("s)"));

    pserial.println(F("--------------------------------"));
    pserial.println();

    sprintf_P(STR, PSTR("%15S: "), F("TOTAL"));
    pserial.print(STR);
    pserial.print(total);
    if (GAME.time > 5000) {
        pserial.print(F(" ("));
        pserial.print(pace);
        pserial.print(F(")"));
    }
    pserial.println();

    sprintf_P(STR, PSTR("%15S: "), F("Tempo"));
    pserial.print(STR);
    pserial.print(GAME.time);
    pserial.println(F("ms"));

    sprintf_P(STR, PSTR("%15S: "), F("Quedas"));
    pserial.print(STR);
    pserial.println(GAME.falls);

    sprintf_P(STR, PSTR("%15S: "), F("Golpes"));
    pserial.print(STR);
    pserial.println(GAME.hits);

    for (int i=0; i<2; i++) {
        sprintf_P(STR, PSTR("%15s: "), NAMES[i]);
        pserial.print(STR);
        pserial.println(GAME.ps[i]/100);
        for (int j=0; j<2; j++) {
            pserial.print(F(" [ "));
            for (int k=0; k<BESTS; k++) {
                pserial.print(GAME.bests[i][j][k]);
                pserial.print(" ");
            }
            pserial.println(F("]"));
        }
        pserial.println();
    }

    int i = -1;
    int bola = 0;
    while (1) {
        i = i + 1;
        if (i == HIT) {
            break;
        }
        Hit v = HITS[i];

        if (v.dt == BALL_SERVICE) {
            bola = bola + 1;
            pserial.print(F("-- Sequencia "));
            sprintf_P(STR, PSTR("%2d"), bola);
            pserial.print(STR);
            pserial.println(F(" ----------------"));
        }

        if (v.dt == BALL_NONE) {
            continue;
        }

        if (i%2 == 0) {
            pserial.print(F("         "));
            if (v.kmh < 0) {
                pserial.print(F("! "));
            } else {
                pserial.print(F("  "));
            }
        } else {
            pserial.print(F("                 "));
        }

        if (v.dt == BALL_SERVICE) {
            pserial.println(F("****"));
        } else {
            sprintf_P(STR, PSTR("%4d"), v.dt);
            pserial.print(STR);
            if (i%2==1 and v.kmh<0) {
                pserial.print(F(" !"));
            }
            pserial.println();
        }
    }
    pserial.println();
}

int Serial_Check (void) {
    static char CMD[32];
    static int  i = 0;

    if (! pserial.available()) {
        return 0;
    }

    while (pserial.available()) {
        CMD[i++] = pserial.read();
    }

    char last = CMD[i-1];
    if (last!='\n' && last!='\r') {
        return 0;
    }

    while (STR[i-1]=='\n' || STR[i-1]=='\r') {
        i--;
    }
    STR[i++] = '\0';

    if (strncmp_P(STR, PSTR("reinicio"), 8) == 0) {
        return -1;
    } else if (strncmp_P(STR, PSTR("placar"), 6) == 0) {
        Serial_All();
    } else if (strncmp_P(STR, PSTR("tempo "), 6) == 0) {
        String str = &STR[6];
        TIMEOUT = str.toInt() * 1000;
    } else if (strncmp_P(STR, PSTR("dist "), 5) == 0) {
        String str = &STR[5];
        DISTANCE = str.toInt();
    } else if (strncmp_P(STR, PSTR("esq"), 3) == 0) {
        if (strlen(&STR[3]) < 15) {
            strcpy(NAMES[0], &STR[3]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(STR, PSTR("dir"), 3) == 0) {
        if (strlen(&STR[3]) < 15) {
            strcpy(NAMES[1], &STR[3]);
        } else {
            goto ERR;
        }
    } else if (strncmp_P(STR, PSTR("-seq"), 4) == 0) {
        if (HIT == 0) {
            goto ERR;
        }
        while (1) {
            HIT -= 1;
            if (HIT == 0) {
                break;
            } else if (HITS[HIT].dt == BALL_SERVICE) {
                if (HITS[HIT-1].dt == BALL_NONE) {
                    HIT -= 1;
                }
                break;
            }
        }
    } else if (strncmp_P(STR, PSTR("+seq"), 4) == 0) {
        if (HITS[HIT].dt == BALL_MARK) {
            goto ERR;
        }
        while (1) {
            HIT += 1;
            if (HITS[HIT].dt==BALL_MARK or HITS[HIT].dt==BALL_SERVICE) {
                break;
            }
        }
    } else if (strncmp_P(STR, PSTR("-1"), 2) == 0) {
        if (HIT > 0) {
            HIT -= 1;
            if (HITS[HIT].dt == BALL_NONE) {
                HIT -= 1;
            }
        } else {
            goto ERR;
        }
    } else if (strncmp_P(STR, PSTR("+1"), 2) == 0) {
        if (HITS[HIT].dt != BALL_MARK) {
            HIT += 1;
            if (HITS[HIT].dt == BALL_SERVICE) {
                HIT += 1;    // skip BALL_NONE
            }
        } else {
            goto ERR;
        }
    } else {
        goto ERR;
    }

    if (0) {
ERR:;
        pserial.println(F("err"));
OK:;
    } else {
        pserial.println(F("ok"));
    }
    ALL();
    TV_All("CMD", 0, 0, 0);

    return 0;
}
