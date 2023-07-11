################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Nuklear/nuklear_9slice.c \
../Nuklear/nuklear_buffer.c \
../Nuklear/nuklear_button.c \
../Nuklear/nuklear_chart.c \
../Nuklear/nuklear_color.c \
../Nuklear/nuklear_color_picker.c \
../Nuklear/nuklear_combo.c \
../Nuklear/nuklear_context.c \
../Nuklear/nuklear_contextual.c \
../Nuklear/nuklear_draw.c \
../Nuklear/nuklear_edit.c \
../Nuklear/nuklear_font.c \
../Nuklear/nuklear_group.c \
../Nuklear/nuklear_image.c \
../Nuklear/nuklear_input.c \
../Nuklear/nuklear_layout.c \
../Nuklear/nuklear_list_view.c \
../Nuklear/nuklear_math.c \
../Nuklear/nuklear_menu.c \
../Nuklear/nuklear_page_element.c \
../Nuklear/nuklear_panel.c \
../Nuklear/nuklear_pool.c \
../Nuklear/nuklear_popup.c \
../Nuklear/nuklear_progress.c \
../Nuklear/nuklear_property.c \
../Nuklear/nuklear_scrollbar.c \
../Nuklear/nuklear_selectable.c \
../Nuklear/nuklear_slider.c \
../Nuklear/nuklear_string.c \
../Nuklear/nuklear_style.c \
../Nuklear/nuklear_table.c \
../Nuklear/nuklear_text.c \
../Nuklear/nuklear_text_editor.c \
../Nuklear/nuklear_toggle.c \
../Nuklear/nuklear_tooltip.c \
../Nuklear/nuklear_tree.c \
../Nuklear/nuklear_utf8.c \
../Nuklear/nuklear_util.c \
../Nuklear/nuklear_vertex.c \
../Nuklear/nuklear_widget.c \
../Nuklear/nuklear_window.c 

OBJS += \
./Nuklear/nuklear_9slice.o \
./Nuklear/nuklear_buffer.o \
./Nuklear/nuklear_button.o \
./Nuklear/nuklear_chart.o \
./Nuklear/nuklear_color.o \
./Nuklear/nuklear_color_picker.o \
./Nuklear/nuklear_combo.o \
./Nuklear/nuklear_context.o \
./Nuklear/nuklear_contextual.o \
./Nuklear/nuklear_draw.o \
./Nuklear/nuklear_edit.o \
./Nuklear/nuklear_font.o \
./Nuklear/nuklear_group.o \
./Nuklear/nuklear_image.o \
./Nuklear/nuklear_input.o \
./Nuklear/nuklear_layout.o \
./Nuklear/nuklear_list_view.o \
./Nuklear/nuklear_math.o \
./Nuklear/nuklear_menu.o \
./Nuklear/nuklear_page_element.o \
./Nuklear/nuklear_panel.o \
./Nuklear/nuklear_pool.o \
./Nuklear/nuklear_popup.o \
./Nuklear/nuklear_progress.o \
./Nuklear/nuklear_property.o \
./Nuklear/nuklear_scrollbar.o \
./Nuklear/nuklear_selectable.o \
./Nuklear/nuklear_slider.o \
./Nuklear/nuklear_string.o \
./Nuklear/nuklear_style.o \
./Nuklear/nuklear_table.o \
./Nuklear/nuklear_text.o \
./Nuklear/nuklear_text_editor.o \
./Nuklear/nuklear_toggle.o \
./Nuklear/nuklear_tooltip.o \
./Nuklear/nuklear_tree.o \
./Nuklear/nuklear_utf8.o \
./Nuklear/nuklear_util.o \
./Nuklear/nuklear_vertex.o \
./Nuklear/nuklear_widget.o \
./Nuklear/nuklear_window.o 

