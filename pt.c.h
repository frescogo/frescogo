int PT_Bests (s8* bests, int* min_, int* max_) {
    *min_ = bests[HITS_BESTS-1];
    *max_ = bests[0];
    for (int i=0; i<HITS_BESTS; i++) {
        if (bests[i] == 0) {
            return i;
        }
    }
    return HITS_BESTS;
}

void PT_Bests_Apply (void) {
    for (int i=0; i<2; i++) {
        for (int j=0; j<2; j++) {
            for (int k=0; k<HITS_BESTS; k++) {
                s8 v = GAME.bests[i][j][k];
                GAME.ps[i] += v*v*4;
            }
        }
    }
}

u32 PT_Total (int falls) {
    PT_Bests_Apply();
    u32 avg   = (GAME.ps[0] + GAME.ps[1]) / 2;
    u32 total = min(avg, min(GAME.ps[0],GAME.ps[1])*1.1);
    int pct   = 100 - min(100, (falls)*3);
    return total * pct/100;
}

void PT_All (void) {
    GAME.ps[0] = 0;
    GAME.ps[1] = 0;
    GAME.time  = 0;
    GAME.hits  = 0;
    GAME.falls = 0;

    memset(GAME.bests, 0, 2*2*HITS_BESTS*sizeof(s8));

    for (int i=0 ; i<HIT ; i++) {
    //for (int i=0 ; i<600 ; i++) {
        Hit v = HITS[i];
        s8  kmh = (v.kmh >= 0 ? v.kmh : -v.kmh);
        u16 pt  = ((u16)kmh)*((u16)kmh);

        if (v.dt != HIT_NONE) {
            GAME.hits++;
        }

        if (v.dt == HIT_SERVICE) {
            GAME.falls++;
        }

        if (v.dt!=HIT_NONE && v.dt!=HIT_SERVICE) {
            Hit nxt = HITS[i+1];
            if (i==HIT-1 || nxt.dt==HIT_NONE || nxt.dt==HIT_SERVICE) {
                // ignore last hit
            }
            else
            {
                // ps
                GAME.ps[1-(i%2)] += pt;

                // bests
                s8* vec = GAME.bests[ 1-(i%2) ][ v.kmh>0 ];
                for (int j=0; j<HITS_BESTS; j++) {
                    if (kmh > vec[j]) {
                        for (int k=HITS_BESTS-1; k>j; k--) {
                            vec[k] = vec[k-1];
                        }
                        vec[j] = kmh;
                        break;
                    }
                }
            }

            GAME.time += v.dt;
        }
    }
    GAME.time *= 10;
}

