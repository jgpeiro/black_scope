################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/Font/FontUbuntuBookRNormal16.c \
../Lib/Font/font.c 

OBJS += \
./Lib/Font/FontUbuntuBookRNormal16.o \
./Lib/Font/font.o 

C_DEPS += \
./Lib/Font/FontUbuntuBookRNormal16.d \
./Lib/Font/font.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/Font/%.o Lib/Font/%.su Lib/Font/%.cyclo: ../Lib/Font/%.c Lib/Font/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNK_INCLUDE_STANDARD_VARARGS -DUSE_HAL_DRIVER -DSTM32G473xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Lcd" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Scope" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Tsc" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Framebuf" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Font" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Nuklear" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-Font

clean-Lib-2f-Font:
	-$(RM) ./Lib/Font/FontUbuntuBookRNormal16.cyclo ./Lib/Font/FontUbuntuBookRNormal16.d ./Lib/Font/FontUbuntuBookRNormal16.o ./Lib/Font/FontUbuntuBookRNormal16.su ./Lib/Font/font.cyclo ./Lib/Font/font.d ./Lib/Font/font.o ./Lib/Font/font.su

.PHONY: clean-Lib-2f-Font

