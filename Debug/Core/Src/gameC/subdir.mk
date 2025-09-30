################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/gameC/mario.c 

OBJS += \
./Core/Src/gameC/mario.o 

C_DEPS += \
./Core/Src/gameC/mario.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/gameC/%.o Core/Src/gameC/%.su Core/Src/gameC/%.cyclo: ../Core/Src/gameC/%.c Core/Src/gameC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G071xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-gameC

clean-Core-2f-Src-2f-gameC:
	-$(RM) ./Core/Src/gameC/mario.cyclo ./Core/Src/gameC/mario.d ./Core/Src/gameC/mario.o ./Core/Src/gameC/mario.su

.PHONY: clean-Core-2f-Src-2f-gameC

