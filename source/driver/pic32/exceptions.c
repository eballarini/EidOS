#include <xc.h>                 /* Defines special function registers, CP0 regs  */
#include "system_config.h"
#include "system_definitions.h"
#include "system/debug/sys_debug.h"
#include <sys/eid_io.h>

/* Code identifying the cause of the exception (CP0 Cause register). */
static unsigned int _excep_code;

/* Address of instruction that caused the exception. */
static unsigned int _excep_addr;

/* Pointer to the string describing the cause of the exception. */
static char *_cause_str;

/* Array identifying the cause (indexed by _exception_code). */
static char *cause[] =
{
    "Interrupt",
    "Undefined",
    "Undefined",
    "Undefined",
    "Load/fetch address error",
    "Store address error",
    "Instruction bus error",
    "Data bus error",
    "Syscall",
    "Breakpoint",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void _general_exception_handler ( void )
{
    //unsigned int cause_got;
    /* Mask off Mask of the ExcCode Field from the Cause Register
    Refer to the MIPs Software User's manual */
    _excep_code = (_CP0_GET_CAUSE() & 0x0000007C) >> 2;
    _excep_addr = _CP0_GET_EPC();
    _cause_str  = cause[_excep_code];
    
    //cause_got = _cause_str;
    
    printf("\n\rGeneral Exception %s (cause=%d, addr=%x).\n\r", _cause_str, _excep_code, _excep_addr);
    
    SYS_DEBUG_PRINT(SYS_ERROR_FATAL, "\n\rGeneral Exception %s (cause=%d, addr=%x).\n\r", _cause_str, _excep_code, _excep_addr);

    vTaskDelete(NULL);
    
    /*while (1)
    {
        SYS_DEBUG_BreakPoint();
    }*/
}