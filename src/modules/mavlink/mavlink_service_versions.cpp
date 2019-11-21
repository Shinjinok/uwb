/****************************************************************************
 *
 *   Copyright (c) 2012-2019 PX4 Development Team. All rights reserved.
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
 * @file mavlink_service_versions.c
 * Compile-time constants for supported MAVLink microservices
 *
 * @author Timothy Scott <timothy@auterion.com>
 */

#include "mavlink_service_versions.h"

// TODO microservice versioning: These will be replaced with the autogenerated MAVLink constants
#define MAVLINK_SERVICE_ID_MISSION 1
#define MAVLINK_SERVICE_ID_PARAMETERS 2

namespace microservice_versions
{

const service_metadata services_metadata[NUM_SERVICES] {
	{
		.service_id = MAVLINK_SERVICE_UNKNOWN,
		.min_version = 0,
		.max_version = 0
	},
	{
		.service_id = MAVLINK_SERVICE_ID_MISSION,
		.min_version = 1,
		.max_version = 3
	},
	{
		.service_id = MAVLINK_SERVICE_ID_PARAMETERS,
		.min_version = 1,
		.max_version = 1
	}
};

// TODO microservice versioning: Remove this
//	const service_metadata *get_metadata(uint16_t service_id){
//		for(size_t i = 0; i < NUM_SERVICES; i++){
//			if(services_metadata[i].service_id == service_id){
//				return &services_metadata[i];
//			}
//		}
//		return &services_metadata[0];
//	}
}