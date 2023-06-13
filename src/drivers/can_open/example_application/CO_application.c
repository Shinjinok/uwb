/****************************************************************************
 *
 *   Copyright (c) 2014-2023 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *	used to endorse or promote products derived from this software
 *	without specific prior written permission.
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

/*
	This function provides C stubs that are called by CanOpenNode.
	Main reason this is here is to maintain compatibility with the
	can_open_node project.  Most people will add custom code to
	co_example_app.hpp as it provides more functionality.
*/
#include <stdio.h>

#include "CO_application.h"

CO_ReturnError_t app_programStart(uint16_t *bitRate,
				  uint8_t *nodeId,
				  uint32_t *errInfo)
{
	(void)bitRate;
	(void)nodeId;
	(void)errInfo;
	return CO_ERROR_NO;
}

void app_communicationReset(CO_t *co)
{
	(void)co;
}

void app_programEnd(void)
{

}

void app_programAsync(CO_t *co, uint32_t timer1usDiff)
{
	(void)co;
	(void)timer1usDiff;
}

void app_programRt(CO_t *co, uint32_t timer1usDiff)
{

}
