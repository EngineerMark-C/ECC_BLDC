#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H
#include "zf_common_headfile.h"
#include "hmac_sha256.h"
#include "base64.h"



void url_encode(const char* str, char* encoded);

bool websocket_client_connect(const char* url);

bool websocket_client_send(const uint8_t* data, uint32_t len);

bool websocket_client_receive(uint8_t* buffer);

void websocket_client_close();

uint32_t websocket_create_frame(uint8_t* frame, const uint8_t* payload, uint64_t payload_len, uint8_t type, bool mask);

#endif // WEBSOCKET_CLIENT_H
