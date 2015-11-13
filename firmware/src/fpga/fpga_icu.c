#include "main.h"
#include "pads.h"

#if defined(BOARD_MNU)

#include "fpga_icu.h"

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

/*
 ******************************************************************************
 * EXTERNS
 ******************************************************************************
 */
FpgaIcu FPGAICUD1;

/*
 ******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************
 */

/*
 ******************************************************************************
 * PROTOTYPES
 ******************************************************************************
 */

/*
 *******************************************************************************
 *******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************
 *******************************************************************************
 */

/*
 *******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************
 */
/**
 *
 */
void fpgaicuObjectInit(FpgaIcu *icup) {
  icup->state = FPGAICU_STOP;
}

/**
 *
 */
void fpgaicuStart(FpgaIcu *icup, const FPGADriver *fpgap) {

  osalDbgCheck(fpgap->state == FPGA_READY);

  icup->icu = fpgaGetCmdSlice(fpgap, FPGA_CMD_SLICE_ICU);
  icup->state = FPGAICU_READY;
}

/**
 *
 */
void fpgaicuStop(FpgaIcu *icup) {
  icup->state = FPGAICU_STOP;
  icup->icu = NULL;
}

/**
 *
 */
fpgacmd_t fpgaicuRead(const FpgaIcu *icup, size_t N) {
  osalDbgCheck(FPGAICU_READY == icup->state);
  //osalDbgCheck(N < FPGA_ICU_CHANNELS);

  return icup->icu[N];
}

#endif // defined(BOARD_MNU)