################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/Lcd/lcd.c 

OBJS += \
./Lib/Lcd/lcd.o 

C_DEPS += \
./Lib/Lcd/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/Lcd/%.o Lib/Lcd/%.su Lib/Lcd/%.cyclo: ../Lib/Lcd/%.c Lib/Lcd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G473xx -DNK_INCLUDE_STANDARD_VARARGS -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/git/black_scope/software/test_g473_blink/Nuklear" -I"C:/git/black_scope/software/test_g473_blink/tsc2046" -I"C:/git/black_scope/software/test_g473_blink/Lib/Framebuf" -I"C:/git/black_scope/software/test_g473_blink/Lib/Lcd" -O1 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-Lcd

clean-Lib-2f-Lcd:
	-$(RM) ./Lib/Lcd/lcd.cyclo ./Lib/Lcd/lcd.d ./Lib/Lcd/lcd.o ./Lib/Lcd/lcd.su

.PHONY: clean-Lib-2f-Lcd

