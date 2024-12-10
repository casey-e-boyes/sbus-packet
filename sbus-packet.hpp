/**
 * @file    sbus-packet.hpp
 * @author  Casey Boyes (github.com/casey-e-boyes)
 * @brief   SBUS Packet Processing Library Written in C++ for (Primarily) Embedded Systems
 * @version 0.1
 * @date    10 Dec. 2024
 */
#ifndef __SBUS_PACKET_HPP__
#define __SBUS_PACKET_HPP__

#include <cstdint>
#include <variant>
#include <type_traits>


namespace SBUS {
    enum class ErrorStatus : uint8_t {
        NONE = 0,
        FAIL,
        INVALID_PACKET_FRAMING,
        CHANNEL_EXCEEDS_LIMITS
    };

    class Port {
        public:
            uint16_t channel[16];
            bool channel17;
            bool channel18;
            bool lost_frame;
            bool fail_safe;

            [[nodiscard]] ErrorStatus encode() noexcept;
            [[nodiscard]] ErrorStatus decode() noexcept;
        private:
            volatile uint8_t buffer[25];
            const uint16_t channel_min;
            const uint16_t channel_max;

            inline ErrorStatus check_packet_framing();
            inline ErrorStatus check_channel_limits(const uint16_t channel);
            ErrorStatus check_packet();

            Port(const uint16_t channel_min, const uint16_t channel_max) :
                channel_min(channel_min),
                channel_max(channel_max) {

            }
    };
}

#endif /* __SBUS_PACKET_HPP__ */
