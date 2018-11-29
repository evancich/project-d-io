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
all: SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf

# Tool invocations
SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "SLSTK3401A_EFM32PG1B_adc_single_interrupt.ld" -Xlinker --gc-sections -Xlinker -Map="SLSTK3401A_EFM32PG1B_adc_single_interrupt.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf "./CMSIS/EFM32PG1B/startup_efm32pg1b.o" "./CMSIS/EFM32PG1B/system_efm32pg1b.o" "./emlib/em_adc.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_dma.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_idac.o" "./emlib/em_ldma.o" "./emlib/em_rtcc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./src/main_s1.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: SLSTK3401A_EFM32PG1B_adc_single_interrupt.hex'
	arm-none-eabi-objcopy -O ihex "SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf" "SLSTK3401A_EFM32PG1B_adc_single_interrupt.hex"
	@echo ' '

	@echo 'Building bin file: SLSTK3401A_EFM32PG1B_adc_single_interrupt.bin'
	arm-none-eabi-objcopy -O binary "SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf" "SLSTK3401A_EFM32PG1B_adc_single_interrupt.bin"
	@echo ' '

	@echo 'Building s37 file: SLSTK3401A_EFM32PG1B_adc_single_interrupt.s37'
	arm-none-eabi-objcopy -O srec "SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf" "SLSTK3401A_EFM32PG1B_adc_single_interrupt.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) SLSTK3401A_EFM32PG1B_adc_single_interrupt.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
