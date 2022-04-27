#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FloodingMesh.h>
#include <TypeConversionFunctions.h>

#include "data.hpp"

bool received_callback(String &msg, FloodingMesh &meshInstance);

// A custom encryption key is required when using encrypted ESP-NOW transmissions. There is always a default Kok set, but it can be replaced if desired.
// All ESP-NOW keys below must match in an encrypted connection pair for encrypted communication to be possible.
// Note that it is also possible to use Strings as key seeds instead of arrays.
uint8_t espnow_encrypted_connection_key[16] = { 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44,  // This is the key for encrypting transmissions of encrypted connections.
																						 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x32, 0x11 };
uint8_t espnow_hash_key[16] = { 0xEF, 0x44, 0x33, 0x0C, 0x33, 0x44, 0xFE, 0x44,  // This is the secret key used for HMAC during encrypted connection requests.
															0x33, 0x44, 0x33, 0xB0, 0x33, 0x44, 0x32, 0xAD };

FloodingMesh mesh = FloodingMesh(received_callback, FPSTR(password), espnow_encrypted_connection_key,
								espnow_hash_key, FPSTR(ssid), MeshTypeConversionFunctions::uint64ToString(ESP.getChipId()), true);;
char count[NODE_ID_SIZE + 1];
uint16_t message_counter = 0;
uint32_t prev_millis = 0;
uint32_t interval_ms = 1000;
String message_offset;
void setup() {
	WiFi.persistent(false);
	sprintf(chip_id, "%u", ESP.getChipId());

    //message_offset
    for (int i = 0; i < OFFSET_SIZE; i++)
        message_offset += String(':');

	mesh.begin();
	mesh.activateAP();
}

void loop() {
	floodingMeshDelay(1);
	
    uint32_t curr_millis = millis();
	if (curr_millis - prev_millis > interval_ms) {
        prev_millis = curr_millis;
        
        sprintf(count, "%03x", message_counter);
		mesh.broadcast(String(ESP.getChipId()) + String('\t') + String(message_counter) + message_offset);
        message_counter++;
	}
}

bool received_callback(String &msg, FloodingMesh &meshInstance) {
    return true;
}
