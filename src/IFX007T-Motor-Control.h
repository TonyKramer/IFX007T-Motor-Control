
#ifndef IFX007T_MOTOR_CONTROL_H
#define IFX007T_MOTOR_CONTROL_H

#include <Arduino.h>

//======= Very important ========
//#define DEBUG_IFX007T         //Uncomment, if you wish debug output or tune the motor (Disables automatic V_neutralOffset)
//===============================

#ifdef DEBUG_IFX007T
    #define DEBUG_PRINT_LN(str)  Serial.println(str)
    #define DEBUG_PRINT(str)  Serial.print(str)
    #define TRIGGER_PIN  digitalWrite(12, _debugPin); _debugPin = !_debugPin;
#else
	#define DEBUG_PRINT_LN(str)
    #define DEBUG_PRINT(str)
    #define TRIGGER_PIN
#endif

// --------------- Define row-names for the _PinAssignment - matrix -----------------------------------------------
#define InputPin 0
#define InhibitPin 1
#define AdcPin 2
#define RefVoltage 3

#define PI_REG_K 0.01
#define PI_REG_I 0.001

typedef struct
    {
        uint8_t MotorPoles;
        bool SensingMode;
        float V_neutral[4];
        float V_neutralFunct[2];        //Slope, offset
        float Phasedelay[4];
        float PhasedelayFunct[2];       //Slope, offset
    }BLDCParameter;

typedef struct
    {
        uint8_t in_U;
        uint8_t in_V;
        uint8_t in_W;
        uint8_t inh_U;
        uint8_t inh_V;
        uint8_t inh_W;
        uint8_t BEMF_U;
        uint8_t BEMF_V;
        uint8_t BEMF_W;
        uint8_t adc_Vneutral;
        uint8_t adc_IS;
        uint8_t adc_ISRC;
    }BLDCPinSetting;

//================ Class Definition ===============================================================================
class IFX007TMotorControl
{
    public:

    

    //------------- User Functions --------------------------------------------------------------------------------

                IFX007TMotorControl(void);
                IFX007TMotorControl(BLDCPinSetting MotorPins);
                ~IFX007TMotorControl(void);
        void    begin(void);
        void    end(void);

        void    setUniDirMotorSpeed(uint8_t motor, uint8_t dutycycle);          //For Unidirectional motors; Parameters: motor can be 0, 1 or 2, dutycycle can be 0 - 255
        void    setBiDirMotorSpeed(bool direction, uint8_t dutycycle);          //For Bidirectional motors; Parametrs: direction can be 0 or 1, dutycycle can be 0 - 255
        void    configureBLDCMotor(BLDCParameter MyParameters);  
        void    setBLDCmotorRPMspeed(bool direction, uint16_t desired_rpmSpeed);
        void    setBLDCDutyCyclespeed(bool direction, uint8_t dutycycle);
        void    setHallBLDCmotorRPMspeed(bool direction, uint16_t desired_rpmSpeed);                         
        void    DebugRoutine(uint8_t Serialinput);
        
    //------------- Variables ----------------------------------------------------
        BLDCParameter MotorParam;
        BLDCPinSetting MotorPins;

        
        
        

    private:
        bool    StartupBLDC(bool dir);                  // Algorithm to start up the motor, as long as theres no BEMF
        void    changeBEMFspeed(bool direction, uint16_t dutycycle);
        void    DoBEMFCommutation(bool dir);
        bool    DetectZeroCrossing(uint8_t Pin, bool sign);
        void    UpdateHardware(uint8_t CommutationStep);       //For BLDC motor
        void    calculateLinearFunction(float *array, float *result);
        void    setPwmFrequency(uint8_t pin, uint16_t divisor);
        void    PI_Regulator_DoWork(uint16_t desired_rpmSpeed);
        void    CommutateHallBLDC(bool direction);
        uint8_t UpdateHall(void);
        
        void    setADCspeedFast(void);

        /*  _________________________
            | INU   | INV   | INW   |
            | INHU  | INHV  | INHW  |
            | ADC_U | ADC_V | ADC_W |
            | ADC_VS| IS    | V_IS_RC
            |_______________________|
        */
        uint8_t _PinAssignment[4][3];


        uint16_t _V_neutral;
        uint8_t _NumberofSteps;
        uint8_t _Commutation;
        uint16_t _lastBLDCspeed;
        uint8_t _CurrentDutyCycle;
        bool _debugPin;
        uint16_t _Stepcounter = 0;
        uint32_t timerstart;
        uint32_t _TimeperRotation;

        uint8_t _ClosedLoop = 0;
        uint8_t _OpenLoopSteps = 100;
        uint16_t _OpenLoopDelay = 3000;
        uint8_t _DutyCycle = 80; // it's related to the rotate speed (round per min)
        uint8_t _oldHall, _latestHall = 0;
        uint16_t _HallCounts = 0;
        unsigned long _PI_Update_Timeout = 999999999;
        uint16_t _LastRPM = 0; //the current rotate speed
        float _PI_Integral = 0.0; 

        // Values to start with, if debug option is turned on
        uint8_t iterations = 3;
        int16_t phasedelay = 90;
        uint8_t _V_NeutralOffset  = 100;

};

#endif /**< IFX007_*/