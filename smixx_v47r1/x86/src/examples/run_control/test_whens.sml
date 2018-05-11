object: AUTO_PILOT
	state: off
		action: ACTIVATE			
			do RECOVER RUN
			do START_RUN RUN
			terminate_action/state=ON
	state: ON
	when (RUN not_in_state RUNNING) do X_ACTIVATE
		action: DISACTIVATE
			terminate_action/state=OFF
		action: X_ACTIVATE			
			do RECOVER RUN
			do START_RUN RUN
			terminate_action/state=ON

object: RUN_TYPE
	state: TEST
		action: PHYSICS
			do LOG LOGGER
			if (LOGGER in_state LOGGING) then
				terminate_action/state=PHYSICS
			endif
			terminate_action/state=TEST
	state: PHYSICS
		action: TEST
			do NOLOG LOGGER
			if (LOGGER in_state NOT_LOGGING) then
				terminate_action/state=TEST
			endif
			terminate_action/state=PHYSICS

object: RUN
	parameters: int NUMBER_T = 0, int NUMBER_P = 0
	state: STOPPED
		action: START_RUN
			set NUMBER_T = EVT_BUILDER<NUMBER_T>
			set NUMBER_P = EVT_BUILDER<NUMBER_P>
			if (not EVT_BUILDER in_state READY) then
				terminate_action/state=STOPPED
			endif
			if (RUN_TYPE in_state PHYSICS) then
				do LOG LOGGER
				if (not LOGGER in_state LOGGING) then
					terminate_action/state=STOPPED
				endif
			endif
			if (LOGGER in_state LOGGING) then
				do X_OPEN_FILE LOGGER
			endif
			if (RUN_TYPE in_state PHYSICS) then
				do START (TYPE="PHYSICS",NR=NUMBER_P) EVT_BUILDER
			else
				do START (TYPE="TEST",NR=NUMBER_T) EVT_BUILDER
			endif
			if (EVT_BUILDER in_state RUNNING) then
				terminate_action/state=RUNNING
			endif
			terminate_action/state=STOPPED
	state: RUNNING
	when (EVT_BUILDER in_state ERROR) do X_SET_ERROR
		action: STOP_RUN
			do STOP EVT_BUILDER
			if (LOGGER in_state WRITING) then
				do X_CLOSE_FILE LOGGER
			endif
			terminate_action/state=STOPPED
		action: X_SET_ERROR
			if (LOGGER in_state WRITING) then
				do X_CLOSE_FILE LOGGER
			endif
			terminate_action/state=ERROR
	state: ERROR
		action: RECOVER
			do RECOVER EVT_BUILDER
			terminate_action/state=STOPPED

object: LOGGER /associated
	state: DEAD /dead_state
	state: NOT_LOGGING
		action: LOG
	state: LOGGING
		action: NOLOG
		action: X_OPEN_FILE
	state: WRITING
		action: X_CLOSE_FILE

object: EVT_BUILDER /associated
	parameters: int NUMBER_T, int NUMBER_P
	state: DEAD /dead_state
	state: READY
		action: START(TYPE, int NR)
	state: RUNNING
		action: STOP
	state: ERROR
		action: RECOVER

