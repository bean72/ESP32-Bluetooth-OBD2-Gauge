#pragma once

#include <Arduino.h>

// Returns:
// 0-3  = graphical gauges
// 4-11 = numerical readouts
// -1   = outside dashboard

int dashboardHitTest(uint16_t x, uint16_t y)
{
    if (x >= DASH_WIDTH || y >= DASH_HEIGHT)
        return -1;

    int row = y / CELL_HEIGHT;

    // Left graphical gauge column
    if (x < GAUGE_WIDTH) {
        return row;
    }

    // Right numerical columns
    int column = (x - GAUGE_WIDTH) / NUMERIC_WIDTH;

    return 4 + row * 2 + column;
}


// --------------------------------------------------
// Temporary touchscreen test
// --------------------------------------------------

void dashboardTouchTest()
{
    uint16_t x, y;

    if (!getTouch(&x, &y))
        return;

    int selected = dashboardHitTest(x, y);

    if (selected >= 0) {

        Serial.printf(
            "Dashboard cell selected: %d\n",
            selected
        );

        // Visual feedback
        int cellX;
        int cellY;
        int cellWidth;

        if (selected < 4) {

            cellX = 0;
            cellY = selected * CELL_HEIGHT;
            cellWidth = GAUGE_WIDTH;

        } else {

            int numericIndex = selected - 4;

            cellX = GAUGE_WIDTH +
                (numericIndex % 2) * NUMERIC_WIDTH;

            cellY =
                (numericIndex / 2) * CELL_HEIGHT;

            cellWidth = NUMERIC_WIDTH;
        }

        tft.drawRect(
            cellX + 1,
            cellY + 1,
            cellWidth - 2,
            CELL_HEIGHT - 2,
            TFT_CYAN
        );

        delay(150);

        // Restore normal dashboard
        if (selected < 4) {
            drawDashboardGauge(
                selected,
                testGauges[selected]
            );
        } else {
            drawDashboardNumeric(
                selected - 4,
                testNumerics[selected - 4]
            );
        }
    }

    // Wait for release
    while (tft.getTouch(&x, &y)) {
        delay(10);
    }
}