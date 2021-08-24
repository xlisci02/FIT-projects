################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include board/subdir.mk
-include CMSIS/subdir.mk
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
all: MK60DN512xxx10_Project.axf

# Tool invocations
MK60DN512xxx10_Project.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="MK60DN512xxx10_Project.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -mcpu=cortex-m4 -mthumb -T MK60DN512xxx10_Project_Debug.ld -o "MK60DN512xxx10_Project.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) MK60DN512xxx10_Project.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "MK60DN512xxx10_Project.axf"; # arm-none-eabi-objcopy -v -O binary "MK60DN512xxx10_Project.axf" "MK60DN512xxx10_Project.bin" ; # checksum -p MK60DN512xxx10 -d "MK60DN512xxx10_Project.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
