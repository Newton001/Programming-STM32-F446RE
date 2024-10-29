################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Thermo/Src/thermo.c 

OBJS += \
./Thermo/Src/thermo.o 

C_DEPS += \
./Thermo/Src/thermo.d 


# Each subdirectory must supply rules for building sources it contributes
Thermo/Src/%.o Thermo/Src/%.su Thermo/Src/%.cyclo: ../Thermo/Src/%.c Thermo/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/T_6_1_DigitalThermometer/LCD_Driver/Inc" -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/T_6_1_DigitalThermometer/Thermo/Inc" -I../Core/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Thermo-2f-Src

clean-Thermo-2f-Src:
	-$(RM) ./Thermo/Src/thermo.cyclo ./Thermo/Src/thermo.d ./Thermo/Src/thermo.o ./Thermo/Src/thermo.su

.PHONY: clean-Thermo-2f-Src

