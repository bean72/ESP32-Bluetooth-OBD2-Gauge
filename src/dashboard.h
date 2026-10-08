#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

// Native ST7796 landscape resolution
constexpr int DASH_WIDTH = 480;
constexpr int DASH_HEIGHT = 320;

// Layout dimensions
constexpr int GAUGE_WIDTH = 220;
constexpr int NUMERIC_WIDTH = 130;
constexpr int CELL_HEIGHT = 80;

constexpr uint8_t GAUGE_COUNT = 4;
constexpr uint8_t NUMERIC_COUNT = 8;

// Dashboard colours
constexpr uint16_t DASH_BG = TFT_BLACK;
constexpr uint16_t DASH_BORDER = 0x4208;
constexpr uint16_t DASH_LABEL = TFT_LIGHTGREY;
constexpr uint16_t DASH_VALUE = TFT_WHITE;
constexpr uint16_t DASH_BAR_BG = 0x2104;
constexpr uint16_t DASH_BAR = TFT_CYAN;

// Temporary test data
struct DashboardReading {
    const char* label;
    float value;
    const char* unit;
    float minimum;
    float maximum;
    bool valid;
};

// Four graphical gauges
DashboardReading testGauges[GAUGE_COUNT] = {
    {"ENGINE RPM", 794, "RPM", 0, 5000, true},
    {"ENGINE LOAD", 24, "%", 0, 100, true},
    {"COOLANT", 74, "C", 0, 120, true},
    {"BATTERY", 14.2, "V", 0, 16, true}
};

// Eight numerical readouts
DashboardReading testNumerics[NUMERIC_COUNT] = {
    {"ECT", 74, "C", 0, 120, true},
    {"PCM VOLT", 14.2, "V", 0, 16, true},
    {"OIL TEMP", 0, "C", 0, 120, false},
    {"TRANS TEMP", 0, "C", 0, 120, false},
    {"MAP", 4.5, "PSI", 0, 40, true},
    {"IAT", 21, "C", 0, 120, true},
    {"FUEL TRIM", 3.1, "%", -25, 25, true},
    {"THROTTLE", 18, "%", 0, 100, true}
};


// --------------------------------------------------
// Draw graphical gauge
// --------------------------------------------------

void drawDashboardGauge(
    uint8_t index,
    const DashboardReading& reading
) {
    if (index >= GAUGE_COUNT)
        return;

    int x = 0;
    int y = index * CELL_HEIGHT;

    // Background
    tft.fillRect(
        x,
        y,
        GAUGE_WIDTH,
        CELL_HEIGHT,
        DASH_BG
    );

    // Border
    tft.drawRect(
        x,
        y,
        GAUGE_WIDTH,
        CELL_HEIGHT,
        DASH_BORDER
    );

    // Label
    tft.setTextColor(DASH_LABEL, DASH_BG);
    tft.drawString(
        reading.label,
        x + 10,
        y + 6,
        2
    );

    // Value
    String valueText = "N/A";

    if (reading.valid) {
        valueText = String(
            reading.value,
            reading.value == (int)reading.value ? 0 : 1
        );

        valueText += " ";
        valueText += reading.unit;
    }

    tft.setTextColor(DASH_VALUE, DASH_BG);

    tft.drawRightString(
        valueText,
        x + GAUGE_WIDTH - 12,
        y + 28,
        4
    );

    // Bar background
    constexpr int barX = 10;
    constexpr int barWidth = GAUGE_WIDTH - 20;
    constexpr int barHeight = 12;

    int barY = y + 61;

    tft.fillRoundRect(
        barX,
        barY,
        barWidth,
        barHeight,
        3,
        DASH_BAR_BG
    );

    if (reading.valid &&
        reading.maximum > reading.minimum) {

        float fraction =
            (reading.value - reading.minimum) /
            (reading.maximum - reading.minimum);

        fraction = constrain(fraction, 0.0f, 1.0f);

        int fillWidth = (int)(fraction * barWidth);

        if (fillWidth > 0) {
            tft.fillRoundRect(
                barX,
                barY,
                fillWidth,
                barHeight,
                3,
                DASH_BAR
            );
        }
    }
}


// --------------------------------------------------
// Draw numerical readout
// --------------------------------------------------

void drawDashboardNumeric(
    uint8_t index,
    const DashboardReading& reading
) {
    if (index >= NUMERIC_COUNT)
        return;

    // Two columns, four rows
    int column = index % 2;
    int row = index / 2;

    int x = GAUGE_WIDTH + column * NUMERIC_WIDTH;
    int y = row * CELL_HEIGHT;

    tft.fillRect(
        x,
        y,
        NUMERIC_WIDTH,
        CELL_HEIGHT,
        DASH_BG
    );

    tft.drawRect(
        x,
        y,
        NUMERIC_WIDTH,
        CELL_HEIGHT,
        DASH_BORDER
    );

    // Label
    tft.setTextColor(DASH_LABEL, DASH_BG);

    tft.drawCentreString(
        reading.label,
        x + NUMERIC_WIDTH / 2,
        y + 8,
        2
    );

    // Value
    String valueText = "N/A";

    if (reading.valid) {
        valueText = String(
            reading.value,
            reading.value == (int)reading.value ? 0 : 1
        );
    }

    tft.setTextColor(
        reading.valid ? DASH_VALUE : TFT_DARKGREY,
        DASH_BG
    );

    tft.drawCentreString(
        valueText,
        x + NUMERIC_WIDTH / 2,
        y + 30,
        4
    );

    // Unit
    if (reading.valid) {
        tft.setTextColor(DASH_LABEL, DASH_BG);

        tft.drawCentreString(
            reading.unit,
            x + NUMERIC_WIDTH / 2,
            y + 61,
            2
        );
    }
}


// --------------------------------------------------
// Draw complete dashboard
// --------------------------------------------------

void drawDashboard()
{
    tft.fillScreen(DASH_BG);

    for (uint8_t i = 0; i < GAUGE_COUNT; i++) {
        drawDashboardGauge(i, testGauges[i]);
    }

    for (uint8_t i = 0; i < NUMERIC_COUNT; i++) {
        drawDashboardNumeric(i, testNumerics[i]);
    }
}