C_DEPS += \
./Nuklear/nuklear_9slice.d \
./Nuklear/nuklear_buffer.d \
./Nuklear/nuklear_button.d \
./Nuklear/nuklear_chart.d \
./Nuklear/nuklear_color.d \
./Nuklear/nuklear_color_picker.d \
./Nuklear/nuklear_combo.d \
./Nuklear/nuklear_context.d \
./Nuklear/nuklear_contextual.d \
./Nuklear/nuklear_draw.d \
./Nuklear/nuklear_edit.d \
./Nuklear/nuklear_font.d \
./Nuklear/nuklear_group.d \
./Nuklear/nuklear_image.d \
./Nuklear/nuklear_input.d \
./Nuklear/nuklear_layout.d \
./Nuklear/nuklear_list_view.d \
./Nuklear/nuklear_math.d \
./Nuklear/nuklear_menu.d \
./Nuklear/nuklear_page_element.d \
./Nuklear/nuklear_panel.d \
./Nuklear/nuklear_pool.d \
./Nuklear/nuklear_popup.d \
./Nuklear/nuklear_progress.d \
./Nuklear/nuklear_property.d \
./Nuklear/nuklear_scrollbar.d \
./Nuklear/nuklear_selectable.d \
./Nuklear/nuklear_slider.d \
./Nuklear/nuklear_string.d \
./Nuklear/nuklear_style.d \
./Nuklear/nuklear_table.d \
./Nuklear/nuklear_text.d \
./Nuklear/nuklear_text_editor.d \
./Nuklear/nuklear_toggle.d \
./Nuklear/nuklear_tooltip.d \
./Nuklear/nuklear_tree.d \
./Nuklear/nuklear_utf8.d \
./Nuklear/nuklear_util.d \
./Nuklear/nuklear_vertex.d \
./Nuklear/nuklear_widget.d \
./Nuklear/nuklear_window.d 


# Each subdirectory must supply rules for building sources it contributes
Nuklear/%.o Nuklear/%.su Nuklear/%.cyclo: ../Nuklear/%.c Nuklear/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G473xx -DNK_INCLUDE_STANDARD_VARARGS -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"C:/git/black_scope/software/test_g473_blink/Nuklear" -I"C:/git/black_scope/software/test_g473_blink/tsc2046" -I"C:/git/black_scope/software/test_g473_blink/Lib/Framebuf" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Nuklear

