################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MEM.c \
../src/argparse.c \
../src/connection.c \
../src/consola.c \
../src/error.c \
../src/file_conf.c \
../src/gossipingMemoria.c \
../src/i_kernel_proceso.c \
../src/inotifyMemoria.c \
../src/journaling.c \
../src/memory.c \
../src/proceso.c 

OBJS += \
./src/MEM.o \
./src/argparse.o \
./src/connection.o \
./src/consola.o \
./src/error.o \
./src/file_conf.o \
./src/gossipingMemoria.o \
./src/i_kernel_proceso.o \
./src/inotifyMemoria.o \
./src/journaling.o \
./src/memory.o \
./src/proceso.o 

C_DEPS += \
./src/MEM.d \
./src/argparse.d \
./src/connection.d \
./src/consola.d \
./src/error.d \
./src/file_conf.d \
./src/gossipingMemoria.d \
./src/i_kernel_proceso.d \
./src/inotifyMemoria.d \
./src/journaling.d \
./src/memory.d \
./src/proceso.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/projects/tp-2019-1c-one-more-time/Shared_Library" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


