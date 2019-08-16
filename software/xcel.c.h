void CEL_Restart (void) {
    Serial.print(F("= FrescoGO! (versao "));
    Serial.print(MAJOR);
    Serial.print(".");
    Serial.print(MINOR);
    Serial.print(".");
    Serial.print(REVISION);
    Serial.println(F(") ="));
    Serial_Score();
}

void CEL_Hit (u32 kmh, bool is_back) {
    Serial.print(F("> "));
    Serial.print(kmh);
    if (is_back) {
        Serial.print(F(" !"));
    }
    Serial.println();
}

void CEL_Service (void) {
    Serial.println(F("> saque"));
}

void CEL_Fall (void) {
    Serial.println(F("QUEDA"));
    Serial_Score();
}

void CEL_End (void) {
    Serial.println(F("= FIM ="));
    Serial_Score();
}

void CEL_Nop (void) {
}
