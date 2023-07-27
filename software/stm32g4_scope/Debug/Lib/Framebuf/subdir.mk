################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/Framebuf/framebuf.c 

OBJS += \
./Lib/Framebuf/framebuf.o 

C_DEPS += \
./Lib/Framebuf/framebuf.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/Framebuf/%.o Lib/Framebuf/%.su Lib/Framebuf/%.cyclo: ../Lib/Framebuf/%.c Lib/Framebuf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNK_INCLUDE_STANDARD_VARARGS -DUSE_HAL_DRIVER -DSTM32G473xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Lcd" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Scope" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Tsc" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Framebuf" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Font" -I"C:/Users/jgpei/STM32CubeIDE/workspace_1.13.0/stm32g4_scope/Lib/Nuklear" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-Framebuf

clean-Lib-2f-Framebuf:
	-$(RM) ./Lib/Framebuf/framebuf.cyclo ./Lib/Framebuf/framebuf.d ./Lib/Framebuf/framebuf.o ./Lib/Framebuf/framebuf.su

.PHONY: clean-Lib-2f-Framebuf

