(define (problem turtlebot)
(:domain MoBAr-Rover)
(:objects
;
        highRes depthField - mode
	    p90_70 p145_100 p120_100 - ptupos
        turtle - rover
)
(:init
;DATA AND CONFIG
(= (square_size) 0.4) ;m
;(position turtle c11_3)/////////
;(free turtle)//////////////////////
(position turtle c13_16)
(is_charging turtle)
;Energy and SubSystems
(= (energy_cons turtle) 0)
(= (energy turtle) 15840);A totales de la batería (4.4 A/h* 3600s)
;LOCOMOTION
(= (speed turtle) 0.075); m/seg (0.1)
(= (power_per_dis turtle) 26.20) ; A/m (0.006)
;CAMERAS
(camera_mode turtle kinect highRes)
(= (extra_energy turtle) 10.55); Turn energy
(= (camera_energy kinect highRes) 1.95) ;0.01
(= (time_to_picture kinect highRes) 8)  
(ptu_pos turtle kinect p90_90)
(= (time_move_ptu p90_90 p90_70) 3)
(= (time_move_ptu p90_70 p90_90) 3)
(= (time_move_ptu p145_100 p90_90) 3)
(= (time_move_ptu p90_90 p145_100) 3)
(= (time_move_ptu p90_90 p120_100) 3)
(= (time_move_ptu p120_100 p90_90) 3)
(= (ptu_energy) 2.07)
;AUTODOCK
(dock c13_16)
(= (autodocking-time c13_16) 10)
(= (energy_charge c13_16) 0.4)
;(dock c16_3)
;(= (autodocking-time c16_3) 10)
;(= (energy_charge c16_3) 0.4)
;(= (time_to_charge c16_3) 10)
;UNDOCK
(= (undocking_duration turtle) 11)
(= (turn_time turtle) 20)
)
;===============================================================================
;GOALS AND METRIC
(:goal (and
PDDL_GOALS 
)))
