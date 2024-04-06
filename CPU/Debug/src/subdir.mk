################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cicloInstruccion.c \
../src/comunicacion.c \
../src/ejecucionInstrucciones.c \
../src/leerConfigCPU.c \
../src/limpiarCPU.c \
../src/main.c \
../src/mmu.c 

C_DEPS += \
./src/cicloInstruccion.d \
./src/comunicacion.d \
./src/ejecucionInstrucciones.d \
./src/leerConfigCPU.d \
./src/limpiarCPU.d \
./src/main.d \
./src/mmu.d 

OBJS += \
./src/cicloInstruccion.o \
./src/comunicacion.o \
./src/ejecucionInstrucciones.o \
./src/leerConfigCPU.o \
./src/limpiarCPU.o \
./src/main.o \
./src/mmu.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-2c-Operaneta/Cliente-Servidor/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/cicloInstruccion.d ./src/cicloInstruccion.o ./src/comunicacion.d ./src/comunicacion.o ./src/ejecucionInstrucciones.d ./src/ejecucionInstrucciones.o ./src/leerConfigCPU.d ./src/leerConfigCPU.o ./src/limpiarCPU.d ./src/limpiarCPU.o ./src/main.d ./src/main.o ./src/mmu.d ./src/mmu.o

.PHONY: clean-src

