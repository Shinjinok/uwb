/****************************************************************************
 *
 *   Copyright (c) 2024 PX4 Development Team. All rights reserved.
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

#include <matrix/math.hpp>

using namespace matrix;

/**
 * @file PurePursuit.hpp
 *
 * Implementation of pure pursuit guidance logic.
 *
 * Acknowledgements and References:
 *
 *    This implementation has been built for PX4 based on the idea from [1] (not including any code).
 *
 *    [1] Coulter, R. C. (1992). Implementation of the Pure Pursuit Path Tracking Algorithm
 * 	  (Techreport CMU-RI-TR-92-01).
 *
 * Pure pursuit is a path following algorithm that uses the intersection between the path and
 * a circle (the radius of which is referred to as lookahead distance) around the vehicle as
 * the target point for the vehicle.
 * 							C
 * 		  				       /
 * 						    __/__
 * 						  /  /    \
 * 						 /  /      \
 * 						|  /  V     |
 * 						 \/        /
 * 						 /\ _____ /
 * 	         	    N (0 rad)		/
 * 			        ^	       P
 * 				|
 * 				| D
 *  	   (-1.5708 rad) <----- ⨂ -----> E (1.5708 rad)
 * 				|
 * 				|
 * 				⌄
 * 			(+- 3.14159 rad)
 *
 * Input:  Current/prev waypoint and the vehicle position in NED frame as well as the lookahead distance.
 * Output: Calculates the intersection points and returns the heading towards the point that is closer to the current waypoint.
 */
class PurePursuit
{
public:
	/**
	 * @brief Return heading towards the intersection point between a circle with a radius of
	 * lookahead_distance around the vehicle and an extended line segment from the previous to the current waypoint.
	 * Exceptions:
	 * 	Will return heading towards the current waypoint if it is closer to the vehicle than the lookahead.
	 * 	Will return heading towards the closest point on the path if there are no intersection points (crosstrack error bigger than lookahead).
	 * 	Will return NAN if input is invalid.
	 * @param curr_wp_ned North/East coordinates of current waypoint in NED frame [m].
	 * @param prev_wp_ned North/East coordinates of previous waypoint in NED frame [m].
	 * @param curr_pos_ned North/East coordinates of current position of the vehicle in NED frame [m].
	 * @param lookahead_distance Radius of circle around vehicle [m].
	 */
	float calcDesiredHeading(const Vector2f &curr_wp_ned, const Vector2f &prev_wp_ned, const Vector2f &curr_pos_ned,
				 const float lookahead_distance);
};
