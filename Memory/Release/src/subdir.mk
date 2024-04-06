################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/algoritmos.c \
../src/comunicacion.c \
../src/esquema.c \
../src/leerConfigMemory.c \
../src/main.c \
../src/memoriaInstrucciones.c 

C_DEPS += \
./src/algoritmos.d \
./src/comunicacion.d \
./src/esquema.d \
./src/leerConfigMemory.d \
./src/main.d \
./src/memoriaInstrucciones.d 

OBJS += \
./src/algoritmos.o \
./src/comunicacion.o \
./src/esquema.o \
./src/leerConfigMemory.o \
./src/main.o \
./src/memoriaInstrucciones.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-2c-Operaneta/Cliente-Servidor/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/algoritmos.d ./src/algoritmos.o ./src/comunicacion.d ./src/comunicacion.o ./src/esquema.d ./src/esquema.o ./src/leerConfigMemory.d ./src/leerConfigMemory.o ./src/main.d ./src/main.o ./src/memoriaInstrucciones.d ./src/memoriaInstrucciones.o

.PHONY: clean-src