clean-Nuklear:
	-$(RM) ./Nuklear/nuklear_9slice.cyclo ./Nuklear/nuklear_9slice.d ./Nuklear/nuklear_9slice.o ./Nuklear/nuklear_9slice.su ./Nuklear/nuklear_buffer.cyclo ./Nuklear/nuklear_buffer.d ./Nuklear/nuklear_buffer.o ./Nuklear/nuklear_buffer.su ./Nuklear/nuklear_button.cyclo ./Nuklear/nuklear_button.d ./Nuklear/nuklear_button.o ./Nuklear/nuklear_button.su ./Nuklear/nuklear_chart.cyclo ./Nuklear/nuklear_chart.d ./Nuklear/nuklear_chart.o ./Nuklear/nuklear_chart.su ./Nuklear/nuklear_color.cyclo ./Nuklear/nuklear_color.d ./Nuklear/nuklear_color.o ./Nuklear/nuklear_color.su ./Nuklear/nuklear_color_picker.cyclo ./Nuklear/nuklear_color_picker.d ./Nuklear/nuklear_color_picker.o ./Nuklear/nuklear_color_picker.su ./Nuklear/nuklear_combo.cyclo ./Nuklear/nuklear_combo.d ./Nuklear/nuklear_combo.o ./Nuklear/nuklear_combo.su ./Nuklear/nuklear_context.cyclo ./Nuklear/nuklear_context.d ./Nuklear/nuklear_context.o ./Nuklear/nuklear_context.su ./Nuklear/nuklear_contextual.cyclo ./Nuklear/nuklear_contextual.d ./Nuklear/nuklear_contextual.o ./Nuklear/nuklear_contextual.su ./Nuklear/nuklear_draw.cyclo ./Nuklear/nuklear_draw.d ./Nuklear/nuklear_draw.o ./Nuklear/nuklear_draw.su ./Nuklear/nuklear_edit.cyclo ./Nuklear/nuklear_edit.d ./Nuklear/nuklear_edit.o ./Nuklear/nuklear_edit.su ./Nuklear/nuklear_font.cyclo ./Nuklear/nuklear_font.d ./Nuklear/nuklear_font.o ./Nuklear/nuklear_font.su ./Nuklear/nuklear_group.cyclo ./Nuklear/nuklear_group.d ./Nuklear/nuklear_group.o ./Nuklear/nuklear_group.su ./Nuklear/nuklear_image.cyclo ./Nuklear/nuklear_image.d ./Nuklear/nuklear_image.o ./Nuklear/nuklear_image.su ./Nuklear/nuklear_input.cyclo ./Nuklear/nuklear_input.d ./Nuklear/nuklear_input.o ./Nuklear/nuklear_input.su ./Nuklear/nuklear_layout.cyclo ./Nuklear/nuklear_layout.d ./Nuklear/nuklear_layout.o ./Nuklear/nuklear_layout.su ./Nuklear/nuklear_list_view.cyclo ./Nuklear/nuklear_list_view.d ./Nuklear/nuklear_list_view.o ./Nuklear/nuklear_list_view.su ./Nuklear/nuklear_math.cyclo ./Nuklear/nuklear_math.d ./Nuklear/nuklear_math.o ./Nuklear/nuklear_math.su ./Nuklear/nuklear_menu.cyclo ./Nuklear/nuklear_menu.d ./Nuklear/nuklear_menu.o ./Nuklear/nuklear_menu.su ./Nuklear/nuklear_page_element.cyclo ./Nuklear/nuklear_page_element.d ./Nuklear/nuklear_page_element.o ./Nuklear/nuklear_page_element.su ./Nuklear/nuklear_panel.cyclo ./Nuklear/nuklear_panel.d ./Nuklear/nuklear_panel.o ./Nuklear/nuklear_panel.su ./Nuklear/nuklear_pool.cyclo ./Nuklear/nuklear_pool.d ./Nuklear/nuklear_pool.o ./Nuklear/nuklear_pool.su ./Nuklear/nuklear_popup.cyclo ./Nuklear/nuklear_popup.d ./Nuklear/nuklear_popup.o ./Nuklear/nuklear_popup.su ./Nuklear/nuklear_progress.cyclo ./Nuklear/nuklear_progress.d ./Nuklear/nuklear_progress.o ./Nuklear/nuklear_progress.su ./Nuklear/nuklear_property.cyclo ./Nuklear/nuklear_property.d ./Nuklear/nuklear_property.o ./Nuklear/nuklear_property.su ./Nuklear/nuklear_scrollbar.cyclo ./Nuklear/nuklear_scrollbar.d ./Nuklear/nuklear_scrollbar.o ./Nuklear/nuklear_scrollbar.su ./Nuklear/nuklear_selectable.cyclo ./Nuklear/nuklear_selectable.d ./Nuklear/nuklear_selectable.o ./Nuklear/nuklear_selectable.su ./Nuklear/nuklear_slider.cyclo ./Nuklear/nuklear_slider.d ./Nuklear/nuklear_slider.o ./Nuklear/nuklear_slider.su ./Nuklear/nuklear_string.cyclo ./Nuklear/nuklear_string.d ./Nuklear/nuklear_string.o ./Nuklear/nuklear_string.su ./Nuklear/nuklear_style.cyclo ./Nuklear/nuklear_style.d ./Nuklear/nuklear_style.o ./Nuklear/nuklear_style.su ./Nuklear/nuklear_table.cyclo ./Nuklear/nuklear_table.d ./Nuklear/nuklear_table.o ./Nuklear/nuklear_table.su ./Nuklear/nuklear_text.cyclo ./Nuklear/nuklear_text.d ./Nuklear/nuklear_text.o ./Nuklear/nuklear_text.su ./Nuklear/nuklear_text_editor.cyclo ./Nuklear/nuklear_text_editor.d ./Nuklear/nuklear_text_editor.o ./Nuklear/nuklear_text_editor.su ./Nuklear/nuklear_toggle.cyclo ./Nuklear/nuklear_toggle.d ./Nuklear/nuklear_toggle.o ./Nuklear/nuklear_toggle.su ./Nuklear/nuklear_tooltip.cyclo ./Nuklear/nuklear_tooltip.d ./Nuklear/nuklear_tooltip.o ./Nuklear/nuklear_tooltip.su ./Nuklear/nuklear_tree.cyclo ./Nuklear/nuklear_tree.d ./Nuklear/nuklear_tree.o ./Nuklear/nuklear_tree.su ./Nuklear/nuklear_utf8.cyclo ./Nuklear/nuklear_utf8.d ./Nuklear/nuklear_utf8.o ./Nuklear/nuklear_utf8.su ./Nuklear/nuklear_util.cyclo ./Nuklear/nuklear_util.d ./Nuklear/nuklear_util.o ./Nuklear/nuklear_util.su ./Nuklear/nuklear_vertex.cyclo ./Nuklear/nuklear_vertex.d ./Nuklear/nuklear_vertex.o ./Nuklear/nuklear_vertex.su ./Nuklear/nuklear_widget.cyclo ./Nuklear/nuklear_widget.d ./Nuklear/nuklear_widget.o ./Nuklear/nuklear_widget.su ./Nuklear/nuklear_window.cyclo ./Nuklear/nuklear_window.d ./Nuklear/nuklear_window.o ./Nuklear/nuklear_window.su

.PHONY: clean-Nuklear

