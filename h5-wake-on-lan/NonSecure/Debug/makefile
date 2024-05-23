################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include NetXDuo/App/subdir.mk
-include Middlewares/ST/threadx/Core/subdir.mk
-include Middlewares/ST/netxduo/NX\ Core/subdir.mk
-include Middlewares/ST/netxduo/LAN8742\ Phy\ Interface/subdir.mk
-include Middlewares/ST/netxduo/Ethernet\ Interface/subdir.mk
-include Drivers/STM32H5xx_HAL_Driver/subdir.mk
-include Drivers/BSP/Components/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include AZURE_RTOS/App/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := h5-wake-on-lan_NonSecure
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
h5-wake-on-lan_NonSecure.elf \

MAP_FILES += \
h5-wake-on-lan_NonSecure.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
h5-wake-on-lan_NonSecure.list \


# All Target
all: main-build

# Main-build Target
main-build: h5-wake-on-lan_NonSecure.elf secondary-outputs

dependents:
	-cd C:/Users/Alessandro/progetto_misure_sulle_reti/h5-wake-on-lan/Secure/Debug && $(MAKE) all

# Tool invocations
h5-wake-on-lan_NonSecure.elf h5-wake-on-lan_NonSecure.map: $(OBJS) $(USER_OBJS) C:/Users/Alessandro/progetto_misure_sulle_reti/h5-wake-on-lan/Secure/Debug/h5-wake-on-lan_Secure.elf C:\Users\Alessandro\progetto_misure_sulle_reti\h5-wake-on-lan\NonSecure\STM32H563ZITX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "h5-wake-on-lan_NonSecure.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m33 -T"C:\Users\Alessandro\progetto_misure_sulle_reti\h5-wake-on-lan\NonSecure\STM32H563ZITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="h5-wake-on-lan_NonSecure.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

h5-wake-on-lan_NonSecure.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "h5-wake-on-lan_NonSecure.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) default.size.stdout h5-wake-on-lan_NonSecure.elf h5-wake-on-lan_NonSecure.list h5-wake-on-lan_NonSecure.map
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified
C:/Users/Alessandro/progetto_misure_sulle_reti/h5-wake-on-lan/Secure/Debug/h5-wake-on-lan_Secure.elf:

-include ../makefile.targets
