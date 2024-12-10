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
        ErrorStatus status = check_packet_framing();
        if (ErrorStatus::NONE != status) {
            return status;
        }

        channel[0]  = (uint16_t)(((buffer[1]  & 0xFF) >> 0) |
                                 ((buffer[2]  & 0x07) << 8));
                                    
        channel[1]  = (uint16_t)(((buffer[2]  & 0xF8) >> 3) |
                                 ((buffer[3]  & 0x3F) << 5));

        channel[2]  = (uint16_t)(((buffer[3]  & 0xC0) >> 6) |
                                 ((buffer[4]  & 0xFF) << 2) |
                                 ((buffer[5]  & 0x01) << 10));

        channel[3]  = (uint16_t)(((buffer[5]  & 0xFE) >> 1) |
                                 ((buffer[6]  & 0x0F) << 7));

        channel[4]  = (uint16_t)(((buffer[6]  & 0xF0) >> 4) |
                                 ((buffer[7]  & 0x7F) << 4));

        channel[5]  = (uint16_t)(((buffer[7]  & 0x80) >> 7) |
                                 ((buffer[8]  & 0xFF) << 1) |
                                 ((buffer[9]  & 0x03) << 9));

        channel[6]  = (uint16_t)(((buffer[9]  & 0xFC) >> 2) |
                                 ((buffer[10] & 0x1F) << 6));

        channel[7]  = (uint16_t)(((buffer[10] & 0xE0) >> 5) |
                                 ((buffer[11] & 0xFF) << 3));
        
        channel[9]  = (uint16_t)(((buffer[13] & 0xF8) >> 3) |
                                 ((buffer[14] & 0x3F) << 5));

        channel[8]  = (uint16_t)(((buffer[12] & 0xFF) >> 0) |
                                 ((buffer[13] & 0x07) << 8));

        channel[10] = (uint16_t)(((buffer[14] & 0xC0) >> 6) |
                                 ((buffer[15] & 0xFF) << 2) |
                                 ((buffer[16] & 0x01) << 10));

        channel[11] = (uint16_t)(((buffer[16] & 0xFE) >> 1) |
                                 ((buffer[17] & 0x0F) << 7));

        channel[12] = (uint16_t)(((buffer[17] & 0xF0) >> 4) |
                                 ((buffer[18] & 0x7F) << 4));

        channel[13] = (uint16_t)(((buffer[18] & 0x80) >> 7) |
                                 ((buffer[19] & 0xFF) << 1) |
                                 ((buffer[20] & 0x03) << 9));

        channel[14] = (uint16_t)(((buffer[20] & 0xFC) >> 2) |
                                 ((buffer[21] & 0x1F) << 6));

        channel[15] = (uint16_t)(((buffer[21] & 0xE0) >> 5) |
                                 ((buffer[22] & 0xFF) << 3));

        channel17  = (bool)(buffer[23] & (0x01U));
        channel18  = (bool)(buffer[23] & (0x02U));
        lost_frame = (bool)(buffer[23] & (0x04U));
        fail_safe  = (bool)(buffer[23] & (0x08U));

        return check_packet();
    }
}