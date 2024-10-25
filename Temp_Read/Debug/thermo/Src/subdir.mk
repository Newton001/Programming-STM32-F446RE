################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../thermo/Src/thermo.c 

OBJS += \
./thermo/Src/thermo.o 

C_DEPS += \
./thermo/Src/thermo.d 


# Each subdirectory must supply rules for building sources it contributes
thermo/Src/%.o thermo/Src/%.su thermo/Src/%.cyclo: ../thermo/Src/%.c thermo/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Temp_Read/thermo" -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Temp_Read/thermo/Inc" -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Temp_Read/thermo/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-thermo-2f-Src

clean-thermo-2f-Src:
	-$(RM) ./thermo/Src/thermo.cyclo ./thermo/Src/thermo.d ./thermo/Src/thermo.o ./thermo/Src/thermo.su

.PHONY: clean-thermo-2f-Src

