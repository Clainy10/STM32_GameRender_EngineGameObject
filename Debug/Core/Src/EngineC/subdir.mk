################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/EngineC/engine.c \
../Core/Src/EngineC/gameobject.c \
../Core/Src/EngineC/input.c \
../Core/Src/EngineC/render.c 

OBJS += \
./Core/Src/EngineC/engine.o \
./Core/Src/EngineC/gameobject.o \
./Core/Src/EngineC/input.o \
./Core/Src/EngineC/render.o 

C_DEPS += \
./Core/Src/EngineC/engine.d \
./Core/Src/EngineC/gameobject.d \
./Core/Src/EngineC/input.d \
./Core/Src/EngineC/render.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/EngineC/%.o Core/Src/EngineC/%.su Core/Src/EngineC/%.cyclo: ../Core/Src/EngineC/%.c Core/Src/EngineC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G071xx -c -I../Core/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc -I../Drivers/STM32G0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-EngineC

clean-Core-2f-Src-2f-EngineC:
	-$(RM) ./Core/Src/EngineC/engine.cyclo ./Core/Src/EngineC/engine.d ./Core/Src/EngineC/engine.o ./Core/Src/EngineC/engine.su ./Core/Src/EngineC/gameobject.cyclo ./Core/Src/EngineC/gameobject.d ./Core/Src/EngineC/gameobject.o ./Core/Src/EngineC/gameobject.su ./Core/Src/EngineC/input.cyclo ./Core/Src/EngineC/input.d ./Core/Src/EngineC/input.o ./Core/Src/EngineC/input.su ./Core/Src/EngineC/render.cyclo ./Core/Src/EngineC/render.d ./Core/Src/EngineC/render.o ./Core/Src/EngineC/render.su

.PHONY: clean-Core-2f-Src-2f-EngineC

