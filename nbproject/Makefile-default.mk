#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=ADC.c App_Main.c Gestor_Bateria.c Gestor_EEPROM.c Gestor_Sensores.c Main.c PMP_LCD.c SPI.c SPI_EEPROM.c Switches.c Temporizacion.c Traps.c UART.c ../Menu_Operario.X/MenuOperario.c ../Menu_Operario.X/Menu.c Reloj_RTCC.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/ADC.o ${OBJECTDIR}/App_Main.o ${OBJECTDIR}/Gestor_Bateria.o ${OBJECTDIR}/Gestor_EEPROM.o ${OBJECTDIR}/Gestor_Sensores.o ${OBJECTDIR}/Main.o ${OBJECTDIR}/PMP_LCD.o ${OBJECTDIR}/SPI.o ${OBJECTDIR}/SPI_EEPROM.o ${OBJECTDIR}/Switches.o ${OBJECTDIR}/Temporizacion.o ${OBJECTDIR}/Traps.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/_ext/374666656/MenuOperario.o ${OBJECTDIR}/_ext/374666656/Menu.o ${OBJECTDIR}/Reloj_RTCC.o
POSSIBLE_DEPFILES=${OBJECTDIR}/ADC.o.d ${OBJECTDIR}/App_Main.o.d ${OBJECTDIR}/Gestor_Bateria.o.d ${OBJECTDIR}/Gestor_EEPROM.o.d ${OBJECTDIR}/Gestor_Sensores.o.d ${OBJECTDIR}/Main.o.d ${OBJECTDIR}/PMP_LCD.o.d ${OBJECTDIR}/SPI.o.d ${OBJECTDIR}/SPI_EEPROM.o.d ${OBJECTDIR}/Switches.o.d ${OBJECTDIR}/Temporizacion.o.d ${OBJECTDIR}/Traps.o.d ${OBJECTDIR}/UART.o.d ${OBJECTDIR}/_ext/374666656/MenuOperario.o.d ${OBJECTDIR}/_ext/374666656/Menu.o.d ${OBJECTDIR}/Reloj_RTCC.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/ADC.o ${OBJECTDIR}/App_Main.o ${OBJECTDIR}/Gestor_Bateria.o ${OBJECTDIR}/Gestor_EEPROM.o ${OBJECTDIR}/Gestor_Sensores.o ${OBJECTDIR}/Main.o ${OBJECTDIR}/PMP_LCD.o ${OBJECTDIR}/SPI.o ${OBJECTDIR}/SPI_EEPROM.o ${OBJECTDIR}/Switches.o ${OBJECTDIR}/Temporizacion.o ${OBJECTDIR}/Traps.o ${OBJECTDIR}/UART.o ${OBJECTDIR}/_ext/374666656/MenuOperario.o ${OBJECTDIR}/_ext/374666656/Menu.o ${OBJECTDIR}/Reloj_RTCC.o

# Source Files
SOURCEFILES=ADC.c App_Main.c Gestor_Bateria.c Gestor_EEPROM.c Gestor_Sensores.c Main.c PMP_LCD.c SPI.c SPI_EEPROM.c Switches.c Temporizacion.c Traps.c UART.c ../Menu_Operario.X/MenuOperario.c ../Menu_Operario.X/Menu.c Reloj_RTCC.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ128GA010
MP_LINKER_FILE_OPTION=,--script="p24FJ128GA010.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o.ok ${OBJECTDIR}/ADC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    
	
${OBJECTDIR}/App_Main.o: App_Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/App_Main.o.d 
	@${RM} ${OBJECTDIR}/App_Main.o.ok ${OBJECTDIR}/App_Main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/App_Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/App_Main.o.d" -o ${OBJECTDIR}/App_Main.o App_Main.c    
	
${OBJECTDIR}/Gestor_Bateria.o: Gestor_Bateria.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Gestor_Bateria.o.d 
	@${RM} ${OBJECTDIR}/Gestor_Bateria.o.ok ${OBJECTDIR}/Gestor_Bateria.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Gestor_Bateria.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Gestor_Bateria.o.d" -o ${OBJECTDIR}/Gestor_Bateria.o Gestor_Bateria.c    
	
${OBJECTDIR}/Gestor_EEPROM.o: Gestor_EEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Gestor_EEPROM.o.d 
	@${RM} ${OBJECTDIR}/Gestor_EEPROM.o.ok ${OBJECTDIR}/Gestor_EEPROM.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Gestor_EEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Gestor_EEPROM.o.d" -o ${OBJECTDIR}/Gestor_EEPROM.o Gestor_EEPROM.c    
	
${OBJECTDIR}/Gestor_Sensores.o: Gestor_Sensores.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Gestor_Sensores.o.d 
	@${RM} ${OBJECTDIR}/Gestor_Sensores.o.ok ${OBJECTDIR}/Gestor_Sensores.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Gestor_Sensores.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Gestor_Sensores.o.d" -o ${OBJECTDIR}/Gestor_Sensores.o Gestor_Sensores.c    
	
${OBJECTDIR}/Main.o: Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Main.o.d 
	@${RM} ${OBJECTDIR}/Main.o.ok ${OBJECTDIR}/Main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Main.o.d" -o ${OBJECTDIR}/Main.o Main.c    
	
