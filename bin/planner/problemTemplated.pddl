(define (problem turtlebot)
(:domain MoBAr-Rover)
(:objects
        A-4 A-3 A-2 A-1 A0 A1 A2 A3 A4 A13 A45 A90 A135 A180 A225 A270 A315 A360 - ang
        kinect - cam highRes depthField - mode
	P90_90 P120_120 P80_120 - ptupos
        fast - navmode
        turtle - rover
)
(:init
(= (square_size) 0.4) ;m
;TURTLEBOT DATA AND CONFIG
(position turtle C10_15)
(orientation turtle A0)
;Energy and SubSystems
(= (energy turtle) 2.000)
(= (energy_cons turtle) 0)
;LOCOMOTION
(has_locomotion turtle)
(navigation_mode turtle fast)
(= (speed turtle fast) 0.04) ; m/seg
;(= (speed turtle slow) 0.02)
(= (power_per_dis turtle fast) 0.006) ; A/m
;(= (power_per_dis turtle slow) 0.05) 
(= (ang_to_rotate A0 A45) 45)
(= (ang_to_rotate A45 A90) 45)
(= (ang_to_rotate A90 A135) 45)
(= (ang_to_rotate A135 A180) 45)
(= (ang_to_rotate A180 A225) 45)
(= (ang_to_rotate A225 A270) 45)
(= (ang_to_rotate A270 A315) 45)
(= (ang_to_rotate A315 A360) 45)
(= (ang_to_rotate A360 A0) 0)
(= (ang_to_rotate A0 A360) 0)
(= (ang_to_rotate A45 A0) 45)
(= (ang_to_rotate A90 A45) 45)
(= (ang_to_rotate A135 A45) 45)
(= (ang_to_rotate A180 A135) 45)
(= (ang_to_rotate A225 A180) 45)
(= (ang_to_rotate A270 A225) 45)
(= (ang_to_rotate A315 A270) 45)
(= (ang_to_rotate A360 A315) 45)
(= (ang_speed turtle fast) 0.3)
;(= (ang_speed turtle slow) 0.1)
(= (power_per_deg turtle fast) 0.003)
;(= (power_per_deg turtle slow) 0.02)
;CAMERAS
(camera_mode turtle kinect highRes)
(= (transmit_energy turtle) 0.001)
(= (camera_energy kinect highRes) 0.01) ;Consumo completo Ah
(= (time_to_picture kinect highRes) 8)  ;Tiempo en segundos
(ptu_pos turtle kinect P90_90)
(= (time_move_ptu P90_90 P120_120) 2)
(= (time_move_ptu P120_120 P90_90) 2)
(= (time_move_ptu P80_120 P90_90) 2)
(= (time_move_ptu P90_90 P80_120) 2)
(= (ptu_energy) 0.0005)
;AUTODOCK
(dock C15_11)
(= (autodocking-time C15_11) 10)
(= (energy_charge C15_11) 0.1)
(= (time_to_charge C15_11) 60)
(= (time_point) 0)
)
;===============================================================================
;OBJECTIVES AND METRIC
(:goal (and
PDDL_GOALS
)))
