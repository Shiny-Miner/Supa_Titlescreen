.text
.align 2
.thumb

.include "../xse_commands.s"
.include "../xse_defines.s"

.global Script1
Script1:
	lockall
    setvar 0x8000 0x0
    callasm ShowImage
    waitkeypress
    callasm CB2_ReturnToFieldContinueScript
	releaseall
    end
