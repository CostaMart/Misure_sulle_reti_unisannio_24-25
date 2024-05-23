################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/app_threadx.c \
../Core/Src/main.c \
../Core/Src/stm32h5xx_hal_msp.c \
../Core/Src/stm32h5xx_hal_timebase_tim.c \
../Core/Src/stm32h5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h5xx_ns.c 

S_UPPER_SRCS += \
../Core/Src/tx_initialize_low_level.S 

OBJS += \
./Core/Src/app_threadx.o \
./Core/Src/main.o \
./Core/Src/stm32h5xx_hal_msp.o \
./Core/Src/stm32h5xx_hal_timebase_tim.o \
./Core/Src/stm32h5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h5xx_ns.o \
./Core/Src/tx_initialize_low_level.o 

S_UPPER_DEPS += \
./Core/Src/tx_initialize_low_level.d 

C_DEPS += \
./Core/Src/app_threadx.d \
./Core/Src/main.d \
./Core/Src/stm32h5xx_hal_msp.d \
./Core/Src/stm32h5xx_hal_timebase_tim.d \
./Core/Src/stm32h5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h5xx_ns.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H563xx -DNX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -c -I../Core/Inc -I../../Secure_nsclib -I../../Drivers/STM32H5xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../NetXDuo/App -I../NetXDuo/Target -I../AZURE_RTOS/App -I../../Drivers/BSP/Components/lan8742 -I../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../Middlewares/ST/threadx/common/inc -I../../Middlewares/ST/netxduo/common/inc -I../../Middlewares/ST/netxduo/ports/cortex_m33/gnu/inc -I../../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/%.o: ../Core/Src/%.S Core/Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m33 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/app_threadx.cyclo ./Core/Src/app_threadx.d ./Core/Src/app_threadx.o ./Core/Src/app_threadx.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32h5xx_hal_msp.cyclo ./Core/Src/stm32h5xx_hal_msp.d ./Core/Src/stm32h5xx_hal_msp.o ./Core/Src/stm32h5xx_hal_msp.su ./Core/Src/stm32h5xx_hal_timebase_tim.cyclo ./Core/Src/stm32h5xx_hal_timebase_tim.d ./Core/Src/stm32h5xx_hal_timebase_tim.o ./Core/Src/stm32h5xx_hal_timebase_tim.su ./Core/Src/stm32h5xx_it.cyclo ./Core/Src/stm32h5xx_it.d ./Core/Src/stm32h5xx_it.o ./Core/Src/stm32h5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h5xx_ns.cyclo ./Core/Src/system_stm32h5xx_ns.d ./Core/Src/system_stm32h5xx_ns.o ./Core/Src/system_stm32h5xx_ns.su ./Core/Src/tx_initialize_low_level.d ./Core/Src/tx_initialize_low_level.o

.PHONY: clean-Core-2f-Src

