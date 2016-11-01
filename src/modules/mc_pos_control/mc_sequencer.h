#ifndef MC_SEQUENCER_H
#define MC_SEQUENCER_H

#include <px4_defines.h>
#include <math.h>
#include <drivers/drv_hrt.h>

#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/control_state.h>

#include <mathlib/mathlib.h>
#include <platforms/px4_defines.h>

/*
 * Perform a programmed sequence with specified target attitudes
 *
 * Inputs:
 * sequence: array of seq_entry items
 * ctrl_state: for current attitude quaternion
 * manual: sequence trigger switch
 *
 * Outputs:
 * att_sp: thrust and rotation angle setpoints
 * R_sp: attitude setpoint
 * rollRate, pitchRate, yawRate: rotation rate commands
 */
void prog_sequence(
	struct control_state_s &ctrl_state,
	struct vehicle_attitude_setpoint_s &att_sp,
	struct manual_control_setpoint_s &manual,
	matrix::Quatf &R_sp,
	float &rollRate, float &pitchRate, float &yawRate);

enum Seq_state {
	IDLE, RATE, ATTITUDE, DELAY, NEXT_ENTRY
};

struct seq_entry_s {
	Seq_state type;
	float thrust;

	// rates are in radians/second (independent of parameter values)
	float rollRate;
	float pitchRate;
	float yawRate;

	float target_euler[3];

	float delay;
};

struct sequence {
	int N_entries;
	struct seq_entry_s *entries;

	sequence(int n) : N_entries(n)
	{
		entries = (seq_entry_s *) malloc(sizeof(seq_entry_s) * N_entries);
	}
	~sequence()
	{
		free((void *)entries);
	}
};

enum sequence_set { hover, coord_turn, pitch_flip, roll_flip, two_point_roll, tilt_lr };

sequence *get_sequence(sequence_set entry);

void prog_sequence_init(enum sequence_set seq, float trigger_interval);

#endif
