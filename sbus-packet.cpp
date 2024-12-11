/**
 * @file    sbus-packet.cpp
 * @author  Casey Boyes (github.com/casey-e-boyes)
 * @brief   SBUS Packet Processing Library Written in C++ for (Primarily) Embedded Systems
 * @version 0.1
 * @date    10 Dec. 2024
 */
#include "sbus-packet.hpp"

namespace SBUS {
    inline ErrorStatus Port::check_packet_framing() {
        if ((0x0F == buffer[0]) && (0x00 == buffer[24])) {
            return ErrorStatus::NONE;
        } else {
            return ErrorStatus::INVALID_PACKET_FRAMING;
        }
    }

    inline ErrorStatus Port::check_channel_limits(const uint16_t channel) {
        if ((channel_min <= channel) && (channel <= channel_max)) {
            return ErrorStatus::NONE;
        } else {
            return ErrorStatus::CHANNEL_EXCEEDS_LIMITS;
        }
    }

    ErrorStatus Port::check_packet() {
        ErrorStatus status = ErrorStatus::NONE;
        for (uint32_t i = 0; i < 16U; i++) {
            status = check_channel_limits(channel[i]);
            if (ErrorStatus::NONE != status) {
                return status;
            }
        }
        return status;
    }

    ErrorStatus Port::encode() noexcept {
        buffer[0]  = (uint8_t)0x0FU;

        buffer[1]  = (uint8_t)(channel[0] & 0xFF);

        buffer[2]  = (uint8_t)(((channel[0]  >> 8)  & 0x07) |
                               ((channel[1]  << 3)  & 0xF8));

        buffer[3]  = (uint8_t)(((channel[1]  >> 5)  & 0x3F) |
                               ((channel[2]  << 6)  & 0xC0));

        buffer[4]  = (uint8_t)(((channel[2]  >> 2)  & 0xFF));

        buffer[5]  = (uint8_t)(((channel[2]  >> 10) & 0x01) |
                               ((channel[3]  << 1)  & 0xFE));

        buffer[6]  = (uint8_t)(((channel[3]  >> 7)  & 0x0F) | 
                               ((channel[4]  << 4)  & 0xF0));

        buffer[7]  = (uint8_t)(((channel[4]  >> 4)  & 0x7F) |
                               ((channel[5]  << 7)  & 0x80));

        buffer[8]  = (uint8_t)(((channel[5]  >> 1)  & 0xFF));

        buffer[9]  = (uint8_t)(((channel[5]  >> 9)  & 0x03) |
                               ((channel[6]  << 2)  & 0xFC));

        buffer[10] = (uint8_t)(((channel[6]  >> 6)  & 0x1F) |
                               ((channel[7]  << 5)  & 0xE0));

        buffer[11] = (uint8_t)(((channel[7]  >> 3)  & 0xFF));
        
        buffer[12] = (uint8_t)(channel[8] & 0xFF);

        buffer[13] = (uint8_t)(((channel[8]  >> 8)  & 0x07) |
                               ((channel[9]  << 3)  & 0xF8));

        buffer[14] = (uint8_t)(((channel[9]  >> 5)  & 0x3F) |
                               ((channel[10] << 6)  & 0xC0));

        buffer[15] = (uint8_t)(((channel[10] >> 2)  & 0xFF));

        buffer[16] = (uint8_t)(((channel[10] >> 10) & 0x01) |
                               ((channel[11] << 1)  & 0xFE));

        buffer[17] = (uint8_t)(((channel[11] >> 7)  & 0x0F) |
                               ((channel[12] << 4)  & 0xF0));

        buffer[18] = (uint8_t)(((channel[12] >> 4)  & 0x7F) |
                               ((channel[13] << 7)  & 0x80));

        buffer[19] = (uint8_t)(((channel[13] >> 1)  & 0xFF));

        buffer[20] = (uint8_t)(((channel[13] >> 9)  & 0x03) |
                               ((channel[14] << 2)  & 0xFC));

        buffer[21] = (uint8_t)(((channel[14] >> 6)  & 0x1F) |
                               ((channel[15] << 5)  & 0xE0));

        buffer[22] = (uint8_t)(((channel[15] >> 3)  & 0xFF));

        buffer[23] = 0;

        if (channel17)  {buffer[23] |= (0x01U);}
        if (channel18)  {buffer[23] |= (0x02U);}
        if (lost_frame) {buffer[23] |= (0x04U);}
        if (fail_safe)  {buffer[23] |= (0x08U);}

        buffer[24] = (uint8_t)0x00U;

        return ErrorStatus::NONE;
    }

    ErrorStatus Port::decode() noexcept {
        // ErrorStatus status = check_packet_framing();
        // if (ErrorStatus::NONE != status) {
        //     return status;
        // }

        //? Start at index 0 (Aligned(4)) and use 32bit accesses (0bit or 16bit offset to minimise penalty)
        channel[0]  = (uint16_t)((*(uint32_t*)(&buffer[0]))  >>  8) & (0x7FFU);
        channel[1]  = (uint16_t)((*(uint32_t*)(&buffer[0]))  >> 19) & (0x7FFU);
        channel[2]  = (uint16_t)((*(uint32_t*)(&buffer[2]))  >> 14) & (0x7FFU);
        channel[3]  = (uint16_t)((*(uint32_t*)(&buffer[4]))  >>  9) & (0x7FFU);
        channel[4]  = (uint16_t)((*(uint32_t*)(&buffer[4]))  >> 20) & (0x7FFU);
        channel[5]  = (uint16_t)((*(uint32_t*)(&buffer[6]))  >> 15) & (0x7FFU);
        channel[6]  = (uint16_t)((*(uint32_t*)(&buffer[8]))  >> 10) & (0x7FFU);
        channel[7]  = (uint16_t)((*(uint32_t*)(&buffer[8]))  >> 21) & (0x7FFU);
        channel[8]  = (uint16_t)((*(uint32_t*)(&buffer[10])) >> 16) & (0x7FFU);
        channel[9]  = (uint16_t)((*(uint32_t*)(&buffer[12])) >> 11) & (0x7FFU);
        channel[10] = (uint16_t)((*(uint32_t*)(&buffer[14])) >>  6) & (0x7FFU);
        channel[11] = (uint16_t)((*(uint32_t*)(&buffer[14])) >> 17) & (0x7FFU);
        channel[12] = (uint16_t)((*(uint32_t*)(&buffer[16])) >> 12) & (0x7FFU);
        channel[13] = (uint16_t)((*(uint32_t*)(&buffer[18])) >>  7) & (0x7FFU);
        channel[14] = (uint16_t)((*(uint32_t*)(&buffer[18])) >> 18) & (0x7FFU);
        channel[15] = (uint16_t)((*(uint32_t*)(&buffer[20])) >> 13) & (0x7FFU);

        channel17  = (bool)(buffer[23] & (0x01U));
        channel18  = (bool)(buffer[23] & (0x02U));
        lost_frame = (bool)(buffer[23] & (0x04U));
        fail_safe  = (bool)(buffer[23] & (0x08U));

        // return check_packet();
        return ErrorStatus::NONE;
    }
}