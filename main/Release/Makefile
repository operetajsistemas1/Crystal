################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../ADC.c \
../buttons.c \
../conductivity.c \
../eeprom.c \
../glcd.c \
../gpio.c \
../main.c \
../MenuTree.c \
../PWM.c \
../SNx4HC595.c \
../state.c \
../temperature.c \
../timer.c \
../uart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
ADC.o \
buttons.o \
conductivity.o \
eeprom.o \
glcd.o \
gpio.o \
main.o \
MenuTree.o \
PWM.o \
SNx4HC595.o \
state.o \
temperature.o \
timer.o \
uart.o


OBJS_AS_ARGS +=  \
ADC.o \
buttons.o \
conductivity.o \
eeprom.o \
glcd.o \
gpio.o \
main.o \
MenuTree.o \
PWM.o \
SNx4HC595.o \
state.o \
temperature.o \
timer.o \
uart.o


C_DEPS +=  \
ADC.d \
buttons.d \
conductivity.d \
eeprom.d \
glcd.d \
gpio.d \
main.d \
MenuTree.d \
PWM.d \
SNx4HC595.d \
state.d \
temperature.d \
timer.d \
uart.d


C_DEPS_AS_ARGS +=  \
ADC.d \
buttons.d \
conductivity.d \
eeprom.d \
glcd.d \
gpio.d \
main.d \
MenuTree.d \
PWM.d \
SNx4HC595.d \
state.d \
temperature.d \
timer.d \
uart.d


OUTPUT_FILE_PATH +=main.elf

OUTPUT_FILE_PATH_AS_ARGS +=main.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR32/GNU C Compiler





























./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-gcc.exe$(QUOTE)  -funsigned-char -funsigned-bitfields -O0 -fpack-struct -fshort-enums -Wall -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -mmcu=atmega32a  -o"$@" "$<" 
	@echo Finished building: $<



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU C Linker
	$(QUOTE)C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="main.map" -Wl,-lm   -mmcu=atmega32a  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "main.elf" "main.hex"
	"C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "main.elf" "main.eep" || exit 0
	"C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-objdump.exe" -h -S "main.elf" > "main.lss"
	"C:\Program Files (x86)\Atmel\AVR Studio 5.1\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-size.exe" -C --mcu=atmega32a  "main.elf"
	




# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) 
	rm -rf "main.hex" "main.lss" "main.eep" "main.map"
	