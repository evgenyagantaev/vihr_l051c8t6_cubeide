################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32L0xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
vihr_l051c8t6_cubeide.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
vihr_l051c8t6_cubeide.list \


# All Target
all: vihr_l051c8t6_cubeide.elf secondary-outputs

# Tool invocations
vihr_l051c8t6_cubeide.elf: $(OBJS) $(USER_OBJS) /home/eugeny/STM32CubeIDE/workspace_1.1.0/vihr_l051c8t6_cubeide/STM32L051C8TX_FLASH.ld
	arm-none-eabi-gcc -o "vihr_l051c8t6_cubeide.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m0plus -T"/home/eugeny/STM32CubeIDE/workspace_1.1.0/vihr_l051c8t6_cubeide/STM32L051C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="vihr_l051c8t6_cubeide.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

vihr_l051c8t6_cubeide.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "vihr_l051c8t6_cubeide.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
