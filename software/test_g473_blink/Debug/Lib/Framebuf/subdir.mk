################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Lib/Framebuf/FontUbuntuBookRNormal12.c \
../Lib/Framebuf/FontUbuntuBookRNormal16.c \
../Lib/Framebuf/font_petme128_8x8.c \
../Lib/Framebuf/framebuf.c 

OBJS += \
./Lib/Framebuf/FontUbuntuBookRNormal12.o \
./Lib/Framebuf/FontUbuntuBookRNormal16.o \
./Lib/Framebuf/font_petme128_8x8.o \
./Lib/Framebuf/framebuf.o 

C_DEPS += \
./Lib/Framebuf/FontUbuntuBookRNormal12.d \
./Lib/Framebuf/FontUbuntuBookRNormal16.d \
./Lib/Framebuf/font_petme128_8x8.d \
./Lib/Framebuf/framebuf.d 


# Each subdirectory must supply rules for building sources it contributes
Lib/Framebuf/%.o Lib/Framebuf/%.su Lib/Framebuf/%.cyclo: ../Lib/Framebuf/%.c Lib/Framebuf/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G473xx -DNK_INCLUDE_STANDARD_VARARGS -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/git/black_scope/software/test_g473_blink/Nuklear" -I"C:/git/black_scope/software/test_g473_blink/tsc2046" -I"C:/git/black_scope/software/test_g473_blink/Lib/Framebuf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Lib-2f-Framebuf

clean-Lib-2f-Framebuf:
	-$(RM) ./Lib/Framebuf/FontUbuntuBookRNormal12.cyclo ./Lib/Framebuf/FontUbuntuBookRNormal12.d ./Lib/Framebuf/FontUbuntuBookRNormal12.o ./Lib/Framebuf/FontUbuntuBookRNormal12.su ./Lib/Framebuf/FontUbuntuBookRNormal16.cyclo ./Lib/Framebuf/FontUbuntuBookRNormal16.d ./Lib/Framebuf/FontUbuntuBookRNormal16.o ./Lib/Framebuf/FontUbuntuBookRNormal16.su ./Lib/Framebuf/font_petme128_8x8.cyclo ./Lib/Framebuf/font_petme128_8x8.d ./Lib/Framebuf/font_petme128_8x8.o ./Lib/Framebuf/font_petme128_8x8.su ./Lib/Framebuf/framebuf.cyclo ./Lib/Framebuf/framebuf.d ./Lib/Framebuf/framebuf.o ./Lib/Framebuf/framebuf.su

.PHONY: clean-Lib-2f-Framebuf

