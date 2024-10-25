################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Speaker/Src/speaker.c 

OBJS += \
./Speaker/Src/speaker.o 

C_DEPS += \
./Speaker/Src/speaker.d 


# Each subdirectory must supply rules for building sources it contributes
Speaker/Src/%.o Speaker/Src/%.su Speaker/Src/%.cyclo: ../Speaker/Src/%.c Speaker/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/newto/STM32CubeIDE/workspace_1.16.0/Music_Box_Ass/Speaker/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Speaker-2f-Src

clean-Speaker-2f-Src:
	-$(RM) ./Speaker/Src/speaker.cyclo ./Speaker/Src/speaker.d ./Speaker/Src/speaker.o ./Speaker/Src/speaker.su

.PHONY: clean-Speaker-2f-Src

