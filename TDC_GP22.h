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

/**
 *  @file
 *
 *  @brief Platform TDC GP22 Driver
 */

// #############################################################################
// #### Control Include(s) #####################################################
// #############################################################################

// #############################################################################
// #### Control Macro(s) #######################################################
// #############################################################################

// #############################################################################
// #### File Guard #############################################################
// #############################################################################

/**
 *  @addtogroup Platform_TDC_Driver
 *
 *  @{
 */

/**
 *  @defgroup Platform_TDC_GP22 GP22
 *
 *  @{
 */

#ifndef TDC_GP22_H_
    #define TDC_GP22_H_

    #ifdef __cplusplus
extern "C"
{
    #endif /* __cplusplus */

    // #############################################################################
    // #### Include(s) #############################################################
    // #############################################################################

    #include <stdbool.h>
    #include <stdint.h>

    // #############################################################################
    // #### Public Macro(s) ########################################################
    // #############################################################################

    // #############################################################################
    // #### Public Type(s) #########################################################
    // #############################################################################

    /**
     *  @brief TDC GP22 Operation Status Type
     *
     *  @enum TDC_GP22_Status_t
     */
    typedef enum TDC_GP22_Status
    {
        TDC_GP22_Status_Success = 0,          ///< Success
        TDC_GP22_Status_ArgumentInvalid,      ///< Argument Invalid
        TDC_GP22_Status_NotSupported,         ///< Not Supported
        TDC_GP22_Status_Error,                ///< General Error
        TDC_GP22_Status_Busy,                 ///< Busy
        TDC_GP22_Status_Timeout,              ///< Timeout
        TDC_GP22_Status_ConfigurationInvalid, ///< Configuration Invalid
    } TDC_GP22_Status_t;

    /**
     *  @brief TDC GP22 Fire Pulses Type
     *
     *  @enum TDC_GP22_FirePulses_t
     */
    typedef enum TDC_GP22_FirePulses
    {
        TDC_GP22_FirePulses_0 = 0, ///< Zero Pulses (Minimum)
        TDC_GP22_FirePulses_1,     ///<
        TDC_GP22_FirePulses_2,     ///<
        TDC_GP22_FirePulses_3,     ///<
        TDC_GP22_FirePulses_4,     ///<
        TDC_GP22_FirePulses_5,     ///<
        TDC_GP22_FirePulses_6,     ///<
        TDC_GP22_FirePulses_7,     ///<
        TDC_GP22_FirePulses_8,     ///<
        TDC_GP22_FirePulses_9,     ///<
        TDC_GP22_FirePulses_10,    ///<
        TDC_GP22_FirePulses_11,    ///<
        TDC_GP22_FirePulses_12,    ///<
        TDC_GP22_FirePulses_13,    ///<
        TDC_GP22_FirePulses_14,    ///<
        TDC_GP22_FirePulses_15,    ///<
        TDC_GP22_FirePulses_16,    ///<
        TDC_GP22_FirePulses_17,    ///<
        TDC_GP22_FirePulses_18,    ///<
        TDC_GP22_FirePulses_19,    ///<
        TDC_GP22_FirePulses_20,    ///<
        TDC_GP22_FirePulses_21,    ///<
        TDC_GP22_FirePulses_22,    ///<
        TDC_GP22_FirePulses_23,    ///<
        TDC_GP22_FirePulses_24,    ///<
        TDC_GP22_FirePulses_25,    ///<
        TDC_GP22_FirePulses_26,    ///<
        TDC_GP22_FirePulses_27,    ///<
        TDC_GP22_FirePulses_28,    ///<
        TDC_GP22_FirePulses_29,    ///<
        TDC_GP22_FirePulses_30,    ///<
        TDC_GP22_FirePulses_31,    ///<
        TDC_GP22_FirePulses_32,    ///<
        TDC_GP22_FirePulses_33,    ///<
        TDC_GP22_FirePulses_34,    ///<
        TDC_GP22_FirePulses_35,    ///<
        TDC_GP22_FirePulses_36,    ///<
        TDC_GP22_FirePulses_37,    ///<
        TDC_GP22_FirePulses_38,    ///<
        TDC_GP22_FirePulses_39,    ///<
        TDC_GP22_FirePulses_40,    ///<
        TDC_GP22_FirePulses_41,    ///<
        TDC_GP22_FirePulses_42,    ///<
        TDC_GP22_FirePulses_43,    ///<
        TDC_GP22_FirePulses_44,    ///<
        TDC_GP22_FirePulses_45,    ///<
        TDC_GP22_FirePulses_46,    ///<
        TDC_GP22_FirePulses_47,    ///<
        TDC_GP22_FirePulses_48,    ///<
        TDC_GP22_FirePulses_49,    ///<
        TDC_GP22_FirePulses_50,    ///<
        TDC_GP22_FirePulses_51,    ///<
        TDC_GP22_FirePulses_52,    ///<
        TDC_GP22_FirePulses_53,    ///<
        TDC_GP22_FirePulses_54,    ///<
        TDC_GP22_FirePulses_55,    ///<
        TDC_GP22_FirePulses_56,    ///<
        TDC_GP22_FirePulses_57,    ///<
        TDC_GP22_FirePulses_58,    ///<
        TDC_GP22_FirePulses_59,    ///<
        TDC_GP22_FirePulses_60,    ///<
        TDC_GP22_FirePulses_61,    ///<
        TDC_GP22_FirePulses_62,    ///<
        TDC_GP22_FirePulses_63,    ///<
        TDC_GP22_FirePulses_64,    ///<
        TDC_GP22_FirePulses_65,    ///<
        TDC_GP22_FirePulses_66,    ///<
        TDC_GP22_FirePulses_67,    ///<
        TDC_GP22_FirePulses_68,    ///<
        TDC_GP22_FirePulses_69,    ///<
        TDC_GP22_FirePulses_70,    ///<
        TDC_GP22_FirePulses_71,    ///<
        TDC_GP22_FirePulses_72,    ///<
        TDC_GP22_FirePulses_73,    ///<
        TDC_GP22_FirePulses_74,    ///<
        TDC_GP22_FirePulses_75,    ///<
        TDC_GP22_FirePulses_76,    ///<
        TDC_GP22_FirePulses_77,    ///<
        TDC_GP22_FirePulses_78,    ///<
        TDC_GP22_FirePulses_79,    ///<
        TDC_GP22_FirePulses_80,    ///<
        TDC_GP22_FirePulses_81,    ///<
        TDC_GP22_FirePulses_82,    ///<
        TDC_GP22_FirePulses_83,    ///<
        TDC_GP22_FirePulses_84,    ///<
        TDC_GP22_FirePulses_85,    ///<
        TDC_GP22_FirePulses_86,    ///<
        TDC_GP22_FirePulses_87,    ///<
        TDC_GP22_FirePulses_88,    ///<
        TDC_GP22_FirePulses_89,    ///<
        TDC_GP22_FirePulses_90,    ///<
        TDC_GP22_FirePulses_91,    ///<
        TDC_GP22_FirePulses_92,    ///<
        TDC_GP22_FirePulses_93,    ///<
        TDC_GP22_FirePulses_94,    ///<
        TDC_GP22_FirePulses_95,    ///<
        TDC_GP22_FirePulses_96,    ///<
        TDC_GP22_FirePulses_97,    ///<
        TDC_GP22_FirePulses_98,    ///<
        TDC_GP22_FirePulses_99,    ///<
        TDC_GP22_FirePulses_100,   ///<
        TDC_GP22_FirePulses_101,   ///<
        TDC_GP22_FirePulses_102,   ///<
        TDC_GP22_FirePulses_103,   ///<
        TDC_GP22_FirePulses_104,   ///<
        TDC_GP22_FirePulses_105,   ///<
        TDC_GP22_FirePulses_106,   ///<
        TDC_GP22_FirePulses_107,   ///<
        TDC_GP22_FirePulses_108,   ///<
        TDC_GP22_FirePulses_109,   ///<
        TDC_GP22_FirePulses_110,   ///<
        TDC_GP22_FirePulses_111,   ///<
        TDC_GP22_FirePulses_112,   ///<
        TDC_GP22_FirePulses_113,   ///<
        TDC_GP22_FirePulses_114,   ///<
        TDC_GP22_FirePulses_115,   ///<
        TDC_GP22_FirePulses_116,   ///<
        TDC_GP22_FirePulses_117,   ///<
        TDC_GP22_FirePulses_118,   ///<
        TDC_GP22_FirePulses_119,   ///<
        TDC_GP22_FirePulses_120,   ///<
        TDC_GP22_FirePulses_121,   ///<
        TDC_GP22_FirePulses_122,   ///<
        TDC_GP22_FirePulses_123,   ///<
        TDC_GP22_FirePulses_124,   ///<
        TDC_GP22_FirePulses_125,   ///<
        TDC_GP22_FirePulses_126,   ///<
        TDC_GP22_FirePulses_127,   ///< 127 Pulses (Maximum)
    } TDC_GP22_FirePulses_t;

    /**
     *  @brief TDC GP22 Fire Divider Type
     *
     *  @enum TDC_GP22_FireDivider_t
     */
    typedef enum TDC_GP22_FireDivider
    {
        TDC_GP22_FireDivider_2 = 1, ///< Divider 2 (Minimum)
        TDC_GP22_FireDivider_3,     ///<
        TDC_GP22_FireDivider_4,     ///<
        TDC_GP22_FireDivider_5,     ///<
        TDC_GP22_FireDivider_6,     ///<
        TDC_GP22_FireDivider_7,     ///<
        TDC_GP22_FireDivider_8,     ///<
        TDC_GP22_FireDivider_9,     ///<
        TDC_GP22_FireDivider_10,    ///<
        TDC_GP22_FireDivider_11,    ///<
        TDC_GP22_FireDivider_12,    ///<
        TDC_GP22_FireDivider_13,    ///<
        TDC_GP22_FireDivider_14,    ///<
        TDC_GP22_FireDivider_15,    ///<
        TDC_GP22_FireDivider_16,    ///< Divider 16 (Maximum)
    } TDC_GP22_FireDivider_t;

    /**
     *  @brief TDC GP22 Calibration Period Type
     *
     *  @enum TDC_GP22_CalibrationPeriod_t
     */
    typedef enum TDC_GP22_CalibrationPeriod
    {
        TDC_GP22_CalibrationPeriod_2 = 0, ///< 2 Periods (Minimum)
        TDC_GP22_CalibrationPeriod_4,     ///< 4 Periods
        TDC_GP22_CalibrationPeriod_8,     ///< 8 Periods
        TDC_GP22_CalibrationPeriod_16,    ///< 16 Periods (Maximum)
    } TDC_GP22_CalibrationPeriod_t;

    /**
     *  @brief TDC GP22 Oscillator Divider Type
     *
     *  @enum TDC_GP22_OscillatorDivider_t
     */
    typedef enum TDC_GP22_OscillatorDivider
    {
        TDC_GP22_OscillatorDivider_1 = 0, ///< Divider 1 (Minimum)
        TDC_GP22_OscillatorDivider_2,     ///< Divider 2
        TDC_GP22_OscillatorDivider_4,     ///< Divider 4 (Maximum)
    } TDC_GP22_OscillatorDivider_t;

    /**
     *  @brief TDC GP22 Oscillator Interval Type
     *
     *  @enum TDC_GP22_OscillatorInterval_t
     */
    typedef enum TDC_GP22_OscillatorInterval
    {
        TDC_GP22_OscillatorInterval_Off = 0,    ///< Off
        TDC_GP22_OscillatorInterval_Continuous, ///< Continuous
        TDC_GP22_OscillatorInterval_480us,      ///<  480 us
        TDC_GP22_OscillatorInterval_1460us,     ///< 1460 us
        TDC_GP22_OscillatorInterval_2440us,     ///< 2440 us
        TDC_GP22_OscillatorInterval_5140us,     ///< 5140 us
    } TDC_GP22_OscillatorInterval_t;

    /**
     *  @brief TDC GP22 Temperature Ports Type
     *
     *  @enum TDC_GP22_TemperaturePorts_t
     */
    typedef enum TDC_GP22_TemperaturePorts
    {
        TDC_GP22_TemperaturePorts_2 = 0, ///< 2 Ports (Minimum)
        TDC_GP22_TemperaturePorts_4,     ///< 4 Ports (Maximum)
    } TDC_GP22_TemperaturePorts_t;

    /**
     *  @brief TDC GP22 Temperature Interval Type
     *
     *  @enum TDC_GP22_TemperatureInterval_t
     */
    typedef enum TDC_GP22_TemperatureInterval
    {
        TDC_GP22_TemperatureInterval_128us = 0, ///< 128 us
        TDC_GP22_TemperatureInterval_512us,     ///< 512 us
    } TDC_GP22_TemperatureInterval_t;

    /**
     *  @brief TDC GP22 Temperature Dummy Interval Type
     *
     *  @enum TDC_GP22_TemperatureDummyInterval_t
     */
    typedef enum TDC_GP22_TemperatureDummyInterval
    {
        TDC_GP22_TemperatureDummyInterval_2 = 0, ///<
        TDC_GP22_TemperatureDummyInterval_7,     ///<
    } TDC_GP22_TemperatureDummyInterval_t;

    /**
     *  @brief TDC GP22 Temperature Clock Source Type
     *
     *  @enum TDC_GP22_TemperatureClockSource_t
     */
    typedef enum TDC_GP22_TemperatureClockSource
    {
        TDC_GP22_TemperatureClockSource_32768Hz = 0, ///< 32.768 kHz
        TDC_GP22_TemperatureClockSource_4MHz,        ///< 4.0 MHz
    } TDC_GP22_TemperatureClockSource_t;

    /**
     *  @brief TDC GP22 Calibration Type
     *
     *  @enum TDC_GP22_Calibration_t
     */
    typedef enum TDC_GP22_Calibration
    {
        TDC_GP22_Calibrate_Off = 0, ///< Off
        TDC_GP22_Calibrate_On,      ///< On
    } TDC_GP22_Calibration_t;

    /**
     *  @brief TDC GP22 Auto Calibration Type
     *
     *  @enum TDC_GP22_AutoCalibration_t
     */
    typedef enum TDC_GP22_AutoCalibration
    {
        TDC_GP22_AutoCalibration_On = 0, ///< On
        TDC_GP22_AutoCalibration_Off,    ///< Off
    } TDC_GP22_AutoCalibration_t;

    /**
     *  @brief TDC GP22 Mode Type
     *
     *  @enum TDC_GP22_Mode_t
     */
    typedef enum TDC_GP22_Mode
    {
        TDC_GP22_Mode_1 = 0, ///< Mode 1
        TDC_GP22_Mode_2,     ///< Mode 2
    } TDC_GP22_Mode_t;

    /**
     *  @brief TDC GP22 Stop 2 Signal Mode Type
     *
     *  @enum TDC_GP22_Stop2SignalMode_t
     */
    typedef enum TDC_GP22_Stop2SignalMode
    {
        TDC_GP22_Stop2SignalMode_Rising = 0, ///< Rising
        TDC_GP22_Stop2SignalMode_Falling,    ///< Falling
    } TDC_GP22_Stop2SignalMode_t;

    /**
     *  @brief TDC GP22 Stop 1 Signal Mode Type
     *
     *  @enum TDC_GP22_Stop1SignalMode_t
     */
    typedef enum TDC_GP22_Stop1SignalMode
    {
        TDC_GP22_Stop1SignalMode_Rising = 0, ///< Rising
        TDC_GP22_Stop1SignalMode_Falling,    ///< Falling
    } TDC_GP22_Stop1SignalMode_t;

    /**
     *  @brief TDC GP22 Start Signal Mode Type
     *
     *  @enum TDC_GP22_StartSignalMode_t
     */
    typedef enum TDC_GP22_StartSignalMode
    {
        TDC_GP22_StartSignalMode_Rising = 0, ///< Rising
        TDC_GP22_StartSignalMode_Falling,    ///< Falling
    } TDC_GP22_StartSignalMode_t;

    /**
     *  @brief TDC GP22 HIT 2 Operator Type
     *
     *  @note Contains configuration values that are context-dependent.
     *        The same numeric value may have different meanings depending on the
     *        operational mode @ref TDC_GP22_Mode_t.
     *
     *  @enum TDC_GP22_HIT_2_Operator_t
     */
    typedef enum TDC_GP22_HIT_2Operator
    {
        // In measurement mode-1
        TDC_GP22_HIT_2_Operator_Mode_1_Start = 0, ///< Mode 1: Start

        TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_1, ///< Mode 1: Channel ONE 1st Stop
        TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_2, ///< Mode 1: Channel ONE 2nd Stop
        TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_3, ///< Mode 1: Channel ONE 3rd Stop
        TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Stop_4, ///< Mode 1: Channel ONE 4th Stop

        TDC_GP22_HIT_2_Operator_Mode_1_No_Action, ///< Mode 1: No Action

        TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Calibration_1, ///< Mode 1: Channel ONE 1st Calibration
        TDC_GP22_HIT_2_Operator_Mode_1_Channel_1_Calibration_2, ///< Mode 1: Channel ONE 2nd Calibration

        // Note: Value 8 is not used !!

        TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_1 = 9, ///< Mode 1: Channel TWO 1st Stop
        TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_2,     ///< Mode 1: Channel TWO 2nd Stop
        TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_3,     ///< Mode 1: Channel TWO 3rd Stop
        TDC_GP22_HIT_2_Operator_Mode_1_Channel_2_Stop_4,     ///< Mode 1: Channel TWO 4th Stop

        // In measurement mode-2
        TDC_GP22_HIT_2_Operator_Mode_2_Channel_1_Stop_1 = 2, ///< Mode 2: Channel ONE 1st Stop
        TDC_GP22_HIT_2_Operator_Mode_2_Channel_1_Stop_2,     ///< Mode 2: Channel ONE 2nd Stop
        TDC_GP22_HIT_2_Operator_Mode_2_Channel_1_Stop_3,     ///< Mode 2: Channel ONE 3rd Stop
    } TDC_GP22_HIT_2_Operator_t;

    /**
     *  @brief TDC GP22 HIT 1 Operator Type
     *
     *  @note Contains configuration values that are context-dependent.
     *        The same numeric value may have different meanings depending on the
     *        operational mode @ref TDC_GP22_Mode_t.
     *
     *  @enum TDC_GP22_HIT_1_Operator_t
     */
    typedef enum TDC_GP22_HIT_1_Operator
    {
        // In measurement mode-1
        TDC_GP22_HIT_1_Operator_Mode_1_Start = 0, ///< Mode 1: Start

        TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_1, ///< Mode 1: Channel ONE 1st Stop
        TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_2, ///< Mode 1: Channel ONE 2nd Stop
        TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_3, ///< Mode 1: Channel ONE 3rd Stop
        TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Stop_4, ///< Mode 1: Channel ONE 4th Stop

        TDC_GP22_HIT_1_Operator_Mode_1_No_Action, ///< Mode 1: No Action

        TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Calibration_1, ///< Mode 1: Channel ONE 1st Calibration
        TDC_GP22_HIT_1_Operator_Mode_1_Channel_1_Calibration_2, ///< Mode 1: Channel ONE 2nd Calibration

        // Note: Value 8 is not used !!

        TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_1 = 9, ///< Mode 1: Channel TWO 1st Stop
        TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_2,     ///< Mode 1: Channel TWO 2nd Stop
        TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_3,     ///< Mode 1: Channel TWO 3rd Stop
        TDC_GP22_HIT_1_Operator_Mode_1_Channel_2_Stop_4,     ///< Mode 1: Channel TWO 4th Stop

        // In measurement mode-2
        TDC_GP22_HIT_1_Operator_Mode_2_Start = 1, ///< Mode 2: Start
    } TDC_GP22_HIT_1_Operator_t;

    /**
     *  @brief TDC GP22 Fast Init Type
     *
     *  @enum TDC_GP22_FastInit_t
     */
    typedef enum TDC_GP22_FastInit
    {
        TDC_GP22_FastInit_Disable = 0, ///< Disable
        TDC_GP22_FastInit_Enable,      ///< Enable
    } TDC_GP22_FastInit_t;

    /**
     *  @brief TDC GP22 Channel 2 Hits Type
     *
     *  @enum TDC_GP22_Channel2Hits_t
     */
    typedef enum TDC_GP22_Channel2Hits
    {
        TDC_GP22_Channel2Hits_0 = 0, ///< Zero Hits
        TDC_GP22_Channel2Hits_1,     ///< 1 Hit
        TDC_GP22_Channel2Hits_2,     ///< 2 Hits
        TDC_GP22_Channel2Hits_3,     ///< 3 Hits
        TDC_GP22_Channel2Hits_4,     ///< 4 Hits

        // Note: Values 5-7 not permitted !!
    } TDC_GP22_Channel2Hits_t;

    /**
     *  @brief TDC GP22 Channel 1 Hits Type
     *
     *  @enum TDC_GP22_Channel1Hits_t
     */
    typedef enum TDC_GP22_Channel1Hits
    {
        TDC_GP22_Channel1Hits_0 = 0, ///< Zero Hits
        TDC_GP22_Channel1Hits_1,     ///< 1 Hit
        TDC_GP22_Channel1Hits_2,     ///< 2 Hits
        TDC_GP22_Channel1Hits_3,     ///< 3 Hits
        TDC_GP22_Channel1Hits_4,     ///< 4 Hits

        // Note: Values 5-7 not permitted !!
    } TDC_GP22_Channel1Hits_t;

    /**
     *  @brief TDC GP22 Current Mode Type
     *
     *  @enum TDC_GP22_CurrentMode_t
     */
    typedef enum TDC_GP22_CurrentMode
    {
        TDC_GP22_CurrentMode_Low = 0, ///< Low
        TDC_GP22_CurrentMode_High,    ///< High
    } TDC_GP22_CurrentMode_t;

    typedef enum TDC_GP22_StartFire
    {
        TDC_GP22_StartFire_GP2 = 0,
        TDC_GP22_StartFire_FireAsStart,
    } TDC_GP22_StartFire_t;

    typedef enum TDC_GP22_StartEnable
    {
        TDC_GP22_StartEnable_GP2 = 0,
        TDC_GP22_StartEnable_Start,
        TDC_GP22_StartEnable_Stop_1,
        TDC_GP22_StartEnable_Stop_2,
        TDC_GP22_StartEnable_Stop_Temperature,
        TDC_GP22_StartEnable_TimeOfFlight,

        // Note: Value 6 is not used !!

        TDC_GP22_StartEnable_4kHz = 7,
    } TDC_GP22_StartEnable_t;

    typedef enum TDC_GP22_FireInput
    {
        TDC_GP22_FireInput_GP2 = 0,
        TDC_GP22_FireInput_Start,
        TDC_GP22_FireInput_Stop_1,
        TDC_GP22_FireInput_Stop_2,
        TDC_GP22_FireInput_Start_Temperature,
        TDC_GP22_FireInput_Stop_Enable,
        TDC_GP22_FireInput_Comparator,
        TDC_GP22_FireInput_32kHz,
    } TDC_GP22_FireInput_t;

    typedef enum TDC_GP22_InterruptEnable
    {
        TDC_GP22_InterruptEnable_Timeout = 0x04,
        TDC_GP22_InterruptEnable_End_Hits = 0x02,
        TDC_GP22_InterruptEnable_ALU = 0x01,
        TDC_GP22_InterruptEnable_End_EEPROM = 0x08,
    } TDC_GP22_InterruptEnable_t;

    typedef enum TDC_GP22_Channel2SenseEdge
    {
        TDC_GP22_Channel2SenseEdge_Rising_OR_Falling = 0,
        TDC_GP22_Channel2SenseEdge_Rising_AND_Falling,
    } TDC_GP22_Channel2SenseEdge_t;

    typedef enum TDC_GP22_Channel1SenseEdge
    {
        TDC_GP22_Channel1SenseEdge_Rising_OR_Falling = 0,
        TDC_GP22_Channel1SenseEdge_Rising_AND_Falling,
    } TDC_GP22_Channel1SenseEdge_t;

    typedef uint32_t TDC_GP22_Channel1DelayValue_t;

    typedef enum TDC_GP22_AutomaticCalculation
    {
        TDC_GP22_AutomaticCalculation_Disable = 0,
        TDC_GP22_AutomaticCalculation_Enable,
    } TDC_GP22_AutomaticCalculation_t;

    typedef enum TDC_GP22_ErrorValue
    {
        TDC_GP22_ErrorValue_Disable = 0,
        TDC_GP22_ErrorValue_Enable,
    } TDC_GP22_ErrorValue_t;

    typedef enum TDC_GP22_FirstWave
    {
        TDC_GP22_FirstWave_Disable = 0,
        TDC_GP22_FirstWave_Enable,
    } TDC_GP22_FirstWave_t;

    typedef enum TDC_GP22_TimeoutDivider
    {
        TDC_GP22_TimeoutDivider_64us = 0,
        TDC_GP22_TimeoutDivider_256us,
        TDC_GP22_TimeoutDivider_1024us,
        TDC_GP22_TimeoutDivider_4096us,
    } TDC_GP22_TimeoutDivider_t;

    typedef enum TDC_GP22_NumberOfPeriodsStop3
    {
        TDC_GP22_NumberOfPeriodsStop3_5 = 5,
        TDC_GP22_NumberOfPeriodsStop3_6,
        TDC_GP22_NumberOfPeriodsStop3_7,
        TDC_GP22_NumberOfPeriodsStop3_8,
        TDC_GP22_NumberOfPeriodsStop3_9,
        TDC_GP22_NumberOfPeriodsStop3_10,
        TDC_GP22_NumberOfPeriodsStop3_11,
        TDC_GP22_NumberOfPeriodsStop3_12,
        TDC_GP22_NumberOfPeriodsStop3_13,
        TDC_GP22_NumberOfPeriodsStop3_14,
        TDC_GP22_NumberOfPeriodsStop3_15,
        TDC_GP22_NumberOfPeriodsStop3_16,
        TDC_GP22_NumberOfPeriodsStop3_17,
        TDC_GP22_NumberOfPeriodsStop3_18,
        TDC_GP22_NumberOfPeriodsStop3_19,
        TDC_GP22_NumberOfPeriodsStop3_20,
        TDC_GP22_NumberOfPeriodsStop3_21,
        TDC_GP22_NumberOfPeriodsStop3_22,
        TDC_GP22_NumberOfPeriodsStop3_23,
        TDC_GP22_NumberOfPeriodsStop3_24,
        TDC_GP22_NumberOfPeriodsStop3_25,
        TDC_GP22_NumberOfPeriodsStop3_26,
        TDC_GP22_NumberOfPeriodsStop3_27,
        TDC_GP22_NumberOfPeriodsStop3_28,
        TDC_GP22_NumberOfPeriodsStop3_29,
        TDC_GP22_NumberOfPeriodsStop3_30,
        TDC_GP22_NumberOfPeriodsStop3_31,
        TDC_GP22_NumberOfPeriodsStop3_32,
        TDC_GP22_NumberOfPeriodsStop3_33,
        TDC_GP22_NumberOfPeriodsStop3_34,
        TDC_GP22_NumberOfPeriodsStop3_35,
        TDC_GP22_NumberOfPeriodsStop3_36,
        TDC_GP22_NumberOfPeriodsStop3_37,
        TDC_GP22_NumberOfPeriodsStop3_38,
        TDC_GP22_NumberOfPeriodsStop3_39,
        TDC_GP22_NumberOfPeriodsStop3_40,
        TDC_GP22_NumberOfPeriodsStop3_41,
        TDC_GP22_NumberOfPeriodsStop3_42,
        TDC_GP22_NumberOfPeriodsStop3_43,
        TDC_GP22_NumberOfPeriodsStop3_44,
        TDC_GP22_NumberOfPeriodsStop3_45,
        TDC_GP22_NumberOfPeriodsStop3_46,
        TDC_GP22_NumberOfPeriodsStop3_47,
        TDC_GP22_NumberOfPeriodsStop3_48,
        TDC_GP22_NumberOfPeriodsStop3_49,
        TDC_GP22_NumberOfPeriodsStop3_50,
        TDC_GP22_NumberOfPeriodsStop3_51,
        TDC_GP22_NumberOfPeriodsStop3_52,
        TDC_GP22_NumberOfPeriodsStop3_53,
        TDC_GP22_NumberOfPeriodsStop3_54,
        TDC_GP22_NumberOfPeriodsStop3_55,
        TDC_GP22_NumberOfPeriodsStop3_56,
        TDC_GP22_NumberOfPeriodsStop3_57,
        TDC_GP22_NumberOfPeriodsStop3_58,
        TDC_GP22_NumberOfPeriodsStop3_59,
        TDC_GP22_NumberOfPeriodsStop3_60,
        TDC_GP22_NumberOfPeriodsStop3_61,
        TDC_GP22_NumberOfPeriodsStop3_62,
        TDC_GP22_NumberOfPeriodsStop3_63,
    } TDC_GP22_NumberOfPeriodsStop3_t;

    typedef enum TDC_GP22_NumberOfPeriodsStop2
    {
        TDC_GP22_NumberOfPeriodsStop2_4 = 4,
        TDC_GP22_NumberOfPeriodsStop2_5,
        TDC_GP22_NumberOfPeriodsStop2_6,
        TDC_GP22_NumberOfPeriodsStop2_7,
        TDC_GP22_NumberOfPeriodsStop2_8,
        TDC_GP22_NumberOfPeriodsStop2_9,
        TDC_GP22_NumberOfPeriodsStop2_10,
        TDC_GP22_NumberOfPeriodsStop2_11,
        TDC_GP22_NumberOfPeriodsStop2_12,
        TDC_GP22_NumberOfPeriodsStop2_13,
        TDC_GP22_NumberOfPeriodsStop2_14,
        TDC_GP22_NumberOfPeriodsStop2_15,
        TDC_GP22_NumberOfPeriodsStop2_16,
        TDC_GP22_NumberOfPeriodsStop2_17,
        TDC_GP22_NumberOfPeriodsStop2_18,
        TDC_GP22_NumberOfPeriodsStop2_19,
        TDC_GP22_NumberOfPeriodsStop2_20,
        TDC_GP22_NumberOfPeriodsStop2_21,
        TDC_GP22_NumberOfPeriodsStop2_22,
        TDC_GP22_NumberOfPeriodsStop2_23,
        TDC_GP22_NumberOfPeriodsStop2_24,
        TDC_GP22_NumberOfPeriodsStop2_25,
        TDC_GP22_NumberOfPeriodsStop2_26,
        TDC_GP22_NumberOfPeriodsStop2_27,
        TDC_GP22_NumberOfPeriodsStop2_28,
        TDC_GP22_NumberOfPeriodsStop2_29,
        TDC_GP22_NumberOfPeriodsStop2_30,
        TDC_GP22_NumberOfPeriodsStop2_31,
        TDC_GP22_NumberOfPeriodsStop2_32,
        TDC_GP22_NumberOfPeriodsStop2_33,
        TDC_GP22_NumberOfPeriodsStop2_34,
        TDC_GP22_NumberOfPeriodsStop2_35,
        TDC_GP22_NumberOfPeriodsStop2_36,
        TDC_GP22_NumberOfPeriodsStop2_37,
        TDC_GP22_NumberOfPeriodsStop2_38,
        TDC_GP22_NumberOfPeriodsStop2_39,
        TDC_GP22_NumberOfPeriodsStop2_40,
        TDC_GP22_NumberOfPeriodsStop2_41,
        TDC_GP22_NumberOfPeriodsStop2_42,
        TDC_GP22_NumberOfPeriodsStop2_43,
        TDC_GP22_NumberOfPeriodsStop2_44,
        TDC_GP22_NumberOfPeriodsStop2_45,
        TDC_GP22_NumberOfPeriodsStop2_46,
        TDC_GP22_NumberOfPeriodsStop2_47,
        TDC_GP22_NumberOfPeriodsStop2_48,
        TDC_GP22_NumberOfPeriodsStop2_49,
        TDC_GP22_NumberOfPeriodsStop2_50,
        TDC_GP22_NumberOfPeriodsStop2_51,
        TDC_GP22_NumberOfPeriodsStop2_52,
        TDC_GP22_NumberOfPeriodsStop2_53,
        TDC_GP22_NumberOfPeriodsStop2_54,
        TDC_GP22_NumberOfPeriodsStop2_55,
        TDC_GP22_NumberOfPeriodsStop2_56,
        TDC_GP22_NumberOfPeriodsStop2_57,
        TDC_GP22_NumberOfPeriodsStop2_58,
        TDC_GP22_NumberOfPeriodsStop2_59,
        TDC_GP22_NumberOfPeriodsStop2_60,
        TDC_GP22_NumberOfPeriodsStop2_61,
        TDC_GP22_NumberOfPeriodsStop2_62,
        TDC_GP22_NumberOfPeriodsStop2_63,
    } TDC_GP22_NumberOfPeriodsStop2_t;

    typedef enum TDC_GP22_NumberOfPeriodsStop1
    {
        TDC_GP22_NumberOfPeriodsStop1_3 = 3,
        TDC_GP22_NumberOfPeriodsStop1_4,
        TDC_GP22_NumberOfPeriodsStop1_5,
        TDC_GP22_NumberOfPeriodsStop1_6,
        TDC_GP22_NumberOfPeriodsStop1_7,
        TDC_GP22_NumberOfPeriodsStop1_8,
        TDC_GP22_NumberOfPeriodsStop1_9,
        TDC_GP22_NumberOfPeriodsStop1_10,
        TDC_GP22_NumberOfPeriodsStop1_11,
        TDC_GP22_NumberOfPeriodsStop1_12,
        TDC_GP22_NumberOfPeriodsStop1_13,
        TDC_GP22_NumberOfPeriodsStop1_14,
        TDC_GP22_NumberOfPeriodsStop1_15,
        TDC_GP22_NumberOfPeriodsStop1_16,
        TDC_GP22_NumberOfPeriodsStop1_17,
        TDC_GP22_NumberOfPeriodsStop1_18,
        TDC_GP22_NumberOfPeriodsStop1_19,
        TDC_GP22_NumberOfPeriodsStop1_20,
        TDC_GP22_NumberOfPeriodsStop1_21,
        TDC_GP22_NumberOfPeriodsStop1_22,
        TDC_GP22_NumberOfPeriodsStop1_23,
        TDC_GP22_NumberOfPeriodsStop1_24,
        TDC_GP22_NumberOfPeriodsStop1_25,
        TDC_GP22_NumberOfPeriodsStop1_26,
        TDC_GP22_NumberOfPeriodsStop1_27,
        TDC_GP22_NumberOfPeriodsStop1_28,
        TDC_GP22_NumberOfPeriodsStop1_29,
        TDC_GP22_NumberOfPeriodsStop1_30,
        TDC_GP22_NumberOfPeriodsStop1_31,
        TDC_GP22_NumberOfPeriodsStop1_32,
        TDC_GP22_NumberOfPeriodsStop1_33,
        TDC_GP22_NumberOfPeriodsStop1_34,
        TDC_GP22_NumberOfPeriodsStop1_35,
        TDC_GP22_NumberOfPeriodsStop1_36,
        TDC_GP22_NumberOfPeriodsStop1_37,
        TDC_GP22_NumberOfPeriodsStop1_38,
        TDC_GP22_NumberOfPeriodsStop1_39,
        TDC_GP22_NumberOfPeriodsStop1_40,
        TDC_GP22_NumberOfPeriodsStop1_41,
        TDC_GP22_NumberOfPeriodsStop1_42,
        TDC_GP22_NumberOfPeriodsStop1_43,
        TDC_GP22_NumberOfPeriodsStop1_44,
        TDC_GP22_NumberOfPeriodsStop1_45,
        TDC_GP22_NumberOfPeriodsStop1_46,
        TDC_GP22_NumberOfPeriodsStop1_47,
        TDC_GP22_NumberOfPeriodsStop1_48,
        TDC_GP22_NumberOfPeriodsStop1_49,
        TDC_GP22_NumberOfPeriodsStop1_50,
        TDC_GP22_NumberOfPeriodsStop1_51,
        TDC_GP22_NumberOfPeriodsStop1_52,
        TDC_GP22_NumberOfPeriodsStop1_53,
        TDC_GP22_NumberOfPeriodsStop1_54,
        TDC_GP22_NumberOfPeriodsStop1_55,
        TDC_GP22_NumberOfPeriodsStop1_56,
        TDC_GP22_NumberOfPeriodsStop1_57,
        TDC_GP22_NumberOfPeriodsStop1_58,
        TDC_GP22_NumberOfPeriodsStop1_59,
        TDC_GP22_NumberOfPeriodsStop1_60,
        TDC_GP22_NumberOfPeriodsStop1_61,
        TDC_GP22_NumberOfPeriodsStop1_62,
        TDC_GP22_NumberOfPeriodsStop1_63,
    } TDC_GP22_NumberOfPeriodsStop1_t;

    typedef uint32_t TDC_GP22_Channel2DelayValue_t;

    typedef uint32_t TDC_GP22_Channel3DelayValue_t;

    typedef enum TDC_GP22_PulseWidthMeasurement
    {
        TDC_GP22_PulseWidthMeasurement_Enable = 0,
        TDC_GP22_PulseWidthMeasurement_Disable,
    } TDC_GP22_PulseWidthMeasurement_t;

    typedef enum TDC_GP22_FirstWaveEdge
    {
        TDC_GP22_FirstWaveEdge_Rising = 0,
        TDC_GP22_FirstWaveEdge_Falling,
    } TDC_GP22_FirstWaveEdge_t;

    typedef enum TDC_GP22_AdditionalOffsetShiftPositive
    {
        // + 20mV
        TDC_GP22_AdditionalOffsetShiftPositive_Off = 0,
        TDC_GP22_AdditionalOffsetShiftPositive_Active,
    } TDC_GP22_AdditionalOffsetShiftPositive_t;

    typedef enum TDC_GP22_AdditionalOffsetShiftNegative
    {
        // - 20mV
        TDC_GP22_AdditionalOffsetShiftNegative_Off = 0,
        TDC_GP22_AdditionalOffsetShiftNegative_Active,
    } TDC_GP22_AdditionalOffsetShiftNegative_t;

    typedef enum TDC_GP22_OffsetShift
    {
        TDC_GP22_OffsetShift_16mV_Negative = -16,
        TDC_GP22_OffsetShift_15mV_Negative,
        TDC_GP22_OffsetShift_14mV_Negative,
        TDC_GP22_OffsetShift_13mV_Negative,
        TDC_GP22_OffsetShift_12mV_Negative,
        TDC_GP22_OffsetShift_11mV_Negative,
        TDC_GP22_OffsetShift_10mV_Negative,
        TDC_GP22_OffsetShift_9mV_Negative,
        TDC_GP22_OffsetShift_8mV_Negative,
        TDC_GP22_OffsetShift_7mV_Negative,
        TDC_GP22_OffsetShift_6mV_Negative,
        TDC_GP22_OffsetShift_5mV_Negative,
        TDC_GP22_OffsetShift_4mV_Negative,
        TDC_GP22_OffsetShift_3mV_Negative,
        TDC_GP22_OffsetShift_2mV_Negative,
        TDC_GP22_OffsetShift_1mV_Negative,

        TDC_GP22_OffsetShift_0mV,

        TDC_GP22_OffsetShift_1mV_Positive,
        TDC_GP22_OffsetShift_2mV_Positive,
        TDC_GP22_OffsetShift_3mV_Positive,
        TDC_GP22_OffsetShift_4mV_Positive,
        TDC_GP22_OffsetShift_5mV_Positive,
        TDC_GP22_OffsetShift_6mV_Positive,
        TDC_GP22_OffsetShift_7mV_Positive,
        TDC_GP22_OffsetShift_8mV_Positive,
        TDC_GP22_OffsetShift_9mV_Positive,
        TDC_GP22_OffsetShift_10mV_Positive,
        TDC_GP22_OffsetShift_11mV_Positive,
        TDC_GP22_OffsetShift_12mV_Positive,
        TDC_GP22_OffsetShift_13mV_Positive,
        TDC_GP22_OffsetShift_14mV_Positive,
        TDC_GP22_OffsetShift_15mV_Positive,
    } TDC_GP22_OffsetShift_t;

    typedef enum TDC_GP22_FireDirection
    {
        TDC_GP22_FireDirection_Both = 0x04, // TODO Check in operation
        TDC_GP22_FireDirection_Up = 0x02,
        TDC_GP22_FireDirection_Down = 0x01,
    } TDC_GP22_FireDirection_t;

    typedef enum TDC_GP22_NoiseStart
    {
        TDC_GP22_NoiseStart_Disable = 0,
        TDC_GP22_NoiseStart_Enable,
    } TDC_GP22_NoiseStart_t;

    typedef enum TDC_GP22_PhaseShift
    {
        TDC_GP22_PhaseShift_Enable = 0,
        TDC_GP22_PhaseShift_Disable,
    } TDC_GP22_PhaseShift_t;

    typedef enum TDC_GP22_NumberOfPulseRepetition
    {
        TDC_GP22_NumberOfPulseRepetition_None = 0,
        TDC_GP22_NumberOfPulseRepetition_1,
        TDC_GP22_NumberOfPulseRepetition_2,
        TDC_GP22_NumberOfPulseRepetition_3,
        TDC_GP22_NumberOfPulseRepetition_4,
        TDC_GP22_NumberOfPulseRepetition_5,
        TDC_GP22_NumberOfPulseRepetition_6,
        TDC_GP22_NumberOfPulseRepetition_7,
    } TDC_GP22_NumberOfPulseRepetition_t;

    typedef enum TDC_GP22_PhaseInversionPulse
    {
        TDC_GP22_PhaseInversionPulse_0 = 0,
        TDC_GP22_PhaseInversionPulse_1,
        TDC_GP22_PhaseInversionPulse_2,
        TDC_GP22_PhaseInversionPulse_3,
        TDC_GP22_PhaseInversionPulse_4,
        TDC_GP22_PhaseInversionPulse_5,
        TDC_GP22_PhaseInversionPulse_6,
        TDC_GP22_PhaseInversionPulse_7,
        TDC_GP22_PhaseInversionPulse_8,
        TDC_GP22_PhaseInversionPulse_9,
        TDC_GP22_PhaseInversionPulse_10,
        TDC_GP22_PhaseInversionPulse_11,
        TDC_GP22_PhaseInversionPulse_12,
        TDC_GP22_PhaseInversionPulse_13,
        TDC_GP22_PhaseInversionPulse_14,
    } TDC_GP22_PhaseInversionPulse_t;

    typedef enum TDC_GP22_PhaseInversion
    {
        TDC_GP22_PhaseInversion_HighToLow = 0,
        TDC_GP22_PhaseInversion_LowToHigh,
    } TDC_GP22_PhaseInversion_t;

    typedef enum TDC_GP22_AnalogEnable
    {
        TDC_GP22_AnalogEnable_Disable = 0,
        TDC_GP22_AnalogEnable_Enable,
    } TDC_GP22_AnalogEnable_t;

    typedef enum TDC_GP22_TemperatureInput
    {
        TDC_GP22_TemperatureInput_External = 0, // 74HC14
        TDC_GP22_TemperatureInput_Internal,     // SchmittTrigger
    } TDC_GP22_TemperatureInput_t;

    typedef enum TDC_GP22_ComparatorOffset
    {
        TDC_GP22_ComparatorOffset_8mV_Negative = -8,
        TDC_GP22_ComparatorOffset_7mV_Negative,
        TDC_GP22_ComparatorOffset_6mV_Negative,
        TDC_GP22_ComparatorOffset_5mV_Negative,
        TDC_GP22_ComparatorOffset_4mV_Negative,
        TDC_GP22_ComparatorOffset_3mV_Negative,
        TDC_GP22_ComparatorOffset_2mV_Negative,
        TDC_GP22_ComparatorOffset_1mV_Negative,

        TDC_GP22_ComparatorOffset_0mV,

        TDC_GP22_ComparatorOffset_1mV_Positive,
        TDC_GP22_ComparatorOffset_2mV_Positive,
        TDC_GP22_ComparatorOffset_3mV_Positive,
        TDC_GP22_ComparatorOffset_4mV_Positive,
        TDC_GP22_ComparatorOffset_5mV_Positive,
        TDC_GP22_ComparatorOffset_6mV_Positive,
        TDC_GP22_ComparatorOffset_7mV_Positive,
    } TDC_GP22_ComparatorOffset_t;

    typedef enum TDC_GP22_ChargeTime
    {
        TDC_GP22_ChargeTime_90us = 0,
        TDC_GP22_ChargeTime_120us,
        TDC_GP22_ChargeTime_150us,
        TDC_GP22_ChargeTime_300us,
    } TDC_GP22_ChargeTime_t;

    typedef enum TDC_GP22_TemperatureCycle
    {
        TDC_GP22_TemperatureCycle_1_0 = 0,
        TDC_GP22_TemperatureCycle_1_5,
        TDC_GP22_TemperatureCycle_2_0,
        TDC_GP22_TemperatureCycle_2_5,
    } TDC_GP22_TemperatureCycle_t;

    typedef enum TDC_GP22_TimeOfFlightCycle
    {
        TDC_GP22_TimeOfFlightCycle_1_0 = 0,
        TDC_GP22_TimeOfFlightCycle_1_5,
        TDC_GP22_TimeOfFlightCycle_2_0,
        TDC_GP22_TimeOfFlightCycle_2_5,
    } TDC_GP22_TimeOfFlightCycle_t;

    typedef enum TDC_GP22_Cycle
    {
        TDC_GP22_Cycle_50Hz = 0,
        TDC_GP22_Cycle_60Hz,
    } TDC_GP22_Cycle_t;

    typedef enum TDC_GP22_DefaultFireLevel
    {
        TDC_GP22_DefaultFireLevel_High_Z = 0,
        TDC_GP22_DefaultFireLevel_Low,
    } TDC_GP22_DefaultFireLevel_t;

    typedef enum TDC_GP22_Resolution
    {
        TDC_GP22_Resolution_Off = 0,
        TDC_GP22_Resolution_2x,
        TDC_GP22_Resolution_4x,
    } TDC_GP22_Resolution_t;

    typedef enum TDC_GP22_TemperatureMeasurementDirection
    {
        TDC_GP22_TemperatureMeasurementDirection_1To4 = 0,
        TDC_GP22_TemperatureMeasurementDirection_4To1,
    } TDC_GP22_TemperatureMeasurementDirection_t;

    typedef struct TDC_GP22_OperationalStatus
    {
        bool EEPROM_Matches_Configuration;
        bool EEPROM_Multi_Error;
        bool EEPROM_Error;
        bool Temperature_Sensor_Short;
        bool Temperature_Sensor_Open;
        bool Timeout_PreCounter;
        bool Timeout_TDC;
        uint8_t Channel2NumberOfHits;
        uint8_t Channel1NumberOfHits;
        uint8_t ALU_Operation_Pointer;
    } TDC_GP22_OperationalStatus_t;

    typedef double TDC_GP22_Measurement_t;

    typedef struct TDC_GP22_Instance TDC_GP22_Instance_t;

    typedef void ( *TDC_GP22_CallbackOnComplete_t )( TDC_GP22_Instance_t * Instance, TDC_GP22_Status_t Status );

    typedef void ( *TDC_GP22_CallbackOnMeasurement_0_t )( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDirection_t FireDirection, TDC_GP22_Measurement_t Measurement );

    typedef void ( *TDC_GP22_CallbackOnMeasurement_1_t )( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDirection_t FireDirection, TDC_GP22_Measurement_t Measurement );

    typedef void ( *TDC_GP22_CallbackOnMeasurement_2_t )( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDirection_t FireDirection, TDC_GP22_Measurement_t Measurement );

    typedef void ( *TDC_GP22_CallbackOnMeasurement_3_t )( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDirection_t FireDirection, TDC_GP22_Measurement_t Measurement );

    typedef double TDC_GP22_PulseWidthRatio_t;

    typedef uint8_t TDC_GP22_ID_t[ 7 ];

    typedef struct TDC_GP22_Instance_Context TDC_GP22_Instance_Context_t;

    typedef struct TDC_GP22_Instance
    {
        TDC_GP22_t GP22x; // FIXME This attribute depends on a port type!!, drivers must be self-contained

        SPI_t SPIx;
        GPIO_t Reset;
        GPIO_t ChipSelect;
        GPIO_t Fire;
        GPIO_t Interrupt;
        GPIO_t Start;
        GPIO_t StartEnable;
        GPIO_t Stop_1_Enable;
        GPIO_t Stop_2_Enable;
        GPIO_t PowerEnable;

        TDC_GP22_CallbackOnComplete_t OnComplete;

        TDC_GP22_CallbackOnMeasurement_0_t OnMeasurement_0;
        TDC_GP22_CallbackOnMeasurement_1_t OnMeasurement_1;
        TDC_GP22_CallbackOnMeasurement_2_t OnMeasurement_2;
        TDC_GP22_CallbackOnMeasurement_3_t OnMeasurement_3;

        // Managed Internally
        TDC_GP22_Instance_Context_t * Context;
    } TDC_GP22_Instance_t;

    // #############################################################################
    // #### Public Method(s) #######################################################
    // #############################################################################

    TDC_GP22_Status_t TDC_GP22_Initialize( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_Cycle( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_DeInitialize( TDC_GP22_Instance_t * Instance );

    TDC_GP22_Status_t TDC_GP22_SetFireNumberOfPulses( TDC_GP22_Instance_t * Instance, TDC_GP22_FirePulses_t FirePulses );
    TDC_GP22_Status_t TDC_GP22_SetFireDivider( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDivider_t FireDivider );
    TDC_GP22_Status_t TDC_GP22_SetCalibrationNumberOfPeriods( TDC_GP22_Instance_t * Instance, TDC_GP22_CalibrationPeriod_t CalibrationPeriod );
    TDC_GP22_Status_t TDC_GP22_SetOscillatorDivider( TDC_GP22_Instance_t * Instance, TDC_GP22_OscillatorDivider_t OscillatorDivider );
    TDC_GP22_Status_t TDC_GP22_SetOscillatorInterval( TDC_GP22_Instance_t * Instance, TDC_GP22_OscillatorInterval_t OscillatorInterval );
    TDC_GP22_Status_t TDC_GP22_SetTemperaturePorts( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperaturePorts_t TemperaturePorts );
    TDC_GP22_Status_t TDC_GP22_SetTemperatureInterval( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureInterval_t TemperatureInterval );
    TDC_GP22_Status_t TDC_GP22_SetTemperatureDummyInterval( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureDummyInterval_t TemperatureDummyInterval );
    TDC_GP22_Status_t TDC_GP22_SetTemperatureClockSource( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureClockSource_t TemperatureClockSource );
    TDC_GP22_Status_t TDC_GP22_SetCalibration( TDC_GP22_Instance_t * Instance, TDC_GP22_Calibration_t Calibration );
    TDC_GP22_Status_t TDC_GP22_SetCalibrationAuto( TDC_GP22_Instance_t * Instance, TDC_GP22_AutoCalibration_t AutoCalibration );
    TDC_GP22_Status_t TDC_GP22_SetMode( TDC_GP22_Instance_t * Instance, TDC_GP22_Mode_t Mode );
    TDC_GP22_Status_t TDC_GP22_SetStop2SignalMode( TDC_GP22_Instance_t * Instance, TDC_GP22_Stop2SignalMode_t Stop2SignalMode );
    TDC_GP22_Status_t TDC_GP22_SetStop1SignalMode( TDC_GP22_Instance_t * Instance, TDC_GP22_Stop1SignalMode_t Stop1SignalMode );
    TDC_GP22_Status_t TDC_GP22_SetStartSignalMode( TDC_GP22_Instance_t * Instance, TDC_GP22_StartSignalMode_t StartSignalMode );

    TDC_GP22_Status_t TDC_GP22_SetHIT_2_Operator( TDC_GP22_Instance_t * Instance, TDC_GP22_HIT_2_Operator_t HIT_2_Operator );
    TDC_GP22_Status_t TDC_GP22_SetHIT_1_Operator( TDC_GP22_Instance_t * Instance, TDC_GP22_HIT_1_Operator_t HIT_1_Operator );
    TDC_GP22_Status_t TDC_GP22_SetFastInit( TDC_GP22_Instance_t * Instance, TDC_GP22_FastInit_t FastInit );
    TDC_GP22_Status_t TDC_GP22_SetChannel2Hits( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel2Hits_t Channel2Hits );
    TDC_GP22_Status_t TDC_GP22_SetChannel1Hits( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel1Hits_t Channel1Hits );
    TDC_GP22_Status_t TDC_GP22_SetCurrentMode( TDC_GP22_Instance_t * Instance, TDC_GP22_CurrentMode_t CurrentMode );
    TDC_GP22_Status_t TDC_GP22_SetStartFire( TDC_GP22_Instance_t * Instance, TDC_GP22_StartFire_t StartFire );
    TDC_GP22_Status_t TDC_GP22_SetStartEnable( TDC_GP22_Instance_t * Instance, TDC_GP22_StartEnable_t StartEnable );
    TDC_GP22_Status_t TDC_GP22_SetFireInput( TDC_GP22_Instance_t * Instance, TDC_GP22_FireInput_t FireInput );

    TDC_GP22_Status_t TDC_GP22_SetInterruptEnable( TDC_GP22_Instance_t * Instance, TDC_GP22_InterruptEnable_t InterruptEnable );
    TDC_GP22_Status_t TDC_GP22_SetChannel2SenseEdge( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel2SenseEdge_t Channel2SenseEdge );
    TDC_GP22_Status_t TDC_GP22_SetChannel1SenseEdge( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel1SenseEdge_t Channel1SenseEdge );
    TDC_GP22_Status_t TDC_GP22_SetChannel1DelayValue( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel1DelayValue_t Channel1DelayValue );

    TDC_GP22_Status_t TDC_GP22_SetAutomaticCalculation( TDC_GP22_Instance_t * Instance, TDC_GP22_AutomaticCalculation_t AutomaticCalculation );
    TDC_GP22_Status_t TDC_GP22_SetErrorValue( TDC_GP22_Instance_t * Instance, TDC_GP22_ErrorValue_t ErrorValue );
    TDC_GP22_Status_t TDC_GP22_SetFirstWave( TDC_GP22_Instance_t * Instance, TDC_GP22_FirstWave_t FirstWave );
    TDC_GP22_Status_t TDC_GP22_SetTimeoutDivider( TDC_GP22_Instance_t * Instance, TDC_GP22_TimeoutDivider_t TimeoutDivider );
    TDC_GP22_Status_t TDC_GP22_SetNumberOfPeriodsStop1( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPeriodsStop1_t NumberOfPeriodsStop1 );
    TDC_GP22_Status_t TDC_GP22_SetNumberOfPeriodsStop2( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPeriodsStop2_t NumberOfPeriodsStop2 );
    TDC_GP22_Status_t TDC_GP22_SetNumberOfPeriodsStop3( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPeriodsStop3_t NumberOfPeriodsStop3 );
    TDC_GP22_Status_t TDC_GP22_SetChannel2DelayValue( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel2DelayValue_t Channel2DelayValue );

    TDC_GP22_Status_t TDC_GP22_SetChannel3DelayValue( TDC_GP22_Instance_t * Instance, TDC_GP22_Channel3DelayValue_t Channel3DelayValue );
    TDC_GP22_Status_t TDC_GP22_SetPulseWidthMeasurement( TDC_GP22_Instance_t * Instance, TDC_GP22_PulseWidthMeasurement_t PulseWidthMeasurement );
    TDC_GP22_Status_t TDC_GP22_SetFirstWaveEdge( TDC_GP22_Instance_t * Instance, TDC_GP22_FirstWaveEdge_t FirstWaveEdge );
    TDC_GP22_Status_t TDC_GP22_SetAdditionalOffsetShiftPositive( TDC_GP22_Instance_t * Instance, TDC_GP22_AdditionalOffsetShiftPositive_t AdditionalOffsetShiftPositive );
    TDC_GP22_Status_t TDC_GP22_SetAdditionalOffsetShiftNegative( TDC_GP22_Instance_t * Instance, TDC_GP22_AdditionalOffsetShiftNegative_t AdditionalOffsetShiftNegative );
    TDC_GP22_Status_t TDC_GP22_SetOffsetShift( TDC_GP22_Instance_t * Instance, TDC_GP22_OffsetShift_t OffsetShift );

    TDC_GP22_Status_t TDC_GP22_SetFireDirection( TDC_GP22_Instance_t * Instance, TDC_GP22_FireDirection_t FireDirection );
    TDC_GP22_Status_t TDC_GP22_SetNoiseStart( TDC_GP22_Instance_t * Instance, TDC_GP22_NoiseStart_t NoiseStart );
    TDC_GP22_Status_t TDC_GP22_SetPhaseShift( TDC_GP22_Instance_t * Instance, TDC_GP22_PhaseShift_t PhaseShift );
    TDC_GP22_Status_t TDC_GP22_SetNumberOfPulseRepetition( TDC_GP22_Instance_t * Instance, TDC_GP22_NumberOfPulseRepetition_t NumberOfPulseRepetition );
    TDC_GP22_Status_t TDC_GP22_SetPhaseInversion( TDC_GP22_Instance_t * Instance, TDC_GP22_PhaseInversionPulse_t TDC_GP22_PhaseInversionPulse, TDC_GP22_PhaseInversion_t PhaseInversion );

    TDC_GP22_Status_t TDC_GP22_SetAnalogEnable( TDC_GP22_Instance_t * Instance, TDC_GP22_AnalogEnable_t AnalogEnable );
    TDC_GP22_Status_t TDC_GP22_SetTemperatureInput( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureInput_t TemperatureInput );
    TDC_GP22_Status_t TDC_GP22_SetComparatorOffset( TDC_GP22_Instance_t * Instance, TDC_GP22_ComparatorOffset_t ComparatorOffset );
    TDC_GP22_Status_t TDC_GP22_SetChargeTime( TDC_GP22_Instance_t * Instance, TDC_GP22_ChargeTime_t ChargeTime );
    TDC_GP22_Status_t TDC_GP22_SetTemperatureCycle( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureCycle_t TemperatureCycle );
    TDC_GP22_Status_t TDC_GP22_SetTimeOfFlightCycle( TDC_GP22_Instance_t * Instance, TDC_GP22_TimeOfFlightCycle_t TimeOfFlightCycle );
    TDC_GP22_Status_t TDC_GP22_SetCycle( TDC_GP22_Instance_t * Instance, TDC_GP22_Cycle_t Cycle );
    TDC_GP22_Status_t TDC_GP22_SetDefaultFireLevel( TDC_GP22_Instance_t * Instance, TDC_GP22_DefaultFireLevel_t DefaultFireLevel );
    TDC_GP22_Status_t TDC_GP22_SetResolution( TDC_GP22_Instance_t * Instance, TDC_GP22_Resolution_t Resolution );
    TDC_GP22_Status_t TDC_GP22_SetTemperatureMeasurementDirection( TDC_GP22_Instance_t * Instance, TDC_GP22_TemperatureMeasurementDirection_t TemperatureMeasurementDirection );

    TDC_GP22_Status_t TDC_GP22_GetMeasurement_0( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement );
    TDC_GP22_Status_t TDC_GP22_GetMeasurement_1( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement );
    TDC_GP22_Status_t TDC_GP22_GetMeasurement_2( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement );
    TDC_GP22_Status_t TDC_GP22_GetMeasurement_3( TDC_GP22_Instance_t * Instance, TDC_GP22_Measurement_t * Measurement );
    TDC_GP22_Status_t TDC_GP22_GetOperationalStatus( TDC_GP22_Instance_t * Instance, TDC_GP22_OperationalStatus_t * OperationalStatus );
    TDC_GP22_Status_t TDC_GP22_GetPulseWidthRatio( TDC_GP22_Instance_t * Instance, TDC_GP22_PulseWidthRatio_t * PulseWidthRatio );

    TDC_GP22_Status_t TDC_GP22_GetID( TDC_GP22_Instance_t * Instance, TDC_GP22_ID_t * ID );

    TDC_GP22_Status_t TDC_GP22_EEPROM_Save( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_EEPROM_Load( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_EEPROM_IsValid( TDC_GP22_Instance_t * Instance );

    TDC_GP22_Status_t TDC_GP22_Commit( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_Reset( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_Test( TDC_GP22_Instance_t * Instance );

    TDC_GP22_Status_t TDC_GP22_StartTimeOfFlight( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_StartTemperature( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_StartCalibrateResonator( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_StartCalibrateTDC( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_StartTimeOfFlightRestart( TDC_GP22_Instance_t * Instance );
    TDC_GP22_Status_t TDC_GP22_StartTemperatureRestart( TDC_GP22_Instance_t * Instance );

    TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_0( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_0 );
    TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_1( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_1 );
    TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_2( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_2 );
    TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_3( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_3 );
    TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_4( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_4 );
    TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_5( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_5 );
    TDC_GP22_Status_t TDC_GP22_GetConfigurationRegister_6( TDC_GP22_Instance_t * Instance, uint32_t * ConfigurationRegister_6 );

    // #############################################################################
    // #### Public Variable(s) #####################################################
    // #############################################################################

    /**
     *  @brief Version
     */
    extern const char TDC_GP22_VERSION[];

    // #############################################################################
    // #### File Guard #############################################################
    // #############################################################################

    #ifdef __cplusplus
} /* extern "C" */
    #endif /* __cplusplus */

#endif /* TDC_GP22_H_ */

/**
 *  @}
 *
 *  @}
 */

// #############################################################################
// #### END OF FILE ############################################################
// #############################################################################
