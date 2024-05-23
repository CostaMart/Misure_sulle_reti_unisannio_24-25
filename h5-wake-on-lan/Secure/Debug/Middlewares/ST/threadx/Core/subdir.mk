################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Alessandro/progetto_misure_sulle_reti/h5-wake-on-lan/Middlewares/ST/threadx/ports/cortex_m33/gnu/src/tx_thread_secure_stack.c 

OBJS += \
./Middlewares/ST/threadx/Core/tx_thread_secure_stack.o 

C_DEPS += \
./Middlewares/ST/threadx/Core/tx_thread_secure_stack.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/threadx/Core/tx_thread_secure_stack.o: C:/Users/Alessandro/progetto_misure_sulle_reti/h5-wake-on-lan/Middlewares/ST/threadx/ports/cortex_m33/gnu/src/tx_thread_secure_stack.c Middlewares/ST/threadx/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H563xx -c -I../Core/Inc -I../../Secure_nsclib -I../../Drivers/STM32H5xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../../Middlewares/ST/threadx/common/inc -I../../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-threadx-2f-Core

clean-Middlewares-2f-ST-2f-threadx-2f-Core:
	-$(RM) ./Middlewares/ST/threadx/Core/tx_thread_secure_stack.cyclo ./Middlewares/ST/threadx/Core/tx_thread_secure_stack.d ./Middlewares/ST/threadx/Core/tx_thread_secure_stack.o ./Middlewares/ST/threadx/Core/tx_thread_secure_stack.su

.PHONY: clean-Middlewares-2f-ST-2f-threadx-2f-Core

