# 1 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino"
# 1 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino"
/*******************************************************************************

 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman

 * Copyright (c) 2018 Terry Moore, MCCI

 *

 * Permission is hereby granted, free of charge, to anyone

 * obtaining a copy of this document and accompanying files,

 * to do whatever they want with them without any restriction,

 * including, but not limited to, copying, modification and redistribution.

 * NO WARRANTY OF ANY KIND IS PROVIDED.

 *

 * This example sends a valid LoRaWAN packet with payload "Hello,

 * world!", using frequency and encryption settings matching those of

 * the The Things Network. It's pre-configured for the Adafruit

 * Feather M0 LoRa.

 *

 * This uses OTAA (Over-the-air activation), where where a DevEUI and

 * application key is configured, which are used in an over-the-air

 * activation procedure where a DevAddr and session keys are

 * assigned/generated for use with all further communication.

 *

 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in

 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably

 * violated by this sketch when left running for longer)!



 * To use this sketch, first register your application and device with

 * the things network, to set or generate an AppEUI, DevEUI and AppKey.

 * Multiple devices can use the same AppEUI, but each device has its own

 * DevEUI and AppKey.

 *

 * Do not forget to define the radio type correctly in config.h.

 *

 *******************************************************************************/
# 34 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino"
# 35 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino" 2
# 36 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino" 2
# 37 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino" 2
//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//







// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t APPEUI[8]= { 0x20, 0xFB, 0x00, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };//0xEC, 0xEE, 0x00, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getArtEui (u1_t* buf) { memcpy((buf), (APPEUI), (8));}

// This should also be in little endian format, see above.
static const u1_t DEVEUI[8]= { 0x93, 0x2B, 0xF5, 0x93, 0x9B, 0xAF, 0x60, 0x00};//0x53, 0x01, 0x00, 0x00, 0x01, 0xCC, 0x02, 0x00 };
void os_getDevEui (u1_t* buf) { memcpy((buf), (DEVEUI), (8));}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t APPKEY[16] = { 0x21, 0xC8, 0x39, 0x66, 0x22, 0xEE, 0xF3, 0xDA, 0xAE, 0x2A, 0x92, 0x89, 0x82, 0x51, 0xD8, 0x29};//0xA2, 0x0E, 0x07, 0x34, 0x6E, 0x98, 0x71, 0xE0, 0x6C, 0x71, 0x98, 0x62, 0x53, 0x1A, 0xD4, 0xA3 };
void os_getDevKey (u1_t* buf) { memcpy((buf), (APPKEY), (16));}

uint8_t mydata[100];


static osjob_t sendjob;

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 6;

// Pin mapping

// Pin mapping for Adafruit Feather M0 LoRa, etc.
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8, // LBT cal for the Adafruit Feather M0 LoRa, in dB
    .spi_freq = 8000000,
};
# 107 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino"
void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_SCAN_TIMEOUT"))));
            break;
        case EV_BEACON_FOUND:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_BEACON_FOUND"))));
            break;
        case EV_BEACON_MISSED:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_BEACON_MISSED"))));
            break;
        case EV_BEACON_TRACKED:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_BEACON_TRACKED"))));
            break;
        case EV_JOINING:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_JOINING"))));
            break;
        case EV_JOINED:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_JOINED"))));
            {
              u4_t netid = 0;
              devaddr_t devaddr = 0;
              u1_t nwkKey[16];
              u1_t artKey[16];
              LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
              Serial.print("netid: ");
              Serial.println(netid, 10);
              Serial.print("devaddr: ");
              Serial.println(devaddr, 16);
              Serial.print("artKey: ");
              for (int i=0; i<sizeof(artKey); ++i) {
                if (i != 0)
                  Serial.print("-");
                Serial.print(artKey[i], 16);
              }
              Serial.println("");
              Serial.print("nwkKey: ");
              for (int i=0; i<sizeof(nwkKey); ++i) {
                      if (i != 0)
                              Serial.print("-");
                      Serial.print(nwkKey[i], 16);
              }
              Serial.println("");
            }
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
      // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        /*

        || This event is defined but not used in the code. No

        || point in wasting codespace on it.

        ||

        || case EV_RFU1:

        ||     Serial.println(F("EV_RFU1"));

        ||     break;

        */
# 166 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino"
        case EV_JOIN_FAILED:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_JOIN_FAILED"))));
            break;
        case EV_REJOIN_FAILED:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_REJOIN_FAILED"))));
            break;
            break;
        case EV_TXCOMPLETE:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_TXCOMPLETE (includes waiting for RX windows)"))));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println((reinterpret_cast<const __FlashStringHelper *>(("Received ack"))));
            if (LMIC.dataLen) {
              Serial.println((reinterpret_cast<const __FlashStringHelper *>(("Received "))));
              Serial.println(LMIC.dataLen);
              Serial.println((reinterpret_cast<const __FlashStringHelper *>((" bytes of payload"))));
            }
            // Schedule next transmission
            os_setTimedCallback(&sendjob, os_getTime()+((ostime_t)( (int64_t)(TX_INTERVAL) * (1000000 / (1 << 4)))), do_send);
            break;
        case EV_LOST_TSYNC:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_LOST_TSYNC"))));
            break;
        case EV_RESET:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_RESET"))));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_RXCOMPLETE"))));
            break;
        case EV_LINK_DEAD:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_LINK_DEAD"))));
            break;
        case EV_LINK_ALIVE:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_LINK_ALIVE"))));
            break;
        /*

        || This event is defined but not used in the code. No

        || point in wasting codespace on it.

        ||

        || case EV_SCAN_FOUND:

        ||    Serial.println(F("EV_SCAN_FOUND"));

        ||    break;

        */
# 209 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino"
        case EV_TXSTART:
            Serial.println((reinterpret_cast<const __FlashStringHelper *>(("EV_TXSTART"))));
            break;
        default:
            Serial.print((reinterpret_cast<const __FlashStringHelper *>(("Unknown event: "))));
            Serial.println((unsigned) ev);
            break;
    }
}

void do_send(osjob_t* j){
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println((reinterpret_cast<const __FlashStringHelper *>(("OP_TXRXPEND, not sending"))));
    } else {
        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
        Serial.println((reinterpret_cast<const __FlashStringHelper *>(("Packet queued"))));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void setup() {
    delay(5000);
    while (! Serial)
        ;
    Serial.begin(9600);
    Serial.println((reinterpret_cast<const __FlashStringHelper *>(("Starting"))));
# 245 "c:\\Users\\xiaoy\\Documents\\GitHub\\iotHardware\\ttn-otaa\\ttn-otaa.ino"
    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7,14);
    LMIC_selectSubBand(1);
    for(int i = 0;i<100; i++){
        mydata[i]=i;
    }
    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
