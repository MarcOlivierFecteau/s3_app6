# PID avec P = -0.5, I = 0, D = 0, donc sans I ou D
# Auteurs: Marc-Olivier Fecteau (FECM0701), Gabriel Aubut (AUBG3402)

LDA R1,	$V_MOTEUR	# R1 = mem($V_MOTEUR)
LDA R2, $V_CIBLE	# R2 = mem($V_CIBLE)
LDA R3, $I_t		# R3 = mem($I_t)
LDC R4, 0			# R4 = 0
SUB R2, R1			# R2 = V_CIBLE(t) - v_mot(t) = e(t)
LDA R1,	$E_PREV		# R1 = mem($E_PREV)
ADD R4, R2			# R4 = R4 + e(t) = e(t)
ADD R3, R2			# R3 = I(t-1) + e(t) = I(t)
SUB R4, R1			# R4 = e(t) - e(t-1) = e_d(t)
STO $E_PREV, R2		# mem($E_PREV) = R2 = e(t)
LDA R1, $K_P		# R1 = mem($K_P)
STO $I_t, R3		# mem($I_t) = R3 = I(t)
MUL R2, R1			# R2 = e(t) * K_P
LDA R1, $K_I		# R1 = mem($K_I)
MUL R3, R1			# R3 = I(t) * K_I
LDA R1, $K_D		# R1 = mem($K_D)
ADD R2, R3			# R2 = K_P * e(t) + K_I * I(t)
MUL R4, R1			# R4 = e_d(t) * K_D
ADD R2, R4			# R2 = K_P * e(t) + K_I * I(t) + K_D * e_d(t) = CMD
STO $CMD, R2		# mem($CMD) = R2 = CMD