${OBJECTDIR}/PMP_LCD.o: PMP_LCD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/PMP_LCD.o.d 
	@${RM} ${OBJECTDIR}/PMP_LCD.o.ok ${OBJECTDIR}/PMP_LCD.o.err 
	@${FIXDEPS} "${OBJECTDIR}/PMP_LCD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/PMP_LCD.o.d" -o ${OBJECTDIR}/PMP_LCD.o PMP_LCD.c    
	
${OBJECTDIR}/SPI.o: SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/SPI.o.d 
	@${RM} ${OBJECTDIR}/SPI.o.ok ${OBJECTDIR}/SPI.o.err 
	@${FIXDEPS} "${OBJECTDIR}/SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI.o.d" -o ${OBJECTDIR}/SPI.o SPI.c    
	
${OBJECTDIR}/SPI_EEPROM.o: SPI_EEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/SPI_EEPROM.o.d 
	@${RM} ${OBJECTDIR}/SPI_EEPROM.o.ok ${OBJECTDIR}/SPI_EEPROM.o.err 
	@${FIXDEPS} "${OBJECTDIR}/SPI_EEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI_EEPROM.o.d" -o ${OBJECTDIR}/SPI_EEPROM.o SPI_EEPROM.c    
	
${OBJECTDIR}/Switches.o: Switches.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Switches.o.d 
	@${RM} ${OBJECTDIR}/Switches.o.ok ${OBJECTDIR}/Switches.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Switches.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Switches.o.d" -o ${OBJECTDIR}/Switches.o Switches.c    
	
${OBJECTDIR}/Temporizacion.o: Temporizacion.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Temporizacion.o.d 
	@${RM} ${OBJECTDIR}/Temporizacion.o.ok ${OBJECTDIR}/Temporizacion.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Temporizacion.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Temporizacion.o.d" -o ${OBJECTDIR}/Temporizacion.o Temporizacion.c    
	
${OBJECTDIR}/Traps.o: Traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Traps.o.d 
	@${RM} ${OBJECTDIR}/Traps.o.ok ${OBJECTDIR}/Traps.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Traps.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Traps.o.d" -o ${OBJECTDIR}/Traps.o Traps.c    
	
${OBJECTDIR}/UART.o: UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o.ok ${OBJECTDIR}/UART.o.err 
	@${FIXDEPS} "${OBJECTDIR}/UART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/UART.o.d" -o ${OBJECTDIR}/UART.o UART.c    
	
${OBJECTDIR}/_ext/374666656/MenuOperario.o: ../Menu_Operario.X/MenuOperario.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/374666656 
	@${RM} ${OBJECTDIR}/_ext/374666656/MenuOperario.o.d 
	@${RM} ${OBJECTDIR}/_ext/374666656/MenuOperario.o.ok ${OBJECTDIR}/_ext/374666656/MenuOperario.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/374666656/MenuOperario.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/374666656/MenuOperario.o.d" -o ${OBJECTDIR}/_ext/374666656/MenuOperario.o ../Menu_Operario.X/MenuOperario.c    
	
${OBJECTDIR}/_ext/374666656/Menu.o: ../Menu_Operario.X/Menu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/374666656 
	@${RM} ${OBJECTDIR}/_ext/374666656/Menu.o.d 
	@${RM} ${OBJECTDIR}/_ext/374666656/Menu.o.ok ${OBJECTDIR}/_ext/374666656/Menu.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/374666656/Menu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/374666656/Menu.o.d" -o ${OBJECTDIR}/_ext/374666656/Menu.o ../Menu_Operario.X/Menu.c    
	
${OBJECTDIR}/Reloj_RTCC.o: Reloj_RTCC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Reloj_RTCC.o.d 
	@${RM} ${OBJECTDIR}/Reloj_RTCC.o.ok ${OBJECTDIR}/Reloj_RTCC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Reloj_RTCC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Reloj_RTCC.o.d" -o ${OBJECTDIR}/Reloj_RTCC.o Reloj_RTCC.c    
	
else
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o.ok ${OBJECTDIR}/ADC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    
	
${OBJECTDIR}/App_Main.o: App_Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/App_Main.o.d 
	@${RM} ${OBJECTDIR}/App_Main.o.ok ${OBJECTDIR}/App_Main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/App_Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/App_Main.o.d" -o ${OBJECTDIR}/App_Main.o App_Main.c    
	
${OBJECTDIR}/Gestor_Bateria.o: Gestor_Bateria.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Gestor_Bateria.o.d 
	@${RM} ${OBJECTDIR}/Gestor_Bateria.o.ok ${OBJECTDIR}/Gestor_Bateria.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Gestor_Bateria.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Gestor_Bateria.o.d" -o ${OBJECTDIR}/Gestor_Bateria.o Gestor_Bateria.c    
	
${OBJECTDIR}/Gestor_EEPROM.o: Gestor_EEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Gestor_EEPROM.o.d 
	@${RM} ${OBJECTDIR}/Gestor_EEPROM.o.ok ${OBJECTDIR}/Gestor_EEPROM.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Gestor_EEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Gestor_EEPROM.o.d" -o ${OBJECTDIR}/Gestor_EEPROM.o Gestor_EEPROM.c    
	
