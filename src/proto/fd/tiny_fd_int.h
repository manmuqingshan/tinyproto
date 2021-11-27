/*
    Copyright 2019-2021 (C) Alexey Dynda

    This file is part of Tiny Protocol Library.

    Protocol Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Protocol Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#define TINY_FD_U_QUEUE_MAX_SIZE 4

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "proto/hdlc/low_level/hdlc.h"
#include "proto/hdlc/low_level/hdlc_int.h"
#include "hal/tiny_types.h"
#include "tiny_fd_frames_int.h"

#define FD_MIN_BUF_SIZE(mtu, window)                                                                                   \
    (sizeof(tiny_fd_data_t) + HDLC_MIN_BUF_SIZE(mtu + sizeof(tiny_frame_header_t), HDLC_CRC_16) +                     \
      (sizeof(tiny_i_frame_info_t *) + sizeof(tiny_i_frame_info_t) + mtu -                                             \
       sizeof(((tiny_i_frame_info_t *)0)->user_payload)) *                                                             \
          window + \
          ( sizeof(tiny_fd_frame_info_t) + sizeof(tiny_fd_frame_info_t *) ) * TINY_FD_U_QUEUE_MAX_SIZE)

#define FD_BUF_SIZE_EX(mtu, tx_window, crc, rx_window)                                                                      \
    (sizeof(tiny_fd_data_t) + HDLC_BUF_SIZE_EX(mtu + sizeof(tiny_frame_header_t), crc, rx_window) +           \
      (sizeof(tiny_i_frame_info_t *) + sizeof(tiny_i_frame_info_t) + mtu -                                             \
       sizeof(((tiny_i_frame_info_t *)0)->user_payload)) *                                                             \
          tx_window + \
       ( sizeof(tiny_fd_frame_info_t) + sizeof(tiny_fd_frame_info_t *) ) * TINY_FD_U_QUEUE_MAX_SIZE)

    typedef enum
    {
        TINY_FD_STATE_DISCONNECTED,
        TINY_FD_STATE_CONNECTING,
        TINY_FD_STATE_CONNECTED_ABM,
        TINY_FD_STATE_DISCONNECTING,
    } tiny_fd_state_t;

    typedef struct
    {
        tiny_frame_header_t header;
        uint8_t data1;
        uint8_t data2;
    } tiny_fd_u_frame_t;

    typedef struct
    {
        /// state of hdlc protocol according to ISO & RFC
        tiny_fd_state_t state;
        uint8_t addr;        // Peer address

        uint8_t next_nr;     // frame waiting to receive
        uint8_t sent_nr;     // frame index last sent back
        uint8_t sent_reject; // If reject was already sent
        uint8_t next_ns;     // next frame to be sent
        uint8_t confirm_ns;  // next frame to be confirmed
        uint8_t last_ns;     // next free frame in cycle buffer

        uint32_t last_i_ts;  // last sent I-frame timestamp
        uint32_t last_ka_ts; // last keep alive timestamp
        uint8_t ka_confirmed;

        uint8_t retries; // Number of retries to perform before timeout takes place
        tiny_events_t events;

    } tiny_fd_peer_info_t;

    typedef struct
    {
        tiny_fd_queue_t i_queue;
        uint8_t window_size;


        tiny_mutex_t mutex;

    } tiny_frames_info_t;

    typedef struct tiny_fd_data_t
    {
        /// Callback to process received frames
        on_frame_cb_t on_frame_cb;
        /// Callback to get notification of sent frames
        on_frame_send_cb_t on_send_cb;
        /// hdlc information
        hdlc_ll_handle_t _hdlc;
        /// Timeout for operations with acknowledge
        uint16_t send_timeout;
        /// Timeout before retrying resend I-frames
        uint16_t retry_timeout;
        /// Timeout before sending keep alive HDLC frame (RR)
        uint16_t ka_timeout;
        /// Number of retries to perform before timeout takes place
        uint8_t retries;
        /// Information for frames being processed
        tiny_frames_info_t frames;
        /// Information on all peers stations
        tiny_fd_peer_info_t peers[1];
        /// Storage for all S- and U- service frames
        tiny_fd_queue_t s_queue;
        /// Global events for HDLC protocol
        tiny_events_t events;
        /// user specific data
        void *user_data;
    } tiny_fd_data_t;

#ifdef __cplusplus
}
#endif

#endif
