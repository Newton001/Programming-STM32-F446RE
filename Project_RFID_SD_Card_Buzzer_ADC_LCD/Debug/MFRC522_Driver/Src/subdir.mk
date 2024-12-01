################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MFRC522_Driver/Src/MFRC522_Driver.c 

OBJS += \
./MFRC522_Driver/Src/MFRC522_Driver.o 

C_DEPS += \
./MFRC522_Driver/Src/MFRC522_Driver.d 


# Each subdirectory must supply rules for building sources it contributes
MFRC522_Driver/Src/%.o MFRC522_Driver/Src/%.su MFRC522_Driver/Src/%.cyclo: ../MFRC522_Driver/Src/%.c MFRC522_Driver/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Project_RFID_SD_Card_Buzzer_ADC_LCD/LCD_Driver/LCD_Driver/Inc" -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Project_RFID_SD_Card_Buzzer_ADC_LCD/MFRC522_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-MFRC522_Driver-2f-Src

clean-MFRC522_Driver-2f-Src:
	-$(RM) ./MFRC522_Driver/Src/MFRC522_Driver.cyclo ./MFRC522_Driver/Src/MFRC522_Driver.d ./MFRC522_Driver/Src/MFRC522_Driver.o ./MFRC522_Driver/Src/MFRC522_Driver.su

.PHONY: clean-MFRC522_Driver-2f-Src

