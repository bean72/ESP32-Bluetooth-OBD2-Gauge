#pragma once

#define CALIBRATION_FILE "/TouchCalData1"

void touch_calibrate()
{
    uint16_t calData[5];
    bool calDataOK = false;

    if (!SPIFFS.begin()) {
        Serial.println(F("Formatting file system"));
        SPIFFS.format();
        SPIFFS.begin();
    }

    if (SPIFFS.exists(CALIBRATION_FILE)) {
        File f = SPIFFS.open(CALIBRATION_FILE, "r");

        if (f) {
            if (f.readBytes((char *)calData, sizeof(calData)) == sizeof(calData)) {
                calDataOK = true;
            }
            f.close();
        }
    }

    if (calDataOK && digitalRead(SELECTOR_PIN) == HIGH) {
        tft.setTouch(calData);
        Serial.println(F("Touch calibration loaded"));
        return;
    }

    Serial.println(F("Touch screen calibration..."));

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextSize(1);

    tft.drawCentreString(
        "Touch screen calibration",
        tft.width() / 2,
        tft.height() / 2 - 10,
        2
    );

    tft.setCursor(20, 0);
    tft.println(F("Touch corners as indicated"));

Serial.println(F(">>> ENTERING calibrateTouch - DO NOT TOUCH SCREEN <<<"));

unsigned long calStart = millis();

tft.calibrateTouch(
    calData,
    TFT_MAGENTA,
    TFT_BLACK,
    15
);

unsigned long calTime = millis() - calStart;

Serial.printf(
    ">>> calibrateTouch RETURNED after %lu ms <<<\n",
    calTime
);

tft.setTouch(calData);

    Serial.println(F("Calibration complete!"));
    Serial.printf(
        "Calibration: {%u, %u, %u, %u, %u}\n",
        calData[0],
        calData[1],
        calData[2],
        calData[3],
        calData[4]
    );

    SPIFFS.remove(CALIBRATION_FILE);

    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
        f.write((const unsigned char *)calData, sizeof(calData));
        f.close();
    }
}

bool getTouch(uint16_t *x, uint16_t *y)
{
    bool touched = tft.getTouch(x, y);

    if (touched) {
        Serial.printf(
            "TOUCH x=%u y=%u millis=%lu\n",
            *x,
            *y,
            millis()
        );
    }

    return touched;
}