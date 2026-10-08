/* Bluetooth function */
//----------------------------------
//convert bt address to text
//{0x00,0x1d,0xa5,0x00,0x12,0x92} -> 00:1d:a5:00:12:92
String ByteArraytoString(esp_bd_addr_t bt_address) {
  String txt = "";
  String nib = "";
  for (uint8_t i=0;i<ESP_BD_ADDR_LEN-1;i++) {//0-4
    nib = String(bt_address[i],HEX);
    if (nib.length() < 2) nib = "0"+nib;
    txt = txt + nib+":";
  }//for
    
  nib = String(bt_address[ESP_BD_ADDR_LEN-1],HEX);//5
  if (nib.length() < 2) nib = "0"+nib;
  txt = txt + nib;
  return txt;
}
/*=======================*/

void scanBTdevice()
{
    digitalWrite(LED_BLUE_PIN, LOW);

    Serial.println("\nScanning for Bluetooth devices...");

    btDeviceCount = 0;

    for (uint8_t i = 0; i < 8; i++) {
        deviceName[i] = "";
        deviceAddr[i] = "";
    }

    if (BTSerial.discoverAsync([](BTAdvertisedDevice* pDevice) {

        if (btDeviceCount >= 8)
            return;

        String addr = pDevice->getAddress().toString().c_str();

        // Ignore duplicates
        for (uint8_t i = 0; i < btDeviceCount; i++) {
            if (deviceAddr[i] == addr)
                return;
        }

        deviceName[btDeviceCount] = pDevice->getName().c_str();
        deviceAddr[btDeviceCount] = addr;

        Serial.printf(
            "Found device %u: %s [%s]\n",
            btDeviceCount + 1,
            deviceName[btDeviceCount].length()
                ? deviceName[btDeviceCount].c_str()
                : "<unknown>",
            deviceAddr[btDeviceCount].c_str()
        );

        btDeviceCount++;

    })) {

        delay(BT_DISCOVER_TIME);
        BTSerial.discoverAsyncStop();

    } else {
        Serial.println("Bluetooth discovery failed.");
    }

    digitalWrite(LED_BLUE_PIN, HIGH);

    Serial.printf(
        "Bluetooth scan complete: %u device(s)\n",
        btDeviceCount
    );
}

bool connectBTdevice(uint8_t selected)
{
    if (selected >= btDeviceCount)
        return false;

    String str = deviceAddr[selected];

    Serial.printf(
        "\nSelected: %s [%s]\n",
        deviceName[selected].length()
            ? deviceName[selected].c_str()
            : "<unknown>",
        deviceAddr[selected].c_str()
    );

    // Convert xx:xx:xx:xx:xx:xx to esp_bd_addr_t
    for (uint8_t i = 0; i < ESP_BD_ADDR_LEN; i++) {

        int separator = str.indexOf(':');
        String byteString;

        if (separator == -1) {
            byteString = str;
        } else {
            byteString = str.substring(0, separator);
            str = str.substring(separator + 1);
        }

        client_addr[i] =
            strtol(byteString.c_str(), nullptr, 16);
    }

    Serial.printf(
        "Connecting to %s...\n",
        deviceAddr[selected].c_str()
    );

    BTSerial.connect(
        client_addr,
        0,
        sec_mask,
        role
    );

    uint8_t tries = 0;

    while (!BTSerial.connected(1000) && tries < 10) {
        Serial.print(".");
        tries++;
    }

    Serial.println();

    if (!BTSerial.connected()) {
        Serial.println("Connection failed.");
        BTSerial.disconnect();
        foundOBD2 = false;
        return false;
    }

    Serial.println("Connected Successfully!");

    // Save only successful SPP devices
    pref.putBytes(
        "recent_client",
        client_addr,
        sizeof(client_addr)
    );

    memcpy(
        recent_client_addr,
        client_addr,
        sizeof(client_addr)
    );

    Serial.printf(
        "Saved adapter MAC: %s\n",
        ByteArraytoString(client_addr).c_str()
    );

    foundOBD2 = true;
    prompt = true;

    digitalWrite(LED_GREEN_PIN, LOW);

    return true;
}

//---------------------------
//connect to recent obdII for fast connection, skip scanning
void connectLastOBDII() {
  digitalWrite(LED_BLUE_PIN,LOW);//blue led on  
  String txt = "Connecting to " + client_name +" - " + ByteArraytoString(recent_client_addr);
  Terminal(txt,0,48,320,191);
  Serial.println(txt);
  BTSerial.connect(recent_client_addr, 0, sec_mask, role);//connect to OBDII adaptor
  uint8_t try_count = 0;
  bool blink = false;
  while(!BTSerial.connected(1000) && (try_count <3)) {
    Serial.print(F("."));
    blink =! blink;
    digitalWrite(LED_BLUE_PIN,blink);//blue led offset
    try_count++;
  }
  if (try_count == 3) {//cannot connect
    Terminal("OBDII Adaptor not found!",0,48,320,191);
    Serial.println(F("OBDII Adaptor not found!"));
    BTSerial.disconnect();
    foundOBD2 = false;
  } else {  
    Terminal("Connected Successfully!",0,48,320,191);  
    Serial.println(F("Connected Successfully!"));
    prompt = true;
    digitalWrite(LED_GREEN_PIN, LOW);//green led 
    foundOBD2 = true;
  }
 
}//connectLasbtOBDII