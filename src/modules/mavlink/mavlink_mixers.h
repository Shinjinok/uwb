/****************************************************************************
 *
 *   Copyright (c) 2012-2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file mavlink_mixers.h
 * Mixer manager definition.
 *
 * @author Matthew Coleman <uavflightdirector@gmail.com>
 */

#pragma once

#include <systemlib/param/param.h>

#include "mavlink_bridge_header.h"
#include "mavlink_stream.h"
#include <uORB/uORB.h>
#include <uORB/topics/mixer_data_request.h>


#if defined(MIXER_CONFIGURATION)
#define MAVLINK_MIXER_STATE_WAITING -1

class MavlinkMixersManager : public MavlinkStream
{
public:
	const char *get_name() const
	{
		return MavlinkMixersManager::get_name_static();
	}

	static const char *get_name_static()
	{
		return "MIXER_DATA";
	}

	uint16_t get_id()
	{
		return MAVLINK_MSG_ID_MIXER_DATA;
	}

	static MavlinkStream *new_instance(Mavlink *mavlink)
	{
		return new MavlinkMixersManager(mavlink);
	}

	unsigned get_size();

	unsigned get_size_avg();

	void handle_message(const mavlink_message_t *msg);

	bool const_rate() { return true; };

	/* do not allow top copying this class */
	MavlinkMixersManager(MavlinkMixersManager &);
	MavlinkMixersManager &operator = (const MavlinkMixersManager &);

protected:
	explicit MavlinkMixersManager(Mavlink *mavlink);
	~MavlinkMixersManager();

	void send(const hrt_abstime t);

	void        _send_mixer_count();

	bool        _request_pending;
	bool        _send_all;
	bool        _send_data_immediate;

	/**For tracking data when sending or streaming*/
	mixer_data_request_s    _mixer_data_req;
	mavlink_mixer_data_t    _msg_mixer_data_immediate;
	uint16_t    _mixer_group;
	uint16_t    _mixer_count;
	uint16_t    _mixer_sub_count;
	uint16_t    _mixer_type;
	uint16_t    _mixer_param_count;
	int16_t     _mavlink_mixer_state;
	char       *_p_mixer_save_buffer;

	orb_advert_t _mixer_data_request_pub;
	orb_advert_t _mixer_parameter_set_pub;
	int _mixer_data_sub;
};
#endif //MIXER_CONFIGURATION
