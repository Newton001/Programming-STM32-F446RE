################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RC522/Src/rc522.c 

OBJS += \
./RC522/Src/rc522.o 

C_DEPS += \
./RC522/Src/rc522.d 


# Each subdirectory must supply rules for building sources it contributes
RC522/Src/%.o RC522/Src/%.su RC522/Src/%.cyclo: ../RC522/Src/%.c RC522/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/RFID_Gym_Member_Management_System/LCD_Driver/Inc" -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/RFID_Gym_Member_Management_System/RC522/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-RC522-2f-Src

clean-RC522-2f-Src:
	-$(RM) ./RC522/Src/rc522.cyclo ./RC522/Src/rc522.d ./RC522/Src/rc522.o ./RC522/Src/rc522.su

.PHONY: clean-RC522-2f-Src

