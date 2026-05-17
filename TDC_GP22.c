// #############################################################################
// #### Copyright ##############################################################
// #############################################################################

/*
 * Copyright 2024 BaSSeM
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// #############################################################################
// #### Description ############################################################
// #############################################################################

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

#include "Platform.h"

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

#ifndef DEBUG
    #define DEBUG
#endif

#ifdef DEBUG
    #undef DEBUG
#endif

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

// #############################################################################
// #### Include(s) #############################################################
// #############################################################################

#include <stddef.h>
#include <stdint.h>

#include "../../TDC_Internal.h"
#include "TDC_GP22.h"

// #############################################################################
// #### Private Macro(s) #######################################################
// #############################################################################

#define TDC_GP22_CLK                  ( 4000000.0 )          // in Hz
#define TDC_GP22_TREF                 ( 1.0 / TDC_GP22_CLK ) // in sec

#define TDC_GP22_BUFFER_SIZE_TRANSMIT ( 16 )
#define TDC_GP22_BUFFER_SIZE_RECEIVE  ( 16 )

// #############################################################################
// #### Private Type(s) ########################################################
// #############################################################################

typedef enum TDC_GP22_OpCode
{
    TDC_GP22_OpCode_Write = 0b10000000,
    TDC_GP22_OpCode_WriteRegister0 = TDC_GP22_OpCode_Write + 0,
    TDC_GP22_OpCode_WriteRegister1 = TDC_GP22_OpCode_Write + 1,
    TDC_GP22_OpCode_WriteRegister2 = TDC_GP22_OpCode_Write + 2,
    TDC_GP22_OpCode_WriteRegister3 = TDC_GP22_OpCode_Write + 3,
    TDC_GP22_OpCode_WriteRegister4 = TDC_GP22_OpCode_Write + 4,
    TDC_GP22_OpCode_WriteRegister5 = TDC_GP22_OpCode_Write + 5,
    TDC_GP22_OpCode_WriteRegister6 = TDC_GP22_OpCode_Write + 6,

    TDC_GP22_OpCode_Read = 0b10110000,
    TDC_GP22_OpCode_ReadResult0 = TDC_GP22_OpCode_Read + 0,
    TDC_GP22_OpCode_ReadResult1 = TDC_GP22_OpCode_Read + 1,
    TDC_GP22_OpCode_ReadResult2 = TDC_GP22_OpCode_Read + 2,
    TDC_GP22_OpCode_ReadResult3 = TDC_GP22_OpCode_Read + 3,
    TDC_GP22_OpCode_ReadStatus = TDC_GP22_OpCode_Read + 4,
    TDC_GP22_OpCode_ReadTest = TDC_GP22_OpCode_Read + 5,

    TDC_GP22_OpCode_ID = 0b10110111,
    TDC_GP22_OpCode_ReadPW1ST = 0b10111000,
    TDC_GP22_OpCode_WriteConfig = 0b11000000,
    TDC_GP22_OpCode_ReadConfig = 0b11110000,
    TDC_GP22_OpCode_ValidateConfig = 0b11000110,

    TDC_GP22_OpCode_Init = 0b01110000,
    TDC_GP22_OpCode_PowerOnReset = 0b01010000,

    TDC_GP22_OpCode_StartTOF = 0b00000001,
    TDC_GP22_OpCode_StartTemp = 0b00000010,
    TDC_GP22_OpCode_StartCalResonator = 0b00000011,
    TDC_GP22_OpCode_StartCalTDC = 0b00000100,
    TDC_GP22_OpCode_StartTOFRestart = 0b00000101,
    TDC_GP22_OpCode_StartTempRestart = 0b00000110,
} TDC_GP22_OpCode_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ConfigurationRegister_0
{
    uint32_t Value;

    struct
    {
        uint32_t ID0            : 8;
        uint32_t NEG_START      : 1;
        uint32_t NEG_STOP1      : 1;
        uint32_t NEG_STOP2      : 1;
        uint32_t MESSB2         : 1;
        uint32_t NO_CAL_AUTO    : 1;
        uint32_t CALIBRATE      : 1;
        uint32_t SEL_ECLK_TMP   : 1;
        uint32_t ANZ_FAKE       : 1;
        uint32_t TCYCLE         : 1;
        uint32_t ANZ_PORT       : 1;
        uint32_t START_CLKHS    : 2;
        uint32_t DIV_CLKHS      : 2;
        uint32_t ANZ_PER_CALRES : 2;
        uint32_t DIV_FIRE       : 4;
        uint32_t ANZ_FIRE       : 4;
    };
} TDC_GP22_ConfigurationRegister_0_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ConfigurationRegister_1
{
    uint32_t Value;

    struct
    {
        uint32_t ID1            : 8;
        uint32_t SEL_TSTO1      : 3;
        uint32_t SEL_TSTO2      : 3;
        uint32_t SEL_START_FIRE : 1;
        uint32_t CURR32K        : 1;
        uint32_t HITIN1         : 3;
        uint32_t HITIN2         : 3;
        uint32_t                : 1;
        uint32_t EN_FAST_INIT   : 1;
        uint32_t HIT1           : 4;
        uint32_t HIT2           : 4;
    };
} TDC_GP22_ConfigurationRegister_1_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ConfigurationRegister_2
{
    uint32_t Value;

    struct
    {
        uint32_t ID2     : 8;
        uint32_t DELVAL1 : 19;
        uint32_t RFEDGE1 : 1;
        uint32_t RFEDGE2 : 1;
        uint32_t EN_INT  : 3;
    };
} TDC_GP22_ConfigurationRegister_2_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ConfigurationRegister_3
{
    uint32_t Value;

    struct // Common
    {
        uint32_t ID3             : 8;
        uint32_t                 : 19; // Depends on `EN_FIRST_WAVE`
        uint32_t SEL_TIMO_MB2    : 2;
        uint32_t EN_ERR_VAL      : 1;
        uint32_t EN_FIRST_WAVE   : 1;
        uint32_t EN_AUTOCALC_MB2 : 1;
    };

    struct // EN_FIRST_WAVE = 0
    {
        uint32_t         : 8;
        uint32_t DELVAL2 : 19;
        uint32_t         : 5;
    };

    struct // EN_FIRST_WAVE = 1
    {
        uint32_t         : 8;
        uint32_t DELREL1 : 6;
        uint32_t DELREL2 : 6;
        uint32_t DELREL3 : 6;
        uint32_t         : 6;
    };
} TDC_GP22_ConfigurationRegister_3_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ConfigurationRegister_4
{
    uint32_t Value;

    struct // Common
    {
        uint32_t ID4 : 8;
        uint32_t     : 24; // Depends on `EN_FIRST_WAVE`
    };

    struct // EN_FIRST_WAVE = 0
    {
        uint32_t         : 8;
        uint32_t DELVAL3 : 19;
        uint32_t         : 5;
    };

    struct // EN_FIRST_WAVE = 1
    {
        uint32_t          : 8;
        uint32_t OFFS     : 5;
        uint32_t OFFSRNG1 : 1;
        uint32_t OFFSRNG2 : 1;
        uint32_t EDGE_FW  : 1;
        uint32_t DIS_PW   : 1;
        uint32_t          : 15;
    };
} TDC_GP22_ConfigurationRegister_4_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ConfigurationRegister_5
{
    uint32_t Value;

    struct
    {
        uint32_t ID5            : 8;
        uint32_t PHFIRE         : 16;
        uint32_t REPEAT_FIRE    : 3;
        uint32_t DIS_PHASESHIFT : 1;
        uint32_t EN_STARTNOISE  : 1;
        uint32_t CONF_FIRE      : 3;
    };
} TDC_GP22_ConfigurationRegister_5_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ConfigurationRegister_6
{
    uint32_t Value;

    struct
    {
        uint32_t ID6           : 8;
        uint32_t ANZ_FIRE      : 3;
        uint32_t TEMP_PORTDIR  : 1;
        uint32_t DOUBLE_RES    : 1;
        uint32_t QUAD_RES      : 1;
        uint32_t FIREO_DEF     : 1;
        uint32_t HZ60          : 1;
        uint32_t CYCLE_TOF     : 2;
        uint32_t CYCLE_TEMP    : 2;
        uint32_t START_CLKHS   : 1;
        uint32_t EN_INT        : 1;
        uint32_t TW2           : 2;
        uint32_t               : 1;
        uint32_t DA_KORR       : 4;
        uint32_t               : 1;
        uint32_t NEG_STOP_TEMP : 1;
        uint32_t EN_ANALOG     : 1;
    };
} TDC_GP22_ConfigurationRegister_6_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ResultRegister_0
{
    uint32_t Value;

    struct
    {
        uint32_t fraction : 16;
        uint32_t integer  : 16;
    };
} TDC_GP22_ResultRegister_0_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ResultRegister_1
{
    uint32_t Value;

    struct
    {
        uint32_t fraction : 16;
        uint32_t integer  : 16;
    };
} TDC_GP22_ResultRegister_1_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ResultRegister_2
{
    uint32_t Value;

    struct
    {
        uint32_t fraction : 16;
        uint32_t integer  : 16;
    };
} TDC_GP22_ResultRegister_2_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_ResultRegister_3
{
    uint32_t Value;

    struct
    {
        uint32_t fraction : 16;
        uint32_t integer  : 16;
    };
} TDC_GP22_ResultRegister_3_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_StatusRegister
{
    uint16_t Value;

    struct
    {
        uint16_t ALU_OP_PTR         : 3;
        uint16_t Number_of_Hits_Ch1 : 3;
        uint16_t Number_of_Hits_Ch2 : 3;
        uint16_t Timeout_TDC        : 1;
        uint16_t Timeout_PreCounter : 1;
        uint16_t Error_Open         : 1;
        uint16_t Error_Short        : 1;
        uint16_t EEPROM_Error       : 1;
        uint16_t EEPROM_DED         : 1; // Double Error Detection (DED)
        uint16_t EEPROM_EQ_CREG     : 1;
    };
} TDC_GP22_StatusRegister_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_TestRegister
{
    uint8_t Value;

    struct
    {
        uint8_t : 8; // mirror for TDC_ConfigurationRegister_1 highest 8 bits
    };
} TDC_GP22_TestRegister_t;

typedef union __attribute__( ( packed, aligned( 1 ) ) ) TDC_GP22_PW1ST_Register
{
    uint8_t Value;

    struct
    {
        uint8_t : 8;
    };
} TDC_GP22_PW1ST_Register_t;

typedef struct TDC_GP22_Command
{
    TDC_GP22_OpCode_t OpCode;
    uint8_t Value[];
} TDC_GP22_Command_t;

typedef struct TDC_GP22_Response
{
    TDC_GP22_OpCode_t OpCode;
    uint8_t Value[];
} TDC_GP22_Response_t;

/**
 * @brief TDC GP22 Operation Type
 *
 * @enum TDC_GP22_OperationType_t
 */
