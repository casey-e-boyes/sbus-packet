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
        for (size_t i = 0; i < 16; i++) {
            status = check_channel_limits(channel[i]);
            if (ErrorStatus::NONE != status) {
                return status;
            }
        }
        return status;
    }

    ErrorStatus Port::encode() noexcept {
        *(uint32_t*)(&buffer[0])   = 0x0FU;
        *(uint32_t*)(&buffer[0])  |= ((uint32_t)(channel[0])  <<  8);
        *(uint32_t*)(&buffer[0])  |= ((uint32_t)(channel[1])  << 19);
        *(uint32_t*)(&buffer[2])  |= ((uint32_t)(channel[2])  << 14);
        *(uint32_t*)(&buffer[4])  |= ((uint32_t)(channel[3])  <<  9);
        *(uint32_t*)(&buffer[4])  |= ((uint32_t)(channel[4])  << 20);
        *(uint32_t*)(&buffer[6])  |= ((uint32_t)(channel[5])  << 15);
        *(uint32_t*)(&buffer[8])  |= ((uint32_t)(channel[6])  << 10);
        *(uint32_t*)(&buffer[8])  |= ((uint32_t)(channel[7])  << 21);
        *(uint32_t*)(&buffer[10]) |= ((uint32_t)(channel[8])  << 16);
        *(uint32_t*)(&buffer[12]) |= ((uint32_t)(channel[9])  << 11);
        *(uint32_t*)(&buffer[14]) |= ((uint32_t)(channel[10]) <<  6);
        *(uint32_t*)(&buffer[14]) |= ((uint32_t)(channel[11]) << 17);
        *(uint32_t*)(&buffer[16]) |= ((uint32_t)(channel[12]) << 12);
        *(uint32_t*)(&buffer[18]) |= ((uint32_t)(channel[13]) <<  7);
        *(uint32_t*)(&buffer[18]) |= ((uint32_t)(channel[14]) << 18);
        *(uint32_t*)(&buffer[20]) |= ((uint32_t)(channel[15]) << 13);
        *(uint32_t*)(&buffer[20]) &= 0x00FFFFFFU;

        if (channel17)  {*(uint32_t*)(&buffer[20]) |= (0x01000000U);}
        if (channel18)  {*(uint32_t*)(&buffer[20]) |= (0x02000000U);}
        if (lost_frame) {*(uint32_t*)(&buffer[20]) |= (0x04000000U);}
        if (fail_safe)  {*(uint32_t*)(&buffer[20]) |= (0x08000000U);}

        buffer[24] = (uint8_t)0x00U;

        return ErrorStatus::NONE;
    }

    ErrorStatus Port::decode() noexcept {
        ErrorStatus status = check_packet_framing();
        if (ErrorStatus::NONE != status) {
            return status;
        }

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

        channel17  = (bool)(*(uint32_t*)(&buffer[20]) & (0x01000000U));
        channel18  = (bool)(*(uint32_t*)(&buffer[20]) & (0x02000000U));
        lost_frame = (bool)(*(uint32_t*)(&buffer[20]) & (0x04000000U));
        fail_safe  = (bool)(*(uint32_t*)(&buffer[20]) & (0x08000000U));

        return check_packet();
    }
}