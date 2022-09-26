/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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

#pragma once

#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>

#include <src/modules/microdds_client/dds_topics.h>

#include <lib/timesync/Timesync.hpp>

class MicroddsClient : public ModuleBase<MicroddsClient>, public ModuleParams
{
public:
	enum class Transport {
		Serial,
		Udp
	};

	MicroddsClient(Transport transport, const char *device, int baudrate, const char *host, const char *recv_port,
		       const char *send_port, bool localhost_only, const char *client_namespace);

	~MicroddsClient();

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static MicroddsClient *instantiate(int argc, char *argv[]);

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	/** @see ModuleBase::run() */
	void run() override;

	/** @see ModuleBase::print_status() */
	int print_status() override;

private:
	int setBaudrate(int fd, unsigned baud);

	const bool _localhost_only;
	const char *_client_namespace;

	SendTopicsSubs *_subs{nullptr};
	RcvTopicsPubs *_pubs{nullptr};

	uxrSerialTransport *_transport_serial{nullptr};
	uxrUDPTransport *_transport_udp{nullptr};
	uxrCommunication *_comm{nullptr};
	int _fd{-1};

	int _last_payload_tx_rate{}; ///< in B/s
	int _last_payload_rx_rate{}; ///< in B/s
	bool _connected{false};

	Timesync _timesync{};

	DEFINE_PARAMETERS(
		(ParamInt<px4::params::XRCE_DDS_DOM_ID>) _param_xrce_dds_dom_id,
		(ParamInt<px4::params::XRCE_DDS_KEY>) _param_xrce_key
	)
};

