################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Alessandro/progetto_misure_sulle_reti/h5-wake-on-lan/Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.c 

OBJS += \
./Middlewares/ST/netxduo/Ethernet\ Interface/nx_stm32_eth_driver.o 

C_DEPS += \
./Middlewares/ST/netxduo/Ethernet\ Interface/nx_stm32_eth_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/netxduo/Ethernet\ Interface/nx_stm32_eth_driver.o: C:/Users/Alessandro/progetto_misure_sulle_reti/h5-wake-on-lan/Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.c Middlewares/ST/netxduo/Ethernet\ Interface/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H563xx -DNX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -c -I../Core/Inc -I../../Secure_nsclib -I../../Drivers/STM32H5xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Include -I../NetXDuo/App -I../NetXDuo/Target -I../AZURE_RTOS/App -I../../Drivers/BSP/Components/lan8742 -I../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../Middlewares/ST/threadx/common/inc -I../../Middlewares/ST/netxduo/common/inc -I../../Middlewares/ST/netxduo/ports/cortex_m33/gnu/inc -I../../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Middlewares/ST/netxduo/Ethernet Interface/nx_stm32_eth_driver.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-netxduo-2f-Ethernet-20-Interface

clean-Middlewares-2f-ST-2f-netxduo-2f-Ethernet-20-Interface:
	-$(RM) ./Middlewares/ST/netxduo/Ethernet\ Interface/nx_stm32_eth_driver.cyclo ./Middlewares/ST/netxduo/Ethernet\ Interface/nx_stm32_eth_driver.d ./Middlewares/ST/netxduo/Ethernet\ Interface/nx_stm32_eth_driver.o ./Middlewares/ST/netxduo/Ethernet\ Interface/nx_stm32_eth_driver.su

.PHONY: clean-Middlewares-2f-ST-2f-netxduo-2f-Ethernet-20-Interface