typedef enum TDC_GP22_OperationType
{
    TDC_GP22_OperationType_None = 0,            ///< None
    TDC_GP22_OperationType_Pending,             ///< Pending
    TDC_GP22_OperationType_PowerOff,            ///< Power Off
    TDC_GP22_OperationType_PowerOn,             ///< Power On
    TDC_GP22_OperationType_TestWrite,           ///< Write Test
    TDC_GP22_OperationType_TestRead,            ///< Read Test
    TDC_GP22_OperationType_CommitRegister_0,    ///< Commit Register 0
    TDC_GP22_OperationType_CommitRegister_1,    ///< Commit Register 1
    TDC_GP22_OperationType_CommitRegister_2,    ///< Commit Register 2
    TDC_GP22_OperationType_CommitRegister_3,    ///< Commit Register 3
    TDC_GP22_OperationType_CommitRegister_4,    ///< Commit Register 4
    TDC_GP22_OperationType_CommitRegister_5,    ///< Commit Register 5
    TDC_GP22_OperationType_CommitRegister_6,    ///< Commit Register 6
    TDC_GP22_OperationType_Init,                ///< Init
    TDC_GP22_OperationType_TimeOfFlightRestart, ///< TOF Restart
    TDC_GP22_OperationType_Interrupt,           ///< Handle Interrupt
    TDC_GP22_OperationType_StatusRead,          ///< Read Status
    TDC_GP22_OperationType_Measurement_0_Read,  ///< Read Measurement 0
    TDC_GP22_OperationType_Measurement_1_Read,  ///< Read Measurement 1
    TDC_GP22_OperationType_Measurement_2_Read,  ///< Read Measurement 2
    TDC_GP22_OperationType_Measurement_3_Read,  ///< Read Measurement 3
} TDC_GP22_OperationType_t;

/**
 * @brief TDC GP22 Operation Handler
 */
typedef TDC_GP22_Status_t ( *TDC_GP22_OperationHandler_t )( TDC_GP22_Instance_t * Instance );

/**
 * @brief TDC GP22 Operation Context
 *
 * @struct TDC_GP22_OperationContext_t
 */
typedef struct TDC_GP22_OperationContext
{
    union
    {
        struct
        {
            uint32_t write_value;
            uint32_t read_value;
        };

        struct
        {
            uint32_t ID;
            TDC_GP22_FireDirection_t FireDirection;
            TDC_GP22_OperationalStatus_t OperationalStatus;
            TDC_GP22_Measurement_t Measurement_0;
            TDC_GP22_Measurement_t Measurement_1;
            TDC_GP22_Measurement_t Measurement_2;
            TDC_GP22_Measurement_t Measurement_3;
        };
    };
} TDC_GP22_OperationContext_t;

/**
 * @brief TDC GP22 Operation
 *
 * @struct TDC_GP22_Operation_t
 */
typedef struct TDC_GP22_Operation
{
    TDC_GP22_OperationType_t Type;       ///< Type
    TDC_GP22_OperationHandler_t Handler; ///< Handler
    TDC_GP22_Status_t Status;            ///< Status
    TIM_Timestamp_t Timeout;             ///< Timeout
    TDC_GP22_OperationContext_t Context; ///< Context
} TDC_GP22_Operation_t;

/**
 * @brief TDC GP22 Process Type
 *
 * @enum TDC_GP22_ProcessType_t
 */
typedef enum TDC_GP22_ProcessType
{
    TDC_GP22_ProcessType_None = 0,            ///< None
    TDC_GP22_ProcessType_Initialize,          ///< Initialize
    TDC_GP22_ProcessType_TimeOfFlightRestart, ///< ToF Restart
} TDC_GP22_ProcessType_t;

/**
 * @brief TDC GP22 Process Handler
 */
typedef TDC_GP22_Status_t ( *TDC_GP22_ProcessHandler_t )( TDC_GP22_Instance_t * Instance );

/**
 * @brief TDC GP22 Process Context
 *
 * @struct TDC_GP22_ProcessContext_t
 */
typedef struct TDC_GP22_ProcessContext
{
    TDC_GP22_Operation_t Operation; ///< Operation
} TDC_GP22_ProcessContext_t;

/**
 * @brief TDC GP22 Process
 *
 * @struct TDC_GP22_Process_t
 */
typedef struct TDC_GP22_Process
{
    TDC_GP22_ProcessType_t Type;       ///< Type
    TDC_GP22_ProcessHandler_t Handler; ///< Handler
    TDC_GP22_Status_t Status;          ///< Status
    TDC_GP22_ProcessContext_t Context; ///< Context
} TDC_GP22_Process_t;

typedef struct TDC_GP22_BufferTransmit
{
    uint32_t Length;
    uint8_t Content[ TDC_GP22_BUFFER_SIZE_TRANSMIT ];
} TDC_GP22_BufferTransmit_t;

typedef struct TDC_GP22_BufferReceive
{
    uint32_t Length;
    uint8_t Content[ TDC_GP22_BUFFER_SIZE_RECEIVE ];
} TDC_GP22_BufferReceive_t;

typedef enum TDC_GP22_Event
{
    TDC_GP22_Event_None = 0,
    TDC_GP22_Event_Interrupt = UTIL_BIT( 0 ),
    TDC_GP22_Event_Timeout = UTIL_BIT( 1 ),
    TDC_GP22_Event_SPI_Success = UTIL_BIT( 2 ),
    TDC_GP22_Event_SPI_Error = UTIL_BIT( 3 ),
} TDC_GP22_Event_t;

typedef struct TDC_GP22_InstanceContext
{
    TDC_GP22_Instance_t * Instance; // Owner Instance

    TDC_GP22_ConfigurationRegister_0_t ConfigurationRegister_0;
    TDC_GP22_ConfigurationRegister_1_t ConfigurationRegister_1;
    TDC_GP22_ConfigurationRegister_2_t ConfigurationRegister_2;
    TDC_GP22_ConfigurationRegister_3_t ConfigurationRegister_3;
    TDC_GP22_ConfigurationRegister_4_t ConfigurationRegister_4;
    TDC_GP22_ConfigurationRegister_5_t ConfigurationRegister_5;
    TDC_GP22_ConfigurationRegister_6_t ConfigurationRegister_6;

    TDC_GP22_StatusRegister_t StatusRegister;

    TDC_GP22_ResultRegister_0_t Result_Register_0;
    TDC_GP22_ResultRegister_1_t Result_Register_1;
    TDC_GP22_ResultRegister_2_t Result_Register_2;
    TDC_GP22_ResultRegister_3_t Result_Register_3;

    TDC_GP22_BufferTransmit_t Transmit;
    TDC_GP22_BufferReceive_t Receive;

    TDC_GP22_Event_t Event;

    TDC_GP22_Process_t Process;
} TDC_GP22_InstanceContext_t;

typedef struct TDC_GP22_Context
{
    TIM_Timestamp_t Timestamp;
    TDC_GP22_InstanceContext_t Context[ TDC_GP22_Count ];
} TDC_GP22_Context_t;

// #############################################################################
// #### Private Method(s) Prototype ############################################
// #############################################################################

static GPIO_Status_t GPIO_CallbackOnInterrupt( GPIO_t GPIOx, GPIO_ContextOnInterrupt_t * Context );
static SPI_Status_t SPI_CallbackOnComplete( SPI_t SPIx, SPI_Status_t Status );

static TDC_GP22_Status_t TDC_GP22_Context_Initialize( void );
static TDC_GP22_Status_t TDC_GP22_Context_Cycle( void );
static TDC_GP22_Status_t TDC_GP22_Context_DeInitialize( void );

