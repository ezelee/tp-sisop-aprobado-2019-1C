################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Compactacion.c \
../src/Conexion.c \
../src/Configuracion.c \
../src/Consola.c \
../src/Dump.c \
../src/Error.c \
../src/Filesystem.c \
../src/Hilos.c \
../src/LFS.c 

OBJS += \
./src/Compactacion.o \
./src/Conexion.o \
./src/Configuracion.o \
./src/Consola.o \
./src/Dump.o \
./src/Error.o \
./src/Filesystem.o \
./src/Hilos.o \
./src/LFS.o 

C_DEPS += \
./src/Compactacion.d \
./src/Conexion.d \
./src/Configuracion.d \
./src/Consola.d \
./src/Dump.d \
./src/Error.d \
./src/Filesystem.d \
./src/Hilos.d \
./src/LFS.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/projects/tp-2019-1c-one-more-time/Shared_Library" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


