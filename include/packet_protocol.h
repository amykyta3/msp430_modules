#ifndef PACKET_PROTOCOL_H
#define PACKET_PROTOCOL_H

#include <stdint.h>

#define PKT_MAX_PAYLOAD_SIZE    128


typedef struct{
    uint8_t pkt_type;
    uint8_t payload_len_H;
    uint8_t payload_len_L;
}PKT_HEADER_t;

// Packet Types:
#define PKT_DATABLOCK    0x02 
    // Data following this header is data of length payload_len
    // Receiver of the datablock must respond with an ACK or CANCEL

#define PKT_END            0x04 // Marks end of transmission of DATABLOCKS
    // Receiver of this packet must respond with an ACK or CANCEL

#define PKT_ACK            0x06 // ACK response to packet. Ready for next packet
#define PKT_CANCEL        0x18 // Cancel a transmission
    // Receiver of this packet must respond with ACK



#define PKT_RESERVED_0D    0x0D // Do not use
#define PKT_RESERVED_0A    0x0A // Do not use


/* A transmission of packets would look like this:
* B#    Contents
* ------------------------------
* H0    PKT_DATABLOCK
* H1    0x00
* H2    0x05    (payload is 5 bytes)
* P0    Payload data ...
* P1    ...
* P2    ...
* P3    ...
* P4    ...
* [waits for ACK response]
* H0    PKT_DATABLOCK
* H1    0x00
* H2    0x06    (payload is 7 bytes)
* P0    Payload data ...
* P1    ...
* P2    ...
* P3    ...
* P4    ...
* P5    ...
* P6    ...
* [waits for ACK response]
* H0    PKT_END
* H1    -- (Dont care)
* H2    --
* [waits for ACK response]
*/



#endif