static TDC_GP22_Status_t TDC_GP22_Instance_Initialize( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_Instance_Cycle( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_Instance_DeInitialize( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_SetProcess( TDC_GP22_Instance_t * Instance, TDC_GP22_ProcessType_t ProcessType );

// TODO Enhance the following
static TDC_GP22_Status_t TDC_GP22_Write( TDC_GP22_Instance_t * Instance, uint8_t address, uint8_t * buffer, uint32_t length );
static TDC_GP22_Status_t TDC_GP22_Read( TDC_GP22_Instance_t * Instance, uint8_t address, uint8_t * buffer, uint32_t length );

static TDC_GP22_Status_t TDC_GP22_ProcessInitialize( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_ProcessTimeOfFlightRestart( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationPowerOffExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationPowerOffResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationPowerOnExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationPowerOnResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationTestWriteExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationTestWriteResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationTestReadExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationTestReadResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_0_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_0_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_1_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_1_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_2_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_2_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_3_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_3_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_4_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_4_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_5_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_5_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_6_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_6_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationInitExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationInitResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationTimeOfFlightRestartExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationTimeOfFlightRestartResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationInterruptExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationInterruptResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationStatusReadExecute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationStatusReadResolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_0_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_0_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_1_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_1_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_2_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_2_Resolve( TDC_GP22_Instance_t * Instance );

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_3_Execute( TDC_GP22_Instance_t * Instance );
static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_3_Resolve( TDC_GP22_Instance_t * Instance );

// #############################################################################
// #### Private Variable(s) ####################################################
// #############################################################################

static TDC_GP22_Context_t TDC_GP22_Context;

// #############################################################################
// #### Private Method(s) ######################################################
// #############################################################################

static GPIO_Status_t GPIO_CallbackOnInterrupt( GPIO_t GPIOx, GPIO_ContextOnInterrupt_t * Context )
{
    GPIO_Status_t GPIO_Status = GPIO_Status_Success;

    do
    {
        TDC_Debug( "%s%s( GPIOx=%d, Context=%p )", UTIL_StringConcatenateConstant( UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionColorForegroundBlack ), UTIL_CSI_SelectGraphicRendition( UTIL_CSI_SelectGraphicRenditionColorBackgroundCyan ) ), __FUNCTION__, GPIOx, Context );

        TDC_GP22_Instance_t * Instance = Context;
        TDC_GP22_Process_t * Process = &Instance->Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        Instance->Context->Event |= TDC_GP22_Event_Interrupt;

        // TODO Initiate Opposite Direction For Restart Process
    }
    while ( 0 );

    return GPIO_Status;
}

static SPI_Status_t SPI_CallbackOnComplete( SPI_t SPIx, SPI_Status_t Status )
{
    SPI_Status_t SPI_Status = SPI_Status_Success;

    do
    {
        TDC_Debug( "%s( SPIx=%d, Status=%p )", __FUNCTION__, SPIx, Status );

        // FIXME Enhance the following
        TDC_GP22_InstanceContext_t * Context = NULL;
        for ( TDC_GP22_t GP22_x = TDC_GP22_1; GP22_x < TDC_GP22_Count; ++GP22_x )
        {
            Context = &TDC_GP22_Context.Context[ GP22_x ];
            if ( Context->Instance->SPIx == SPIx )
            {
                break;
            }

            Context = NULL;
        }
        if ( Context == NULL )
        {
            break;
        }

        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        switch ( Status )
        {
            case SPI_Status_Success:
                Context->Event |= TDC_GP22_Event_SPI_Success;
                break;

            default:
                Context->Event |= TDC_GP22_Event_SPI_Error;
                break;
        }

        GPIO_Status_t GPIO_Status = GPIO_Status_Success;
        if ( ( GPIO_Status = GPIO_Write( Context->Instance->ChipSelect, GPIO_Value_High ) ) != GPIO_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return SPI_Status;
}

static TDC_GP22_Status_t TDC_GP22_Context_Initialize( void )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( void )", __FUNCTION__ );

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_GetTimestamp( TDC_TIM, &TDC_GP22_Context.Timestamp ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_Context_Cycle( void )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( void )", __FUNCTION__ );

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_GetTimestamp( TDC_TIM, &TDC_GP22_Context.Timestamp ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_Context_DeInitialize( void )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( void )", __FUNCTION__ );
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_Instance_Initialize( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        // TODO Enhance GPIOs Configuration

        GPIO_Status_t GPIO_Status = GPIO_Status_Success;
        if ( ( GPIO_Status = GPIO_Write( Instance->PowerEnable, GPIO_Value_High ) ) != GPIO_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        if ( ( GPIO_Status = GPIO_SetMode( Instance->Interrupt, GPIO_Mode_InterruptFalling ) ) != GPIO_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        if ( ( GPIO_Status = GPIO_SetCallbackOnInterrupt( Instance->Interrupt, GPIO_CallbackOnInterrupt, Instance ) ) != GPIO_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        SPI_Status_t SPI_Status = SPI_Status_Success;
        if ( ( SPI_Status = SPI_SetCallbackOnComplete( Instance->SPIx, SPI_CallbackOnComplete ) ) != SPI_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];

        Context->Instance = Instance;

        // Set Defaults
        Context->ConfigurationRegister_0.Value = 0b00100010000001100110100000000000;
        Context->ConfigurationRegister_1.Value = 0b01010101010000000000000000000000;
        Context->ConfigurationRegister_2.Value = 0b00100000000000000000000000000000;
        Context->ConfigurationRegister_3.Value = 0b00011000000000000000000000000000;
        Context->ConfigurationRegister_4.Value = 0b00100000000000000000000000000000;
        Context->ConfigurationRegister_5.Value = 0b00000000000000000000000000000000;
        Context->ConfigurationRegister_6.Value = 0b00000000000000000000000000000000;

        Context->Transmit.Length = 0;
        Context->Transmit.Content[ Context->Transmit.Length ] = 0;

        Context->Receive.Length = 0;
        Context->Receive.Content[ Context->Receive.Length ] = 0;

        Context->Event = TDC_GP22_Event_None;

        Instance->Context = Context;

        Status = TDC_GP22_SetProcess( Instance, TDC_GP22_ProcessType_Initialize );
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_Instance_Cycle( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;
        TDC_GP22_Event_t Event = Context->Event; // CAUTION: Has to copy events occurred at the early start of the cycle, so as to be cleared at the end of the cycle,
                                                 //          which let events occurs after that for the next cycle call

        if ( Operation->Handler != NULL )
        {
            TDC_GP22_Status_t GP22_Status = TDC_GP22_Status_Error;
            if ( ( GP22_Status = Operation->Handler( Instance ) ) != TDC_GP22_Status_Success )
            {
                Status = GP22_Status;
                // FIXME Operation reported non success status, is there any action ?
            }
        }

        if ( Process->Handler != NULL )
        {
            TDC_GP22_Status_t GP22_Status = TDC_GP22_Status_Error;
            if ( ( GP22_Status = Process->Handler( Instance ) ) != TDC_GP22_Status_Success )
            {
                Status = GP22_Status;
                // FIXME Process reported non success status, is there any action ?
            }
        }

        if ( ( Event & TDC_GP22_Event_Interrupt ) == TDC_GP22_Event_Interrupt )
        {
            Context->Event &= ~TDC_GP22_Event_Interrupt;
            TDC_Trace( "Interrupt: Instance=%p, GP22x=%d", Instance, Instance->GP22x );
            // TODO Invoke Callback
        }

        if ( ( Event & TDC_GP22_Event_Timeout ) == TDC_GP22_Event_Timeout )
        {
            Context->Event &= ~TDC_GP22_Event_Timeout;
            TDC_Debug( "Timeout: Instance=%p, GP22x=%d", Instance, Instance->GP22x );
            // TODO Invoke Callback
        }

        if ( ( Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Context->Event &= ~TDC_GP22_Event_SPI_Success;
            TDC_Debug( "SPI Success: Instance=%p, GP22x=%d", Instance, Instance->GP22x );
            // TODO Invoke Callback
        }

        if ( ( Event & TDC_GP22_Event_SPI_Error ) == TDC_GP22_Event_SPI_Error )
        {
            Context->Event &= ~TDC_GP22_Event_SPI_Error;
            TDC_Debug( "SPI Error: Instance=%p, GP22x=%d", Instance, Instance->GP22x );
            // TODO Invoke Callback
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_Instance_DeInitialize( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_SetProcess( TDC_GP22_Instance_t * Instance, TDC_GP22_ProcessType_t ProcessType )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p, ProcessType=%d )", __FUNCTION__, Instance, ProcessType );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        switch ( ProcessType )
        {
            case TDC_GP22_ProcessType_None:
                Process->Handler = NULL;
                break;

            case TDC_GP22_ProcessType_Initialize:
                Process->Handler = TDC_GP22_ProcessInitialize;
                break;

            case TDC_GP22_ProcessType_TimeOfFlightRestart:
                Process->Handler = TDC_GP22_ProcessTimeOfFlightRestart;
                break;

            default:
                TDC_Warning( "%s Not Handled Type %d", __FUNCTION__, ProcessType );
                Status = TDC_GP22_Status_NotSupported;
                break;
        }
        if ( Status != TDC_GP22_Status_Success )
        {
            break;
        }

        Process->Type = ProcessType;
        Process->Status = TDC_GP22_Status_Success;

        Operation->Handler = NULL;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        switch ( ProcessType )
        {
            case TDC_GP22_ProcessType_None:
                Operation->Type = TDC_GP22_OperationType_None;
                break;

            default:
                Operation->Type = TDC_GP22_OperationType_Pending;
                break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_Write( TDC_GP22_Instance_t * Instance, uint8_t address, uint8_t * buffer, uint32_t length )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p, address=%02X, buffer=%p, length=%d )", __FUNCTION__, Instance, address, buffer, length );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];

        Context->Transmit.Length = 0;
        Context->Receive.Length = 0;

        Context->Transmit.Content[ Context->Transmit.Length ] = address;
        Context->Transmit.Length++;
        UTIL_MemoryCopy( Context->Transmit.Content + Context->Transmit.Length, buffer, length );
        UTIL_MemoryReverse( Context->Transmit.Content + Context->Transmit.Length, length ); // TDC-GP22 expects MSB first, STM32 is little-endian. So, reverse
        Context->Transmit.Length += length;

        do
        {
            GPIO_Status_t GPIO_Status = GPIO_Status_Success;
            if ( ( GPIO_Status = GPIO_Write( Instance->ChipSelect, GPIO_Value_Low ) ) != GPIO_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }

            SPI_Status_t SPI_Status = SPI_Status_Success;
            if ( ( SPI_Status = SPI_Write( Instance->SPIx, Context->Transmit.Content, Context->Transmit.Length ) ) != SPI_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }
        }
        while ( 0 );
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_Read( TDC_GP22_Instance_t * Instance, uint8_t address, uint8_t * buffer, uint32_t length )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p, address=%02X, buffer=%p, length=%d )", __FUNCTION__, Instance, address, buffer, length );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];

        Context->Transmit.Length = 0;
        Context->Receive.Length = 0;

        Context->Transmit.Content[ Context->Transmit.Length ] = address;
        Context->Transmit.Length++;
        Context->Receive.Length += UTIL_SizeOf( TDC_GP22_OpCode_t ) + length;

        do
        {
            GPIO_Status_t GPIO_Status = GPIO_Status_Error;
            if ( ( GPIO_Status = GPIO_Write( Instance->ChipSelect, GPIO_Value_Low ) ) != GPIO_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }

            SPI_Status_t SPI_Status = SPI_Status_Error;
            if ( ( SPI_Status = SPI_Transaction( Instance->SPIx, Context->Transmit.Content, Context->Transmit.Length, Context->Receive.Content, Context->Receive.Length ) ) != SPI_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }
        }
        while ( 0 );
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_ProcessInitialize( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != TDC_GP22_ProcessType_Initialize )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Process->Type, TDC_GP22_ProcessType_Initialize );
            Status = TDC_GP22_Status_Error;
            break;
        }

        if ( Operation->Handler != NULL )
        {
            // Operation In-progress
            break;
        }

        if ( Operation->Status != TDC_GP22_Status_Success )
        {
            Operation->Type = TDC_GP22_OperationType_None;
        }

        switch ( Operation->Type )
        {
            case TDC_GP22_OperationType_Pending:
                Operation->Status = TDC_GP22_OperationPowerOffExecute( Instance );
                break;

            case TDC_GP22_OperationType_PowerOff:
                Operation->Status = TDC_GP22_OperationPowerOnExecute( Instance );
                break;

            case TDC_GP22_OperationType_PowerOn:
                Operation->Status = TDC_GP22_OperationTestWriteExecute( Instance );
                break;

            case TDC_GP22_OperationType_TestWrite:
                Operation->Status = TDC_GP22_OperationTestReadExecute( Instance );
                break;

            case TDC_GP22_OperationType_TestRead:
                Operation->Status = TDC_GP22_OperationCommitRegister_0_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_0:
                Operation->Status = TDC_GP22_OperationCommitRegister_1_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_1:
                Operation->Status = TDC_GP22_OperationCommitRegister_2_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_2:
                Operation->Status = TDC_GP22_OperationCommitRegister_3_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_3:
                Operation->Status = TDC_GP22_OperationCommitRegister_4_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_4:
                Operation->Status = TDC_GP22_OperationCommitRegister_5_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_5:
                Operation->Status = TDC_GP22_OperationCommitRegister_6_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_6:
                Operation->Status = TDC_GP22_OperationInitExecute( Instance );
                break;

            case TDC_GP22_OperationType_Init:
            default:
                if ( Instance->OnComplete != NULL )
                {
                    Instance->OnComplete( Instance, Operation->Status );
                }
                Status = TDC_GP22_SetProcess( Instance, TDC_GP22_ProcessType_None );
                break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_ProcessTimeOfFlightRestart( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != TDC_GP22_ProcessType_TimeOfFlightRestart )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Process->Type, TDC_GP22_ProcessType_TimeOfFlightRestart );
            Status = TDC_GP22_Status_Error;
            break;
        }

        if ( Operation->Handler != NULL )
        {
            // Operation In-progress
            break;
        }

        if ( Operation->Status != TDC_GP22_Status_Success )
        {
            Process->Status = Operation->Status;
            Operation->Type = TDC_GP22_OperationType_None;
        }

        switch ( Operation->Type )
        {
            case TDC_GP22_OperationType_Pending:
                Operation->Context.ID = 0;
                // FIXME Note: Hot Workaround Fix For Time-out Hanging Issue, Might Be Power Issue
#if 1
                Operation->Status = TDC_GP22_OperationPowerOffExecute( Instance );
                break;

            case TDC_GP22_OperationType_PowerOff:
                Operation->Status = TDC_GP22_OperationPowerOnExecute( Instance );
                break;

            case TDC_GP22_OperationType_PowerOn:
                Operation->Status = TDC_GP22_OperationCommitRegister_0_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_0:
                Operation->Status = TDC_GP22_OperationCommitRegister_1_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_1:
                Operation->Status = TDC_GP22_OperationCommitRegister_2_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_2:
                Operation->Status = TDC_GP22_OperationCommitRegister_3_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_3:
                Operation->Status = TDC_GP22_OperationCommitRegister_4_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_4:
                Operation->Status = TDC_GP22_OperationCommitRegister_5_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_5:
                Operation->Status = TDC_GP22_OperationCommitRegister_6_Execute( Instance );
                break;

            case TDC_GP22_OperationType_CommitRegister_6:
                Operation->Status = TDC_GP22_OperationInitExecute( Instance );
                break;

            case TDC_GP22_OperationType_Init:
#endif
                if ( Operation->Context.ID == 0 )
                {
                    Operation->Context.ID = 1;
                    // First Init Should execute the following
                    Operation->Status = TDC_GP22_OperationTimeOfFlightRestartExecute( Instance );
                }
                else if ( Operation->Context.ID == 1 )
                {
                    Operation->Context.ID = 2;
                    // Intermediate init should execute the following
                    Operation->Status = TDC_GP22_OperationInterruptExecute( Instance );
                }
                else
                {
                    // last init should terminate the process
                    Operation->Type = TDC_GP22_OperationType_None;
                }
                break;

            case TDC_GP22_OperationType_TimeOfFlightRestart:
                Operation->Status = TDC_GP22_OperationInterruptExecute( Instance );
                break;

            case TDC_GP22_OperationType_Interrupt:
                Operation->Status = TDC_GP22_OperationStatusReadExecute( Instance );
                break;

            case TDC_GP22_OperationType_StatusRead:
                TDC_GP22_OperationalStatus_t * OperationalStatus = &Operation->Context.OperationalStatus;
                if ( OperationalStatus->Timeout_TDC || OperationalStatus->Timeout_PreCounter )
                {
                    Process->Status = TDC_GP22_Status_Timeout;
                }
                Operation->Status = TDC_GP22_OperationMeasurement_0_Execute( Instance );
                break;

            case TDC_GP22_OperationType_Measurement_0_Read:
                Operation->Status = TDC_GP22_OperationMeasurement_1_Execute( Instance );
                break;

            case TDC_GP22_OperationType_Measurement_1_Read:
                Operation->Status = TDC_GP22_OperationMeasurement_2_Execute( Instance );
                break;

            case TDC_GP22_OperationType_Measurement_2_Read:
                Operation->Status = TDC_GP22_OperationMeasurement_3_Execute( Instance );
                break;

            case TDC_GP22_OperationType_Measurement_3_Read:
                Operation->Status = TDC_GP22_OperationInitExecute( Instance );

                switch ( Operation->Context.FireDirection )
                {
                    case TDC_GP22_FireDirection_Up:
                        Operation->Context.FireDirection = TDC_GP22_FireDirection_Down;
                        break;

                    case TDC_GP22_FireDirection_Down:
                        Operation->Context.FireDirection = TDC_GP22_FireDirection_Up;
                        break;

                    case TDC_GP22_FireDirection_Both:
                    default:
                        // FIXME Start TOF for Both Directions ?!
                        break;
                }
                break;

            default:
                if ( Instance->OnComplete != NULL )
                {
                    Instance->OnComplete( Instance, Process->Status );
                }
                Status = TDC_GP22_SetProcess( Instance, TDC_GP22_ProcessType_None );
                break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationPowerOffExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        GPIO_Status_t GPIO_Status = GPIO_Status_Success;
        if ( ( GPIO_Status = GPIO_Write( Instance->Reset, GPIO_Value_Low ) ) != GPIO_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_PowerOff;
        Operation->Handler = TDC_GP22_OperationPowerOffResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 10 ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationPowerOffResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_PowerOff )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_PowerOff );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationPowerOnExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        GPIO_Status_t GPIO_Status = GPIO_Status_Success;
        if ( ( GPIO_Status = GPIO_Write( Instance->Reset, GPIO_Value_High ) ) != GPIO_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_PowerOn;
        Operation->Handler = TDC_GP22_OperationPowerOnResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 10 ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationPowerOnResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_PowerOn )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_PowerOn );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationTestWriteExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        Operation->Context.write_value = 0xA5000000;
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister1, ( uint8_t * ) &Operation->Context.write_value, UTIL_SizeOf( Operation->Context.write_value ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_TestWrite;
        Operation->Handler = TDC_GP22_OperationTestWriteResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationTestWriteResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_TestWrite )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_TestWrite );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationTestReadExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        Operation->Context.read_value = 0x00000000;
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadTest, ( uint8_t * ) &Operation->Context.read_value, UTIL_SizeOf( Operation->Context.read_value ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_TestRead;
        Operation->Handler = TDC_GP22_OperationTestReadResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationTestReadResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_TestRead )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_TestRead );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            TDC_GP22_Response_t * Response = ( TDC_GP22_Response_t * ) Context->Receive.Content;
            UTIL_MemoryReverse( Response->Value, UTIL_SizeOf( Operation->Context.read_value ) ); // TDC-GP22 sends MSB first, STM32 is little-endian. So, reverse
            UTIL_MemoryCopy( &Operation->Context.read_value, Response->Value, UTIL_SizeOf( Operation->Context.read_value ) );
            Context->Receive.Length -= UTIL_SizeOf( TDC_GP22_OpCode_t ) + UTIL_SizeOf( Operation->Context.read_value );
            Context->Transmit.Length--;

            if ( Operation->Context.read_value == Operation->Context.write_value )
            {
                Operation->Status = TDC_GP22_Status_Success;
            }
            else
            {
                Operation->Status = TDC_GP22_Status_Error;
            }

            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_0_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister0, ( uint8_t * ) &Instance->Context->ConfigurationRegister_0, UTIL_SizeOf( Instance->Context->ConfigurationRegister_0 ) ) ) != TDC_GP22_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_CommitRegister_0;
        Operation->Handler = TDC_GP22_OperationCommitRegister_0_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_0_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_CommitRegister_0 )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_CommitRegister_0 );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_1_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister1, ( uint8_t * ) &Instance->Context->ConfigurationRegister_1, UTIL_SizeOf( Instance->Context->ConfigurationRegister_1 ) ) ) != TDC_GP22_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_CommitRegister_1;
        Operation->Handler = TDC_GP22_OperationCommitRegister_1_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_1_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_CommitRegister_1 )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_CommitRegister_1 );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_2_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister2, ( uint8_t * ) &Instance->Context->ConfigurationRegister_2, UTIL_SizeOf( Instance->Context->ConfigurationRegister_2 ) ) ) != TDC_GP22_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_CommitRegister_2;
        Operation->Handler = TDC_GP22_OperationCommitRegister_2_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_2_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_CommitRegister_2 )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_CommitRegister_2 );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_3_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister3, ( uint8_t * ) &Instance->Context->ConfigurationRegister_3, UTIL_SizeOf( Instance->Context->ConfigurationRegister_3 ) ) ) != TDC_GP22_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_CommitRegister_3;
        Operation->Handler = TDC_GP22_OperationCommitRegister_3_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_3_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_CommitRegister_3 )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_CommitRegister_3 );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_4_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister4, ( uint8_t * ) &Instance->Context->ConfigurationRegister_4, UTIL_SizeOf( Instance->Context->ConfigurationRegister_4 ) ) ) != TDC_GP22_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_CommitRegister_4;
        Operation->Handler = TDC_GP22_OperationCommitRegister_4_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_4_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_CommitRegister_4 )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_CommitRegister_4 );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_5_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister5, ( uint8_t * ) &Instance->Context->ConfigurationRegister_5, UTIL_SizeOf( Instance->Context->ConfigurationRegister_5 ) ) ) != TDC_GP22_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_CommitRegister_5;
        Operation->Handler = TDC_GP22_OperationCommitRegister_5_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_5_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_CommitRegister_5 )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_CommitRegister_5 );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_6_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister6, ( uint8_t * ) &Instance->Context->ConfigurationRegister_6, UTIL_SizeOf( Instance->Context->ConfigurationRegister_6 ) ) ) != TDC_GP22_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        Operation->Type = TDC_GP22_OperationType_CommitRegister_6;
        Operation->Handler = TDC_GP22_OperationCommitRegister_6_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationCommitRegister_6_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_CommitRegister_6 )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_CommitRegister_6 );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationInitExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_Init, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_Init;
        Operation->Handler = TDC_GP22_OperationInitResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationInitResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_Init )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_Init );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationTimeOfFlightRestartExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        Operation->Context.FireDirection = Context->ConfigurationRegister_5.CONF_FIRE;

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_StartTOFRestart, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_TimeOfFlightRestart;
        Operation->Handler = TDC_GP22_OperationTimeOfFlightRestartResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationTimeOfFlightRestartResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_TimeOfFlightRestart )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_TimeOfFlightRestart );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }

