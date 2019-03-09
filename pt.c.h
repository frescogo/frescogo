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
                s8 v = G.bests[i][j][k];
                G.ps[i] += v*v*4;
            }
        }
    }
}

void PT_All (void) {
    G.ps[0] = 0;
    G.ps[1] = 0;
    G.time  = 0;
    G.hits  = 0;
    G.servs = 0;

    memset(G.bests, 0, 2*2*HITS_BESTS*sizeof(s8));

    u32 pace = 0;

    for (int i=0 ; i<S.hit ; i++) {
    //for (int i=0 ; i<600 ; i++) {
        s8  dt  = S.dts[i];
        s8  kmh = G.kmhs[i];
        u16 pt  = ((u16)kmh)*((u16)kmh);

        if (dt == HIT_SERV) {
            G.servs++;
        }

        if (dt!=HIT_NONE && dt!=HIT_SERV) {
            if (i==S.hit-1 || S.dts[i+1]==HIT_NONE || S.dts[i+1]==HIT_SERV) {
                // ignore last hit
            }
            else
            {
                G.hits++;
                G.ps[1-(i%2)] += pt;
                pace += kmh;

                // bests
                s8* vec = G.bests[ 1-(i%2) ][ dt>0 ];
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

            G.time += dt;
        }
    }
    G.time *= 10;

    G.pace = pace/G.hits;

    PT_Bests_Apply();

    u32 avg   = (G.ps[0] + G.ps[1]) / 2;
    u32 total = min(avg, min(G.ps[0],G.ps[1])*11/10);
    int pct   = 100 - min(100, Falls()*3);
    G.total = total * pct/10000;
}

