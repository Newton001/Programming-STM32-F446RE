################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mma7660fc/Src/mma7660fc.c 

OBJS += \
./mma7660fc/Src/mma7660fc.o 

C_DEPS += \
./mma7660fc/Src/mma7660fc.d 


# Each subdirectory must supply rules for building sources it contributes
mma7660fc/Src/%.o mma7660fc/Src/%.su mma7660fc/Src/%.cyclo: ../mma7660fc/Src/%.c mma7660fc/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Digital_Accelerometer/mma7660fc/Inc" -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Digital_Accelerometer/LCD_Driver/LCD_Driver/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-mma7660fc-2f-Src

clean-mma7660fc-2f-Src:
	-$(RM) ./mma7660fc/Src/mma7660fc.cyclo ./mma7660fc/Src/mma7660fc.d ./mma7660fc/Src/mma7660fc.o ./mma7660fc/Src/mma7660fc.su

.PHONY: clean-mma7660fc-2f-Src