#if 0 // Got handled by separate operations
        if ( ( Context->Event & TDC_GP22_Event_Interrupt ) != TDC_GP22_Event_Interrupt )
        {
            // Interrupt Not Yet Received
            if ( Operation->Status == TDC_GP22_Status_Success )
            {
                // Operation Still In Progress, So Skip For Now
                break;
            }
        }

        TDC_GP22_FireDirection_t FireDirection = Operation->Context.FireDirection;
        TDC_GP22_Measurement_t Measurement;
        TDC_GP22_OperationalStatus_t OperationalStatus;
        TDC_GP22_Status_t GP22_Status = TDC_GP22_Status_Error;
        if ( ( GP22_Status = TDC_GP22_GetOperationalStatus( Instance, &OperationalStatus ) ) != TDC_GP22_Status_Success )
        {
            Status = GP22_Status;
            break;
        }

        if ( OperationalStatus.Timeout_PreCounter || OperationalStatus.Timeout_TDC )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            // MUST CONTINUE till reach operation completion
        }

        if ( Instance->OnMeasurement_0 != NULL )
        {
            if ( ( GP22_Status = TDC_GP22_GetMeasurement_0( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
            {
                Status = GP22_Status;
                break;
            }
            Instance->OnMeasurement_0( Instance, FireDirection, Measurement );
        }

        if ( Instance->OnMeasurement_1 != NULL )
        {
            if ( ( GP22_Status = TDC_GP22_GetMeasurement_1( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
            {
                Status = GP22_Status;
                break;
            }
            Instance->OnMeasurement_1( Instance, FireDirection, Measurement );
        }

        if ( Instance->OnMeasurement_2 != NULL )
        {
            if ( ( GP22_Status = TDC_GP22_GetMeasurement_2( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
            {
                Status = GP22_Status;
                break;
            }
            Instance->OnMeasurement_2( Instance, FireDirection, Measurement );
        }

        if ( Instance->OnMeasurement_3 != NULL )
        {
            if ( ( GP22_Status = TDC_GP22_GetMeasurement_3( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
            {
                Status = GP22_Status;
                break;
            }
            Instance->OnMeasurement_3( Instance, FireDirection, Measurement );
        }

        if ( ( GP22_Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_Init, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            Status = GP22_Status;
            break;
        }

        TDC_Debug( "" );
        TDC_Debug( "Direction [%d] Status=%d", FireDirection, Operation->Status );

        TDC_Debug( "... ALU_Operation_Pointer=%d", OperationalStatus.ALU_Operation_Pointer );
        TDC_Debug( "... Channel1NumberOfHits=%d", OperationalStatus.Channel1NumberOfHits );
        TDC_Debug( "... Channel2NumberOfHits=%d", OperationalStatus.Channel2NumberOfHits );
        TDC_Debug( "... Timeout_TDC=%d", OperationalStatus.Timeout_TDC );
        TDC_Debug( "... Timeout_PreCounter=%d", OperationalStatus.Timeout_PreCounter );
        TDC_Debug( "... Temperature_Sensor_Open=%d", OperationalStatus.Temperature_Sensor_Open );
        TDC_Debug( "... Temperature_Sensor_Short=%d", OperationalStatus.Temperature_Sensor_Short );
        TDC_Debug( "... EEPROM_Error=%d", OperationalStatus.EEPROM_Error );
        TDC_Debug( "... EEPROM_Multi_Error=%d", OperationalStatus.EEPROM_Multi_Error );
        TDC_Debug( "... EEPROM_Matches_Configuration=%d", OperationalStatus.EEPROM_Matches_Configuration );

        switch ( FireDirection )
        {
            case TDC_GP22_FireDirection_Up:
                Operation->Context.FireDirection = TDC_GP22_FireDirection_Down;
                break;

            case TDC_GP22_FireDirection_Down:
                Operation->Context.FireDirection = TDC_GP22_FireDirection_Up;
                break;

            case TDC_GP22_FireDirection_Both:
            default:
                // FIXME Start TOF for Both Directions ?!
                break;
        }

        if ( Operation->Context.FireDirection == Context->ConfigurationRegister_5.CONF_FIRE )
        {
            // Operation completion state
            // Operation status has been set in a previous stage, So NO OVERWRITE
            Operation->Handler = NULL;
            break;
        }
#endif
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationInterruptExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        // Nothing is being done here

        Operation->Type = TDC_GP22_OperationType_Interrupt;
        Operation->Handler = TDC_GP22_OperationInterruptResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationInterruptResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_Interrupt )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_Interrupt );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_Interrupt ) == TDC_GP22_Event_Interrupt )
        {
            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationStatusReadExecute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadStatus, ( uint8_t * ) &Context->StatusRegister, UTIL_SizeOf( Context->StatusRegister ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_StatusRead;
        Operation->Handler = TDC_GP22_OperationStatusReadResolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ /* FIXME Resolve Magic Number */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationStatusReadResolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_StatusRead )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_StatusRead );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            TDC_GP22_Response_t * Response = ( TDC_GP22_Response_t * ) Context->Receive.Content;
            UTIL_MemoryReverse( Response->Value, UTIL_SizeOf( Context->StatusRegister ) ); // TDC-GP22 sends MSB first, STM32 is little-endian. So, reverse
            UTIL_MemoryCopy( &Context->StatusRegister, Response->Value, UTIL_SizeOf( Context->StatusRegister ) );
            Context->Receive.Length -= UTIL_SizeOf( TDC_GP22_OpCode_t ) + UTIL_SizeOf( Context->StatusRegister );
            Context->Transmit.Length--;

            TDC_GP22_OperationalStatus_t * OperationalStatus = &Operation->Context.OperationalStatus;
            OperationalStatus->ALU_Operation_Pointer = Context->StatusRegister.ALU_OP_PTR;
            OperationalStatus->Channel1NumberOfHits = Context->StatusRegister.Number_of_Hits_Ch1;
            OperationalStatus->Channel2NumberOfHits = Context->StatusRegister.Number_of_Hits_Ch2;
            OperationalStatus->Timeout_TDC = Context->StatusRegister.Timeout_TDC;
            OperationalStatus->Timeout_PreCounter = Context->StatusRegister.Timeout_PreCounter;
            OperationalStatus->Temperature_Sensor_Open = Context->StatusRegister.Error_Open;
            OperationalStatus->Temperature_Sensor_Short = Context->StatusRegister.Error_Short;
            OperationalStatus->EEPROM_Error = Context->StatusRegister.EEPROM_Error;
            OperationalStatus->EEPROM_Multi_Error = Context->StatusRegister.EEPROM_DED;
            OperationalStatus->EEPROM_Matches_Configuration = Context->StatusRegister.EEPROM_EQ_CREG;

            TDC_Debug( "" );
            TDC_Debug( "Direction [%d] Status=%d", Operation->Context.FireDirection, Operation->Status );

            TDC_Debug( "... ALU_Operation_Pointer=%d", OperationalStatus->ALU_Operation_Pointer );
            TDC_Debug( "... Channel1NumberOfHits=%d", OperationalStatus->Channel1NumberOfHits );
            TDC_Debug( "... Channel2NumberOfHits=%d", OperationalStatus->Channel2NumberOfHits );
            TDC_Debug( "... Timeout_TDC=%d", OperationalStatus->Timeout_TDC );
            TDC_Debug( "... Timeout_PreCounter=%d", OperationalStatus->Timeout_PreCounter );
            TDC_Debug( "... Temperature_Sensor_Open=%d", OperationalStatus->Temperature_Sensor_Open );
            TDC_Debug( "... Temperature_Sensor_Short=%d", OperationalStatus->Temperature_Sensor_Short );
            TDC_Debug( "... EEPROM_Error=%d", OperationalStatus->EEPROM_Error );
            TDC_Debug( "... EEPROM_Multi_Error=%d", OperationalStatus->EEPROM_Multi_Error );
            TDC_Debug( "... EEPROM_Matches_Configuration=%d", OperationalStatus->EEPROM_Matches_Configuration );

            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_0_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult0, ( uint8_t * ) &Context->Result_Register_0, UTIL_SizeOf( Context->Result_Register_0 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_Measurement_0_Read;
        Operation->Handler = TDC_GP22_OperationMeasurement_0_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ /* FIXME Resolve Magic Number */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_0_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_Measurement_0_Read )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_Measurement_0_Read );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            TDC_GP22_Response_t * Response = ( TDC_GP22_Response_t * ) Context->Receive.Content;
            UTIL_MemoryReverse( Response->Value, UTIL_SizeOf( Context->Result_Register_0 ) ); // TDC-GP22 sends MSB first, STM32 is little-endian. So, reverse
            UTIL_MemoryCopy( &Context->Result_Register_0, Response->Value, UTIL_SizeOf( Context->Result_Register_0 ) );
            Context->Receive.Length -= UTIL_SizeOf( TDC_GP22_OpCode_t ) + UTIL_SizeOf( Context->Result_Register_0 );
            Context->Transmit.Length--;

            Operation->Context.Measurement_0 = UTIL_FixedToDouble( Context->Result_Register_0.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Context->ConfigurationRegister_0.DIV_CLKHS );

            TDC_GP22_OperationalStatus_t * OperationalStatus = &Operation->Context.OperationalStatus;
            if ( OperationalStatus->Timeout_TDC || OperationalStatus->Timeout_PreCounter )
            {
                // Nothing to do
            }
            else if ( Instance->OnMeasurement_0 != NULL )
            {
                Instance->OnMeasurement_0( Instance, Operation->Context.FireDirection, Operation->Context.Measurement_0 );
            }

            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_1_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult1, ( uint8_t * ) &Context->Result_Register_1, UTIL_SizeOf( Context->Result_Register_1 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_Measurement_1_Read;
        Operation->Handler = TDC_GP22_OperationMeasurement_1_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ /* FIXME Resolve Magic Number */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_1_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_Measurement_1_Read )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_Measurement_1_Read );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            TDC_GP22_Response_t * Response = ( TDC_GP22_Response_t * ) Context->Receive.Content;
            UTIL_MemoryReverse( Response->Value, UTIL_SizeOf( Context->Result_Register_1 ) ); // TDC-GP22 sends MSB first, STM32 is little-endian. So, reverse
            UTIL_MemoryCopy( &Context->Result_Register_1, Response->Value, UTIL_SizeOf( Context->Result_Register_1 ) );
            Context->Receive.Length -= UTIL_SizeOf( TDC_GP22_OpCode_t ) + UTIL_SizeOf( Context->Result_Register_1 );
            Context->Transmit.Length--;

            Operation->Context.Measurement_1 = UTIL_FixedToDouble( Context->Result_Register_1.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Context->ConfigurationRegister_0.DIV_CLKHS );

            TDC_GP22_OperationalStatus_t * OperationalStatus = &Operation->Context.OperationalStatus;
            if ( OperationalStatus->Timeout_TDC || OperationalStatus->Timeout_PreCounter )
            {
                // Nothing to do
            }
            else if ( Instance->OnMeasurement_1 != NULL )
            {
                Instance->OnMeasurement_1( Instance, Operation->Context.FireDirection, Operation->Context.Measurement_1 );
            }

            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_2_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult2, ( uint8_t * ) &Context->Result_Register_2, UTIL_SizeOf( Context->Result_Register_2 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_Measurement_2_Read;
        Operation->Handler = TDC_GP22_OperationMeasurement_2_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ /* FIXME Resolve Magic Number */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_2_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_Measurement_2_Read )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_Measurement_2_Read );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            TDC_GP22_Response_t * Response = ( TDC_GP22_Response_t * ) Context->Receive.Content;
            UTIL_MemoryReverse( Response->Value, UTIL_SizeOf( Context->Result_Register_2 ) ); // TDC-GP22 sends MSB first, STM32 is little-endian. So, reverse
            UTIL_MemoryCopy( &Context->Result_Register_2, Response->Value, UTIL_SizeOf( Context->Result_Register_2 ) );
            Context->Receive.Length -= UTIL_SizeOf( TDC_GP22_OpCode_t ) + UTIL_SizeOf( Context->Result_Register_2 );
            Context->Transmit.Length--;

            Operation->Context.Measurement_2 = UTIL_FixedToDouble( Context->Result_Register_2.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Context->ConfigurationRegister_0.DIV_CLKHS );

            TDC_GP22_OperationalStatus_t * OperationalStatus = &Operation->Context.OperationalStatus;
            if ( OperationalStatus->Timeout_TDC || OperationalStatus->Timeout_PreCounter )
            {
                // Nothing to do
            }
            else if ( Instance->OnMeasurement_2 != NULL )
            {
                Instance->OnMeasurement_2( Instance, Operation->Context.FireDirection, Operation->Context.Measurement_2 );
            }

            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_3_Execute( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult3, ( uint8_t * ) &Context->Result_Register_3, UTIL_SizeOf( Context->Result_Register_3 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        Operation->Type = TDC_GP22_OperationType_Measurement_3_Read;
        Operation->Handler = TDC_GP22_OperationMeasurement_3_Resolve;
        Operation->Status = TDC_GP22_Status_Success;
        Operation->Timeout = TDC_GP22_Context.Timestamp;

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_Timestamp_AddMillisecond( &Operation->Timeout, 50 /* FIXME Set appropriate value */ /* FIXME Resolve Magic Number */ ) ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
    }
    while ( 0 );

    return Status;
}

static TDC_GP22_Status_t TDC_GP22_OperationMeasurement_3_Resolve( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Operation->Type != TDC_GP22_OperationType_Measurement_3_Read )
        {
            TDC_Error( "%s Got %d Expected %d", __FUNCTION__, Operation->Type, TDC_GP22_OperationType_Measurement_3_Read );
            Status = TDC_GP22_Status_Error;
            break;
        }

        TIM_Status_t TIM_Status = TIM_Status_Error;
        if ( ( TIM_Status = TIM_IsExpiredTimestamp( TDC_TIM, &Operation->Timeout ) ) == TIM_Status_Success )
        {
            Operation->Status = TDC_GP22_Status_Timeout;
            Operation->Handler = NULL;
            break;
        }

        if ( ( Context->Event & TDC_GP22_Event_SPI_Success ) == TDC_GP22_Event_SPI_Success )
        {
            TDC_GP22_Response_t * Response = ( TDC_GP22_Response_t * ) Context->Receive.Content;
            UTIL_MemoryReverse( Response->Value, UTIL_SizeOf( Context->Result_Register_3 ) ); // TDC-GP22 sends MSB first, STM32 is little-endian. So, reverse
            UTIL_MemoryCopy( &Context->Result_Register_3, Response->Value, UTIL_SizeOf( Context->Result_Register_3 ) );
            Context->Receive.Length -= UTIL_SizeOf( TDC_GP22_OpCode_t ) + UTIL_SizeOf( Context->Result_Register_3 );
            Context->Transmit.Length--;

            Operation->Context.Measurement_3 = UTIL_FixedToDouble( Context->Result_Register_3.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Context->ConfigurationRegister_0.DIV_CLKHS );

            TDC_GP22_OperationalStatus_t * OperationalStatus = &Operation->Context.OperationalStatus;
            if ( OperationalStatus->Timeout_TDC || OperationalStatus->Timeout_PreCounter )
            {
                // Nothing to do
            }
            else if ( Instance->OnMeasurement_3 != NULL )
            {
                Instance->OnMeasurement_3( Instance, Operation->Context.FireDirection, Operation->Context.Measurement_3 );
            }

            Operation->Status = TDC_GP22_Status_Success;
            Operation->Handler = NULL;
        }
    }
    while ( 0 );

    return Status;
}

// #############################################################################
// #### Public Method(s) #######################################################
// #############################################################################

TDC_GP22_Status_t TDC_GP22_Initialize( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }

        if ( ( Status = TDC_GP22_Context_Initialize( ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Instance_Initialize( Instance ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );

    return Status;
}

TDC_GP22_Status_t TDC_GP22_Cycle( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        if ( ( Status = TDC_GP22_Context_Cycle( ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Instance_Cycle( Instance ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );

    return Status;
}

TDC_GP22_Status_t TDC_GP22_DeInitialize( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        if ( ( Status = TDC_GP22_Instance_DeInitialize( Instance ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Context_DeInitialize( ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );

    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel1DelayValue_nsec( TDC_GP22_Instance_t * Instance, TDC_GP22_DelayValue_nsec_t DelayValue_nsec )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    TDC_GP22_Channel1DelayValue_t Channel1DelayValue = 0;

    do
    {
        TDC_Trace( "%s( Instance=%p, Delay=%.3f )", __FUNCTION__, Instance, DelayValue_nsec );

        TDC_GP22_InstanceContext_t * Context = Instance->Context;

        Channel1DelayValue = UTIL_DoubleToFixed( DelayValue_nsec * ( Context->ConfigurationRegister_0.DIV_FIRE + 1 ), 5 /* Fixed point conversion DELVAL1: 14 integer, and **5** fraction */ );

        Status = TDC_GP22_SetChannel1DelayValue( Instance, Channel1DelayValue );
    }
    while ( 0 );

    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetFireNumberOfPulses( TDC_GP22_Instance_t * Instance, TDC_GP22_FirePulses_t FirePulses )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.ANZ_FIRE = ( FirePulses & 0x0F ) >> 0;
        Instance->Context->ConfigurationRegister_6.ANZ_FIRE = ( FirePulses & 0x70 ) >> 4;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetFireDivider( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDivider_t FireDivider )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.DIV_FIRE = FireDivider;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetCalibrationNumberOfPeriods( TDC_GP22_Instance_t * Instance, TDC_GP22_CalibrationPeriod_t CalibrationPeriod )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.ANZ_PER_CALRES = CalibrationPeriod;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetOscillatorDivider( TDC_GP22_Instance_t * Instance, TDC_GP22_OscillatorDivider_t OscillatorDivider )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.DIV_CLKHS = OscillatorDivider;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetOscillatorInterval( TDC_GP22_Instance_t * Instance, TDC_GP22_OscillatorInterval_t OscillatorInterval )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.START_CLKHS = ( OscillatorInterval & 0x03 ) >> 0;
        Instance->Context->ConfigurationRegister_6.START_CLKHS = ( OscillatorInterval & 0x04 ) >> 2;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTemperaturePorts( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperaturePorts_t TemperaturePorts )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.ANZ_PORT = TemperaturePorts;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTemperatureInterval( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureInterval_t TemperatureInterval )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.TCYCLE = TemperatureInterval;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTemperatureDummyInterval( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureDummyInterval_t TemperatureDummyInterval )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.ANZ_FAKE = TemperatureDummyInterval;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTemperatureClockSource( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureClockSource_t TemperatureClockSource )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.SEL_ECLK_TMP = TemperatureClockSource;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetCalibration( TDC_GP22_Instance_t * Instance, TDC_GP22_Calibration_t Calibration )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_0.MESSB2 )
        {
            case 0:
                // Measurement mode 1
                switch ( Calibration )
                {
                    case TDC_GP22_Calibrate_Off:
                    case TDC_GP22_Calibrate_On:
                        // Valid values
                        break;
                    default:
                        Status = TDC_GP22_Status_ConfigurationInvalid;
                        break;
                }
                break;
            case 1:
                // Measurement mode 2
                // no break
            default:
                // Measurement mode 2
                switch ( Calibration )
                {
                    case TDC_GP22_Calibrate_On:
                        // Valid values
                        break;
                    default:
                        Status = TDC_GP22_Status_ConfigurationInvalid;
                        break;
                }
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_0.CALIBRATE = Calibration;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetCalibrationAuto( TDC_GP22_Instance_t * Instance, TDC_GP22_AutoCalibration_t AutoCalibration )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.NO_CAL_AUTO = AutoCalibration;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetMode( TDC_GP22_Instance_t * Instance, TDC_GP22_Mode_t Mode )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.MESSB2 = Mode;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetStop2SignalMode( TDC_GP22_Instance_t * Instance, TDC_GP22_Stop2SignalMode_t Stop2SignalMode )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.NEG_STOP2 = Stop2SignalMode;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetStop1SignalMode( TDC_GP22_Instance_t * Instance, TDC_GP22_Stop1SignalMode_t Stop1SignalMode )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.NEG_STOP1 = Stop1SignalMode;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetStartSignalMode( TDC_GP22_Instance_t * Instance, TDC_GP22_StartSignalMode_t StartSignalMode )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_0.NEG_START = StartSignalMode;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetHIT_2_Operator( TDC_GP22_Instance_t * Instance, TDC_GP22_HIT_2_Operator_t HIT_2_Operator )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_0.MESSB2 )
        {
            case 0:
                // Measurement mode 1
                switch ( HIT_2_Operator )
                {
                    case TDC_GP22_HIT_2_Operator_Mode_1_Start:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_1:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_2:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_3:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_4:
                    case TDC_GP22_HIT_2_Operator_Mode_1_No_Action:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Calibration_1:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Calibration_2:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_1:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_2:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_3:
                    case TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_4:
                        // Valid values
                        break;
                    default:
                        Status = TDC_GP22_Status_ConfigurationInvalid;
                        break;
                }
                break;
            case 1:
                // Measurement mode 2
                // no break
            default:
                // Measurement mode 2
                switch ( HIT_2_Operator )
                {
                    case TDC_GP22_HIT_2_Operator_Mode_2_Channel_1_Stop_1:
                    case TDC_GP22_HIT_2_Operator_Mode_2_Channel_1_Stop_2:
                    case TDC_GP22_HIT_2_Operator_Mode_2_Channel_1_Stop_3:
                        // Valid values
                        break;
                    default:
                        Status = TDC_GP22_Status_ConfigurationInvalid;
                        break;
                }
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_1.HIT2 = HIT_2_Operator;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetHIT_1_Operator( TDC_GP22_Instance_t * Instance, TDC_GP22_HIT_1_Operator_t HIT_1_Operator )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_0.MESSB2 )
        {
            case 0:
                // Measurement mode 1
                switch ( HIT_1_Operator )
                {
                    case TDC_GP22_HIT_1_Operator_Mode_1_Start:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_1:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_2:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_3:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_4:
                    case TDC_GP22_HIT_1_Operator_Mode_1_No_Action:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Calibration_1:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Calibration_2:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_1:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_2:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_3:
                    case TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_4:
                        // Valid values
                        break;
                    default:
                        Status = TDC_GP22_Status_ConfigurationInvalid;
                        break;
                }
                break;
            case 1:
                // Measurement mode 2
                // no break
            default:
                // Measurement mode 2
                switch ( HIT_1_Operator )
                {
                    case TDC_GP22_HIT_1_Operator_Mode_2_Start:
                        // Valid values
                        break;
                    default:
                        Status = TDC_GP22_Status_ConfigurationInvalid;
                        break;
                }
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_1.HIT1 = HIT_1_Operator;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetFastInit( TDC_GP22_Instance_t * Instance, TDC_GP22_FastInit_t FastInit )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_1.EN_FAST_INIT = FastInit;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel2Hits( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel2Hits_t Channel2Hits )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_1.HITIN2 = Channel2Hits;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel1Hits( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel1Hits_t Channel1Hits )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_1.HITIN1 = Channel1Hits;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetCurrentMode( TDC_GP22_Instance_t * Instance, TDC_GP22_CurrentMode_t CurrentMode )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_1.CURR32K = CurrentMode;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetStartFire( TDC_GP22_Instance_t * Instance, TDC_GP22_StartFire_t StartFire )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_1.SEL_START_FIRE = StartFire;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetStartEnable( TDC_GP22_Instance_t * Instance, TDC_GP22_StartEnable_t StartEnable )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_1.SEL_TSTO2 = StartEnable;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetFireInput( TDC_GP22_Instance_t * Instance, TDC_GP22_FireInput_t FireInput )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_1.SEL_TSTO1 = FireInput;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetInterruptEnable( TDC_GP22_Instance_t * Instance, TDC_GP22_InterruptEnable_t InterruptEnable )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_2.EN_INT = ( InterruptEnable & 0x07 ) >> 0;
        Instance->Context->ConfigurationRegister_6.EN_INT = ( InterruptEnable & 0x08 ) >> 3;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel2SenseEdge( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel2SenseEdge_t Channel2SenseEdge )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_2.RFEDGE2 = Channel2SenseEdge;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel1SenseEdge( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel1SenseEdge_t Channel1SenseEdge )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_2.RFEDGE1 = Channel1SenseEdge;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel1DelayValue( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel1DelayValue_t Channel1DelayValue )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( Instance->Context->ConfigurationRegister_6.EN_ANALOG == 0
             && Channel1DelayValue > 0 )
        {
            TDC_Error( "Invalid Configuration !" );
            Status = TDC_GP22_Status_ConfigurationInvalid;
            break;
        }
        Instance->Context->ConfigurationRegister_2.DELVAL1 = ( Channel1DelayValue & 0x07FFFF ) >> 0;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetAutomaticCalculation( TDC_GP22_Instance_t * Instance, TDC_GP22_AutomaticCalculation_t AutomaticCalculation )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        // TODO Check operational behavior in mode-1 and mode-2
        switch ( Instance->Context->ConfigurationRegister_0.MESSB2 )
        {
            case 0:
                // Measurement mode 1
                // FIXME is it ok to use it in mode-1 ?
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
            case 1:
                // Measurement mode 2
                // no break
            default:
                // Measurement mode 2
                // Valid values
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_3.EN_AUTOCALC_MB2 = AutomaticCalculation;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetErrorValue( TDC_GP22_Instance_t * Instance, TDC_GP22_ErrorValue_t ErrorValue )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_3.EN_ERR_VAL = ErrorValue;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetFirstWave( TDC_GP22_Instance_t * Instance, TDC_GP22_FirstWave_t FirstWave )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE = FirstWave;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTimeoutDivider( TDC_GP22_Instance_t * Instance, TDC_GP22_TimeoutDivider_t TimeoutDivider )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_0.MESSB2 )
        {
            case 0:
                // Measurement mode 1
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
            case 1:
                // Measurement mode 2
                // no break
            default:
                // Measurement mode 2
                // Valid values
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_3.SEL_TIMO_MB2 = TimeoutDivider;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetNumberOfPeriodsStop1( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPeriodsStop1_t NumberOfPeriodsStop1 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_3.DELREL1 = NumberOfPeriodsStop1;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetNumberOfPeriodsStop2( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPeriodsStop2_t NumberOfPeriodsStop2 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_3.DELREL2 = NumberOfPeriodsStop2;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetNumberOfPeriodsStop3( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPeriodsStop3_t NumberOfPeriodsStop3 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_3.DELREL3 = NumberOfPeriodsStop3;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel2DelayValue( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel2DelayValue_t Channel2DelayValue )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                // Valid values
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        if ( Instance->Context->ConfigurationRegister_6.EN_ANALOG == 0
             && Channel2DelayValue > 0 )
        {
            TDC_Error( "Invalid Configuration !" );
            Status = TDC_GP22_Status_ConfigurationInvalid;
            break;
        }
        Instance->Context->ConfigurationRegister_3.DELVAL2 = Channel2DelayValue;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChannel3DelayValue( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel3DelayValue_t Channel3DelayValue )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                // Valid values
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration Detected !" );
            break;
        }
        if ( Instance->Context->ConfigurationRegister_6.EN_ANALOG == 0
             && Channel3DelayValue > 0 )
        {
            TDC_Error( "Invalid Configuration Detected !" );
            Status = TDC_GP22_Status_ConfigurationInvalid;
            break;
        }
        Instance->Context->ConfigurationRegister_4.DELVAL3 = Channel3DelayValue;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetPulseWidthMeasurement( TDC_GP22_Instance_t * Instance, TDC_GP22_PulseWidthMeasurement_t PulseWidthMeasurement )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_4.DIS_PW = PulseWidthMeasurement;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetFirstWaveEdge( TDC_GP22_Instance_t * Instance, TDC_GP22_FirstWaveEdge_t FirstWaveEdge )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_4.EDGE_FW = FirstWaveEdge;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetAdditionalOffsetShiftPositive( TDC_GP22_Instance_t * Instance, TDC_GP22_AdditionalOffsetShiftPositive_t AdditionalOffsetShiftPositive )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_4.OFFSRNG2 = AdditionalOffsetShiftPositive;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetAdditionalOffsetShiftNegative( TDC_GP22_Instance_t * Instance, TDC_GP22_AdditionalOffsetShiftNegative_t AdditionalOffsetShiftNegative )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_4.OFFSRNG1 = AdditionalOffsetShiftNegative;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetOffsetShift( TDC_GP22_Instance_t * Instance, TDC_GP22_OffsetShift_t OffsetShift )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_3.EN_FIRST_WAVE )
        {
            case 1:
                // First wave enabled
                // Valid values
                break;
            case 0:
                // First wave disabled
                // no break
            default:
                // First wave disabled
                Status = TDC_GP22_Status_ConfigurationInvalid;
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        Instance->Context->ConfigurationRegister_4.OFFS = OffsetShift;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetFireDirection( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDirection_t FireDirection )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        // TODO Setting `Both` Overwrites `Down` bit with the inverse of `Up`
        Instance->Context->ConfigurationRegister_5.CONF_FIRE = FireDirection;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetNoiseStart( TDC_GP22_Instance_t * Instance, TDC_GP22_NoiseStart_t NoiseStart )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_5.EN_STARTNOISE = NoiseStart;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetPhaseShift( TDC_GP22_Instance_t * Instance, TDC_GP22_PhaseShift_t PhaseShift )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_5.DIS_PHASESHIFT = PhaseShift;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetNumberOfPulseRepetition( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPulseRepetition_t NumberOfPulseRepetition )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_5.REPEAT_FIRE = NumberOfPulseRepetition;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetPhaseInversion( TDC_GP22_Instance_t * Instance, TDC_GP22_PhaseInversionPulse_t PhaseInversionPulse, TDC_GP22_PhaseInversion_t PhaseInversion )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( Instance->Context->ConfigurationRegister_6.ANZ_FIRE > 0 )
        {
            TDC_Error( "Invalid Configuration !" );
            Status = TDC_GP22_Status_ConfigurationInvalid;
            break;
        }
        switch ( PhaseInversion )
        {
            case TDC_GP22_PhaseInversion_LowToHigh:
                Instance->Context->ConfigurationRegister_5.PHFIRE |= ( 0x0001 << PhaseInversionPulse );
                break;
            case TDC_GP22_PhaseInversion_HighToLow:
                // no break
            default:
                Instance->Context->ConfigurationRegister_5.PHFIRE &= ~( 0x0001 << PhaseInversionPulse );
                break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetAnalogEnable( TDC_GP22_Instance_t * Instance, TDC_GP22_AnalogEnable_t AnalogEnable )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        // TODO Check `DELVAL2` and `DELVAL3` with respect to `EN_ANALOG`
        // FIXME Validate `DELVALx` with respect to `EN_ANALOG`
        if ( Instance->Context->ConfigurationRegister_2.DELVAL1 > 0
             || Instance->Context->ConfigurationRegister_3.DELVAL2 > 0
             || Instance->Context->ConfigurationRegister_4.DELVAL3 > 0 )
        {
            TDC_Error( "Invalid Configuration !" );
            Status = TDC_GP22_Status_ConfigurationInvalid;
            break;
        }
        Instance->Context->ConfigurationRegister_6.EN_ANALOG = AnalogEnable;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTemperatureInput( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureInput_t TemperatureInput )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.NEG_STOP_TEMP = TemperatureInput;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetComparatorOffset( TDC_GP22_Instance_t * Instance, TDC_GP22_ComparatorOffset_t ComparatorOffset )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.DA_KORR = ComparatorOffset;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetChargeTime( TDC_GP22_Instance_t * Instance, TDC_GP22_ChargeTime_t ChargeTime )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.TW2 = ChargeTime;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTemperatureCycle( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureCycle_t TemperatureCycle )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.CYCLE_TEMP = TemperatureCycle;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTimeOfFlightCycle( TDC_GP22_Instance_t * Instance, TDC_GP22_TimeOfFlightCycle_t TimeOfFlightCycle )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.CYCLE_TOF = TimeOfFlightCycle;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetCycle( TDC_GP22_Instance_t * Instance, TDC_GP22_Cycle_t Cycle )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.HZ60 = Cycle;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetDefaultFireLevel( TDC_GP22_Instance_t * Instance, TDC_GP22_DefaultFireLevel_t DefaultFireLevel )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.FIREO_DEF = DefaultFireLevel;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetResolution( TDC_GP22_Instance_t * Instance, TDC_GP22_Resolution_t Resolution )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        switch ( Instance->Context->ConfigurationRegister_0.MESSB2 )
        {
            case 0:
                // Measurement mode 1
                switch ( Resolution )
                {
                    case TDC_GP22_Resolution_2x:
                        // TODO Check the relation to STOP1/2 inputs
                        // Valid values
                        break;
                    case TDC_GP22_Resolution_4x:
                        Status = TDC_GP22_Status_ConfigurationInvalid;
                        break;
                    default:
                        // Valid values
                        break;
                }
                break;
            case 1:
                // Measurement mode 2
                // no break
            default:
                // Measurement mode 2
                // Valid values
                break;
        }
        if ( Status == TDC_GP22_Status_ConfigurationInvalid )
        {
            TDC_Error( "Invalid Configuration !" );
            break;
        }
        switch ( Resolution )
        {
            case TDC_GP22_Resolution_2x:
                Instance->Context->ConfigurationRegister_6.DOUBLE_RES = 1;
                Instance->Context->ConfigurationRegister_6.QUAD_RES = 0;
                break;
            case TDC_GP22_Resolution_4x:
                Instance->Context->ConfigurationRegister_6.DOUBLE_RES = 0;
                Instance->Context->ConfigurationRegister_6.QUAD_RES = 1;
                break;
            case TDC_GP22_Resolution_Off:
                // no break
            default:
                Instance->Context->ConfigurationRegister_6.DOUBLE_RES = 0;
                Instance->Context->ConfigurationRegister_6.QUAD_RES = 0;
                break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_SetTemperatureMeasurementDirection( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureMeasurementDirection_t TemperatureMeasurementDirection )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        Instance->Context->ConfigurationRegister_6.TEMP_PORTDIR = TemperatureMeasurementDirection;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetMeasurement_0( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( Measurement == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        TDC_GP22_ResultRegister_0_t Result_Register_0;
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult0, ( uint8_t * ) &Result_Register_0, UTIL_SizeOf( Result_Register_0 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        *Measurement = UTIL_FixedToDouble( Result_Register_0.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Instance->Context->ConfigurationRegister_0.DIV_CLKHS );
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetMeasurement_1( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( Measurement == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        TDC_GP22_ResultRegister_1_t Result_Register_1;
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult1, ( uint8_t * ) &Result_Register_1, UTIL_SizeOf( Result_Register_1 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        *Measurement = UTIL_FixedToDouble( Result_Register_1.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Instance->Context->ConfigurationRegister_0.DIV_CLKHS );
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetMeasurement_2( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( Measurement == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        TDC_GP22_ResultRegister_2_t Result_Register_2;
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult2, ( uint8_t * ) &Result_Register_2, UTIL_SizeOf( Result_Register_2 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        *Measurement = UTIL_FixedToDouble( Result_Register_2.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Instance->Context->ConfigurationRegister_0.DIV_CLKHS );
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetMeasurement_3( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( Measurement == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        TDC_GP22_ResultRegister_3_t Result_Register_3;
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadResult3, ( uint8_t * ) &Result_Register_3, UTIL_SizeOf( Result_Register_3 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        *Measurement = UTIL_FixedToDouble( Result_Register_3.Value, 16 ) * TDC_GP22_TREF * ( 0x01 << Instance->Context->ConfigurationRegister_0.DIV_CLKHS );
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetOperationalStatus( TDC_GP22_Instance_t * Instance, TDC_GP22_OperationalStatus_t * OperationalStatus )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( OperationalStatus == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }

        // FIXME
        Status = TDC_GP22_Status_NotSupported;
#if 0
        TDC_GP22_StatusRegister_t StatusRegister;
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadStatus, ( uint8_t * ) &StatusRegister, UTIL_SizeOf( StatusRegister ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        OperationalStatus->ALU_Operation_Pointer = StatusRegister.ALU_OP_PTR;
        OperationalStatus->Channel1NumberOfHits = StatusRegister.Number_of_Hits_Ch1;
        OperationalStatus->Channel2NumberOfHits = StatusRegister.Number_of_Hits_Ch2;
        OperationalStatus->Timeout_TDC = StatusRegister.Timeout_TDC;
        OperationalStatus->Timeout_PreCounter = StatusRegister.Timeout_PreCounter;
        OperationalStatus->Temperature_Sensor_Open = StatusRegister.Error_Open;
        OperationalStatus->Temperature_Sensor_Short = StatusRegister.Error_Short;
        OperationalStatus->EEPROM_Error = StatusRegister.EEPROM_Error;
        OperationalStatus->EEPROM_Multi_Error = StatusRegister.EEPROM_DED;
        OperationalStatus->EEPROM_Matches_Configuration = StatusRegister.EEPROM_EQ_CREG;
#endif
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetPulseWidthRatio( TDC_GP22_Instance_t * Instance, TDC_GP22_PulseWidthRatio_t * PulseWidthRatio )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( PulseWidthRatio == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        TDC_GP22_PW1ST_Register_t PW1ST_Register;
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ReadPW1ST, ( uint8_t * ) &PW1ST_Register, UTIL_SizeOf( PW1ST_Register ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        *PulseWidthRatio = UTIL_FixedToDouble( PW1ST_Register.Value, 7 );
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetID( TDC_GP22_Instance_t * Instance, TDC_GP22_ID_t * ID )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ID == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        if ( ( Status = TDC_GP22_Read( Instance, TDC_GP22_OpCode_ID, ( uint8_t * ) ID, UTIL_SizeOf( TDC_GP22_ID_t ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_EEPROM_Save( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteConfig, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_EEPROM_Load( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_ReadConfig, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_EEPROM_IsValid( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_ValidateConfig, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_Commit( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        // FIXME
        //    if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_PowerOnReset, NULL, 0 ) ) != TDC_GP22_Status_Success )
        //    {
        //      break;
        //    }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister0, ( uint8_t * ) &Instance->Context->ConfigurationRegister_0, UTIL_SizeOf( Instance->Context->ConfigurationRegister_0 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister1, ( uint8_t * ) &Instance->Context->ConfigurationRegister_1, UTIL_SizeOf( Instance->Context->ConfigurationRegister_1 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister2, ( uint8_t * ) &Instance->Context->ConfigurationRegister_2, UTIL_SizeOf( Instance->Context->ConfigurationRegister_2 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister3, ( uint8_t * ) &Instance->Context->ConfigurationRegister_3, UTIL_SizeOf( Instance->Context->ConfigurationRegister_3 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister4, ( uint8_t * ) &Instance->Context->ConfigurationRegister_4, UTIL_SizeOf( Instance->Context->ConfigurationRegister_4 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister5, ( uint8_t * ) &Instance->Context->ConfigurationRegister_5, UTIL_SizeOf( Instance->Context->ConfigurationRegister_5 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_WriteRegister6, ( uint8_t * ) &Instance->Context->ConfigurationRegister_6, UTIL_SizeOf( Instance->Context->ConfigurationRegister_6 ) ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_Reset( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        // TODO Decide either to perform soft-reset or hard-reset

        //    // Soft-reset
        //    if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_PowerOnReset, NULL, 0 ) ) != TDC_GP22_Status_Success )
        //    {
        //      break;
        //    }

        // FIXME Split Into Process Operations
        {
            if ( GPIO_Write( Instance->Reset, GPIO_Value_Low ) != GPIO_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }

            TIM_Timestamp_t Timestamp;
            if ( TIM_GetTimestamp( TDC_TIM, &Timestamp ) != TIM_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }

            if ( TIM_Timestamp_AddMillisecond( &Timestamp, 10 ) != TIM_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }
            while ( TIM_IsExpiredTimestamp( TDC_TIM, &Timestamp ) != TIM_Status_Success )
            {
                RTC_Cycle( RTC_All );
                TIM_Cycle( TIM_All );
            }

            if ( GPIO_Write( Instance->Reset, GPIO_Value_High ) != GPIO_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }
        }

        // Restore Defaults
        Instance->Context->ConfigurationRegister_0.Value = 0b00100010000001100110100000000000;
        Instance->Context->ConfigurationRegister_1.Value = 0b01010101010000000000000000000000;
        Instance->Context->ConfigurationRegister_2.Value = 0b00100000000000000000000000000000;
        Instance->Context->ConfigurationRegister_3.Value = 0b00011000000000000000000000000000;
        Instance->Context->ConfigurationRegister_4.Value = 0b00100000000000000000000000000000;
        Instance->Context->ConfigurationRegister_5.Value = 0b00000000000000000000000000000000;
        Instance->Context->ConfigurationRegister_6.Value = 0b00000000000000000000000000000000;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_Test( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        // FIXME Handle
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_StartTimeOfFlight( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_Init, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }

        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_StartTOF, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        TIM_Timestamp_t Timestamp;
        TDC_GP22_OperationalStatus_t OperationalStatus;
        GPIO_Status_t GPIO_Status = GPIO_Status_Error;
        GPIO_Value_t GPIO_Value = GPIO_Value_High;

        if ( TIM_GetTimestamp( TDC_TIM, &Timestamp ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }
        if ( TIM_Timestamp_AddMillisecond( &Timestamp, 50 ) != TIM_Status_Success )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        while ( GPIO_Value == GPIO_Value_High )
        {
            // FIXME Split into process/operations
            RTC_Cycle( RTC_All );
            TIM_Cycle( TIM_All );
            if ( TIM_IsExpiredTimestamp( TDC_TIM, &Timestamp ) == TIM_Status_Success )
            {
                Status = TDC_GP22_Status_Timeout;
                break;
            }
            if ( ( GPIO_Status = GPIO_Read( Instance->Interrupt, &GPIO_Value ) ) != GPIO_Status_Success )
            {
                Status = TDC_GP22_Status_Error;
                break;
            }
        }

        if ( ( Status = TDC_GP22_GetOperationalStatus( Instance, &OperationalStatus ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
        if ( OperationalStatus.Timeout_PreCounter || OperationalStatus.Timeout_TDC )
        {
            Status = TDC_GP22_Status_Timeout;
            break;
        }

        TDC_GP22_FireDirection_t Direction = Instance->Context->ConfigurationRegister_5.CONF_FIRE;
        switch ( Direction )
        {
            case TDC_GP22_FireDirection_Up:
            case TDC_GP22_FireDirection_Down:
                if ( Instance->OnMeasurement_0 != NULL )
                {
                    TDC_GP22_Measurement_t Measurement;
                    if ( ( Status = TDC_GP22_GetMeasurement_0( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
                    {
                        break;
                    }
                    Instance->OnMeasurement_0( Instance, Direction, Measurement );
                }

                if ( Instance->OnMeasurement_1 != NULL )
                {
                    TDC_GP22_Measurement_t Measurement;
                    if ( ( Status = TDC_GP22_GetMeasurement_1( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
                    {
                        break;
                    }
                    Instance->OnMeasurement_1( Instance, Direction, Measurement );
                }

                if ( Instance->OnMeasurement_2 != NULL )
                {
                    TDC_GP22_Measurement_t Measurement;
                    if ( ( Status = TDC_GP22_GetMeasurement_2( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
                    {
                        break;
                    }
                    Instance->OnMeasurement_2( Instance, Direction, Measurement );
                }

                if ( Instance->OnMeasurement_3 != NULL )
                {
                    TDC_GP22_Measurement_t Measurement;
                    if ( ( Status = TDC_GP22_GetMeasurement_3( Instance, &Measurement ) ) != TDC_GP22_Status_Success )
                    {
                        break;
                    }
                    Instance->OnMeasurement_3( Instance, Direction, Measurement );
                }
                break;

            case TDC_GP22_FireDirection_Both:
            default:
                // FIXME Start TOF for Both Directions ?!
                break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_StartTemperature( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_StartTemp, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_StartCalibrateResonator( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_StartCalResonator, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_StartCalibrateTDC( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_StartCalTDC, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_StartTimeOfFlightRestart( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;

    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );

        if ( Instance == NULL )
        {
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }

        if ( Instance->Context == NULL )
        {
            Status = TDC_GP22_Status_Error;
            break;
        }

        TDC_GP22_InstanceContext_t * Context = &TDC_GP22_Context.Context[ Instance->GP22x ];
        TDC_GP22_Process_t * Process = &Context->Process;
        TDC_GP22_Operation_t * Operation = &Process->Context.Operation;

        if ( Process->Type != TDC_GP22_ProcessType_None )
        {
            Status = TDC_GP22_Status_Busy;
            break;
        }

        if ( Operation->Type != TDC_GP22_OperationType_None )
        {
            Status = TDC_GP22_Status_Busy;
            break;
        }

        Status = TDC_GP22_SetProcess( Instance, TDC_GP22_ProcessType_TimeOfFlightRestart );
    }
    while ( 0 );

    return Status;
}

TDC_GP22_Status_t TDC_GP22_StartTemperatureRestart( TDC_GP22_Instance_t * Instance )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ( Status = TDC_GP22_Write( Instance, TDC_GP22_OpCode_StartTempRestart, NULL, 0 ) ) != TDC_GP22_Status_Success )
        {
            break;
        }
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_0( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_0 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ConfigurationRegister_0 == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        *ConfigurationRegister_0 = Instance->Context->ConfigurationRegister_0.Value;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_1( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_1 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ConfigurationRegister_1 == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        *ConfigurationRegister_1 = Instance->Context->ConfigurationRegister_1.Value;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_2( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_2 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ConfigurationRegister_2 == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        *ConfigurationRegister_2 = Instance->Context->ConfigurationRegister_2.Value;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_3( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_3 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ConfigurationRegister_3 == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        *ConfigurationRegister_3 = Instance->Context->ConfigurationRegister_3.Value;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_4( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_4 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ConfigurationRegister_4 == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        *ConfigurationRegister_4 = Instance->Context->ConfigurationRegister_4.Value;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_5( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_5 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ConfigurationRegister_5 == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        *ConfigurationRegister_5 = Instance->Context->ConfigurationRegister_5.Value;
    }
    while ( 0 );
    return Status;
}

TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_6( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_6 )
{
    TDC_GP22_Status_t Status = TDC_GP22_Status_Success;
    do
    {
        TDC_Trace( "%s( Instance=%p )", __FUNCTION__, Instance );
        if ( ConfigurationRegister_6 == NULL )
        {
            TDC_Error( "Invalid Argument" );
            Status = TDC_GP22_Status_ArgumentInvalid;
            break;
        }
        *ConfigurationRegister_6 = Instance->Context->ConfigurationRegister_6.Value;
    }
    while ( 0 );
    return Status;
}

// #############################################################################
// #### Public Variable(s) #####################################################
// #############################################################################

const char TDC_GP22_VERSION[] = "0.0.0.v20260517-1631";

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
