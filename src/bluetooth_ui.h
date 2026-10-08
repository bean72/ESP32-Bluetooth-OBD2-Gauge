#pragma once

#define BT_ROWS_PER_PAGE 3

void drawBluetoothSelector(uint8_t page)
{
    tft.fillScreen(TFT_BLACK);

    // Header
    tft.fillRect(0, 0, 480, 55, TFT_DARKGREY);

    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString("Bluetooth Devices", 15, 8, 4);

    tft.setTextColor(TFT_LIGHTGREY, TFT_DARKGREY);
    tft.drawString("Select your OBD-II adapter", 15, 35, 2);

    // Rescan button
    tft.fillRoundRect(375, 8, 90, 38, 6, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawCentreString("RESCAN", 420, 19, 2);

    uint8_t first = page * BT_ROWS_PER_PAGE;

    for (uint8_t row = 0; row < BT_ROWS_PER_PAGE; row++) {

        uint8_t index = first + row;

        if (index >= btDeviceCount)
            break;

        int y = 60 + (row * 70);

        // Device row
        tft.drawRoundRect(
            10,
            y,
            460,
            64,
            5,
            TFT_DARKGREY
        );

        String name =
            deviceName[index].length()
                ? deviceName[index]
                : "Unknown device";

        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(
            name,
            22,
            y + 8,
            4
        );

        tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
        tft.drawString(
            deviceAddr[index],
            22,
            y + 38,
            2
        );

        // Arrow
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.drawRightString(
            ">",
            450,
            y + 20,
            4
        );
    }

    // Footer
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);

    String status =
        String(btDeviceCount) +
        " device" +
        (btDeviceCount == 1 ? "" : "s") +
        " found";

    tft.drawRightString(
        status,
        465,
        285,
        2
    );

    // Paging controls
    if (page > 0) {
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.drawString("< PREV", 15, 285, 2);
    }

    if ((page + 1) * BT_ROWS_PER_PAGE < btDeviceCount) {
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.drawCentreString("NEXT >", 240, 285, 2);
    }
}

int bluetoothSelector()
{
    uint8_t page = 0;

    drawBluetoothSelector(page);

    while (true) {

        uint16_t x, y;

        if (!getTouch(&x, &y)) {
            delay(20);
            continue;
        }

        Serial.printf("BT UI touch: x=%u y=%u\n", x, y);

        // RESCAN button
        if (x >= 375 && x <= 465 &&
            y >= 8   && y <= 46) {

            Serial.println("Rescanning Bluetooth devices...");

            tft.fillScreen(TFT_BLACK);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawCentreString(
                "Scanning for Bluetooth devices...",
                240,
                145,
                2
            );

            scanBTdevice();

            page = 0;
            drawBluetoothSelector(page);

            // Wait for finger release
            while (getTouch(&x, &y)) {
                delay(20);
            }

            continue;
        }

        // Device rows
        if (x >= 10 && x <= 470 &&
            y >= 60 && y < 270) {

            uint8_t row = (y - 60) / 70;

            if (row < BT_ROWS_PER_PAGE) {

                uint8_t index =
                    (page * BT_ROWS_PER_PAGE) + row;

                if (index < btDeviceCount) {

                    Serial.printf(
                        "Selected Bluetooth device %u\n",
                        index
                    );

                    // Wait for release
                    while (getTouch(&x, &y)) {
                        delay(20);
                    }

                    return index;
                }
            }
        }

        // PREV
        if (page > 0 &&
            x <= 100 &&
            y >= 275) {

            page--;

            drawBluetoothSelector(page);

            while (getTouch(&x, &y)) {
                delay(20);
            }

            continue;
        }

        // NEXT
        if (((page + 1) * BT_ROWS_PER_PAGE) < btDeviceCount &&
            x >= 180 && x <= 300 &&
            y >= 275) {

            page++;

            drawBluetoothSelector(page);

            while (getTouch(&x, &y)) {
                delay(20);
            }

            continue;
        }

        // Ignore touch until released
        while (getTouch(&x, &y)) {
            delay(20);
        }
    }
}