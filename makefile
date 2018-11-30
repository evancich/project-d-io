################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include CMSIS/EFM32PG1B/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: project-d-io.axf

# Tool invocations
project-d-io.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "project-d-io.ld" -Xlinker --gc-sections -Xlinker -Map="project-d-io.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o project-d-io.axf "./CMSIS/EFM32PG1B/startup_efm32pg1b.o" "./CMSIS/EFM32PG1B/system_efm32pg1b.o" "./emlib/em_adc.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_dma.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_idac.o" "./emlib/em_ldma.o" "./emlib/em_rtcc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./src/main_s1.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: project-d-io.hex'
	arm-none-eabi-objcopy -O ihex "project-d-io.axf" "project-d-io.hex"
	@echo ' '

	@echo 'Building bin file: project-d-io.bin'
	arm-none-eabi-objcopy -O binary "project-d-io.axf" "project-d-io.bin"
	@echo ' '

	@echo 'Building s37 file: project-d-io.s37'
	arm-none-eabi-objcopy -O srec "project-d-io.axf" "project-d-io.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "project-d-io.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) project-d-io.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
