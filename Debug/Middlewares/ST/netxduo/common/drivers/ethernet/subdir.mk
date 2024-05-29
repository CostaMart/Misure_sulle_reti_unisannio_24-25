################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.c 

OBJS += \
./Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.o 

C_DEPS += \
./Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/netxduo/common/drivers/ethernet/%.o Middlewares/ST/netxduo/common/drivers/ethernet/%.su Middlewares/ST/netxduo/common/drivers/ethernet/%.cyclo: ../Middlewares/ST/netxduo/common/drivers/ethernet/%.c Middlewares/ST/netxduo/common/drivers/ethernet/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H563xx -DNX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_NON_SECURE=1 -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I../NetXDuo/App -I../NetXDuo/Target -I../AZURE_RTOS/App -I../Drivers/BSP/Components/lan8742 -I../Middlewares/ST/netxduo/common/drivers/ethernet -I../Middlewares/ST/threadx/common/inc -I../Middlewares/ST/netxduo/common/inc -I../Middlewares/ST/netxduo/ports/cortex_m33/gnu/inc -I../Middlewares/ST/threadx/ports/cortex_m33/gnu/inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-netxduo-2f-common-2f-drivers-2f-ethernet

clean-Middlewares-2f-ST-2f-netxduo-2f-common-2f-drivers-2f-ethernet:
	-$(RM) ./Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.cyclo ./Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.d ./Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.o ./Middlewares/ST/netxduo/common/drivers/ethernet/nx_stm32_eth_driver.su

.PHONY: clean-Middlewares-2f-ST-2f-netxduo-2f-common-2f-drivers-2f-ethernet

