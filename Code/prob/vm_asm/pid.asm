# PID avec P = -0.5, I = 0, D = 0, donc sans I ou D
# Auteurs: Marc-Olivier Fecteau (FECM0701), Gabriel Aubut (AUBG3402)

LDA R1,	$v_moteur	# R1 = mem($v_moteur)
LDA R2, $v_cible	# R2 = mem($v_cible)
LDA R3, $I_t		# R3 = mem($I_t)
LDC R4, 0			# R4 = 0
SUB R2, R1			# R2 = v_cible(t) - v_mot(t) = e(t)
LDA R1,	$e_prev		# R1 = mem($e_prev)
ADD R4, R2			# R4 = R4 + e(t) = e(t)
ADD R3, R2			# R3 = I(t-1) + e(t) = I(t)
SUB R4, R1			# R4 = e(t) - e(t-1) = e_d(t)
STO $e_prev, R2		# mem($e_prev) = R2 = e(t)
LDA R1, $kp			# R1 = mem($kp)
STO $I_t, R3		# mem($I_t) = R3 = I(t)
MUL R2, R1			# R2 = e(t) * kp
LDA R1, $ki			# R1 = mem($ki)
MUL R3, R1			# R3 = I(t) * ki
LDA R1, $kd			# R1 = mem($kd)
ADD R2, R3			# R2 = kp * e(t) + ki * I(t)
MUL R4, R1			# R4 = e_d(t) * kd
ADD R2, R4			# R2 = kp * e(t) + ki * I(t) + kd * e_d(t) = cmd
STO $cmd, R2		# mem($cmd) = R2 = cmd