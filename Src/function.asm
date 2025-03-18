			PRESERVE8
			AREA        |.text|, CODE, READONLY

			EXPORT 		Update_Mes
Update_Mes	PROC
			ldr			R2,[R0]
			ldr			R1,=0x00FC0100
			uadd8		R2,R2,R1
			ldr			R1,=0xFF7F7FFF
			ands		R2,R2,R1
			str			R2,[R0]
			bx			LR
			ENDP

			END
