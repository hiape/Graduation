#pragma once

#define G711_PACKET_SIZE 160    /**< G711 packet size is 160 samples, 20ms data at 8kHz sampling */
#define G722_PACKET_SIZE 320    /**< G722 packet size is 320 sapmles, 20ms data at 16kHz sampling */
#define PACKET_DURATION 20    /**< Default packet duration is 20ms */
#define G711a_RTP_PAYLOAD_TYPE 0x8    //*< G711a rtp payload type */
#define G711u_RTP_PAYLOAD_TYPE 0x0    //*< G711u rtp payload type */
#define G722_RTP_PAYLOAD_TYPE 0x9    //*< G722 rtp payload type */

typedef unsigned int  uint;
typedef unsigned char uchar;