${OBJECTDIR}/Gestor_Sensores.o: Gestor_Sensores.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Gestor_Sensores.o.d 
	@${RM} ${OBJECTDIR}/Gestor_Sensores.o.ok ${OBJECTDIR}/Gestor_Sensores.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Gestor_Sensores.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Gestor_Sensores.o.d" -o ${OBJECTDIR}/Gestor_Sensores.o Gestor_Sensores.c    
	
${OBJECTDIR}/Main.o: Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Main.o.d 
	@${RM} ${OBJECTDIR}/Main.o.ok ${OBJECTDIR}/Main.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Main.o.d" -o ${OBJECTDIR}/Main.o Main.c    
	
${OBJECTDIR}/PMP_LCD.o: PMP_LCD.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/PMP_LCD.o.d 
	@${RM} ${OBJECTDIR}/PMP_LCD.o.ok ${OBJECTDIR}/PMP_LCD.o.err 
	@${FIXDEPS} "${OBJECTDIR}/PMP_LCD.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/PMP_LCD.o.d" -o ${OBJECTDIR}/PMP_LCD.o PMP_LCD.c    
	
${OBJECTDIR}/SPI.o: SPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/SPI.o.d 
	@${RM} ${OBJECTDIR}/SPI.o.ok ${OBJECTDIR}/SPI.o.err 
	@${FIXDEPS} "${OBJECTDIR}/SPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI.o.d" -o ${OBJECTDIR}/SPI.o SPI.c    
	
${OBJECTDIR}/SPI_EEPROM.o: SPI_EEPROM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/SPI_EEPROM.o.d 
	@${RM} ${OBJECTDIR}/SPI_EEPROM.o.ok ${OBJECTDIR}/SPI_EEPROM.o.err 
	@${FIXDEPS} "${OBJECTDIR}/SPI_EEPROM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI_EEPROM.o.d" -o ${OBJECTDIR}/SPI_EEPROM.o SPI_EEPROM.c    
	
${OBJECTDIR}/Switches.o: Switches.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Switches.o.d 
	@${RM} ${OBJECTDIR}/Switches.o.ok ${OBJECTDIR}/Switches.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Switches.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Switches.o.d" -o ${OBJECTDIR}/Switches.o Switches.c    
	
${OBJECTDIR}/Temporizacion.o: Temporizacion.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Temporizacion.o.d 
	@${RM} ${OBJECTDIR}/Temporizacion.o.ok ${OBJECTDIR}/Temporizacion.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Temporizacion.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Temporizacion.o.d" -o ${OBJECTDIR}/Temporizacion.o Temporizacion.c    
	
${OBJECTDIR}/Traps.o: Traps.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Traps.o.d 
	@${RM} ${OBJECTDIR}/Traps.o.ok ${OBJECTDIR}/Traps.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Traps.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Traps.o.d" -o ${OBJECTDIR}/Traps.o Traps.c    
	
${OBJECTDIR}/UART.o: UART.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/UART.o.d 
	@${RM} ${OBJECTDIR}/UART.o.ok ${OBJECTDIR}/UART.o.err 
	@${FIXDEPS} "${OBJECTDIR}/UART.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/UART.o.d" -o ${OBJECTDIR}/UART.o UART.c    
	
${OBJECTDIR}/_ext/374666656/MenuOperario.o: ../Menu_Operario.X/MenuOperario.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/374666656 
	@${RM} ${OBJECTDIR}/_ext/374666656/MenuOperario.o.d 
	@${RM} ${OBJECTDIR}/_ext/374666656/MenuOperario.o.ok ${OBJECTDIR}/_ext/374666656/MenuOperario.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/374666656/MenuOperario.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/374666656/MenuOperario.o.d" -o ${OBJECTDIR}/_ext/374666656/MenuOperario.o ../Menu_Operario.X/MenuOperario.c    
	
${OBJECTDIR}/_ext/374666656/Menu.o: ../Menu_Operario.X/Menu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/374666656 
	@${RM} ${OBJECTDIR}/_ext/374666656/Menu.o.d 
	@${RM} ${OBJECTDIR}/_ext/374666656/Menu.o.ok ${OBJECTDIR}/_ext/374666656/Menu.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/374666656/Menu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/374666656/Menu.o.d" -o ${OBJECTDIR}/_ext/374666656/Menu.o ../Menu_Operario.X/Menu.c    
	
${OBJECTDIR}/Reloj_RTCC.o: Reloj_RTCC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/Reloj_RTCC.o.d 
	@${RM} ${OBJECTDIR}/Reloj_RTCC.o.ok ${OBJECTDIR}/Reloj_RTCC.o.err 
	@${FIXDEPS} "${OBJECTDIR}/Reloj_RTCC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Reloj_RTCC.o.d" -o ${OBJECTDIR}/Reloj_RTCC.o Reloj_RTCC.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    p24FJ128GA010.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   p24FJ128GA010.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map"$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/EUPLA_METEO.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
