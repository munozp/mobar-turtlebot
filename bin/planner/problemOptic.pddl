(define (problem turtlebot)
(:domain MoBAr-Rover)
(:objects
        highRes depthField - mode
	    p120_120 p80_120 - ptupos
        turtle - rover
)
(:init
;(is_charging turtle)
(free turtle)
(= (square_size) 0.4) ;m
;TURTLEBOT DATA AND CONFIG
(position turtle c12_3)
;Energy and SubSystems
(= (energy_cons turtle) 0)
(= (energy turtle) 2.040)
;LOCOMOTION
(= (speed turtle) 0.1) ; m/seg
(= (power_per_dis turtle) 0.006) ; A/m
;CAMERAS
(camera_mode turtle kinect highRes)
(= (transmit_energy turtle) 0.001)
(= (camera_energy kinect highRes) 0.01) 
(= (time_to_picture kinect highRes) 8)  
(ptu_pos turtle kinect p90_90)
(= (time_move_ptu p90_90 p120_120) 2)
(= (time_move_ptu p120_120 p90_90) 2)
(= (time_move_ptu p80_120 p90_90) 2)
(= (time_move_ptu p90_90 p80_120) 2)
(= (ptu_energy) 0.0005)
;AUTODOCK
(dock c13_16)
(= (autodocking-time c13_16) 10)
(= (energy_charge c13_16) 0.4)
(= (time_to_charge c13_16) 60)
(dock c16_3)
(= (autodocking-time c16_3) 10)
(= (energy_charge c16_3) 0.4)
(= (time_to_charge c16_3) 30)
)
;===============================================================================
;GOALS AND METRIC
(:goal (and
(is_charging turtle)
(visited turtle c15_10)
)))
