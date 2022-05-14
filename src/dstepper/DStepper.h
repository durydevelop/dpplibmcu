/* mbed Stepper Motor Library
 * Copyright (c) 2012-2022 Fabio Durigon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#ifndef DSTEPPER_H
#define DSTEPPER_H

#include <dsoftpwm>

/**
 * @brief Library for handle Stepper Motor with a Power Module with Clock and Dir input
 * 
 */

class DStepper
{
    public:
        // Pin levels
        enum DPinLevel  { LOW_LEVEL, HIGH_LEVEL };
        // Pin modes
        enum DPinMode   { INPUT_MODE, OUTPUT_MODE, INPUT_PULLUP_MODE };
        // Rotation direction
        enum DRotationDir { ROT_CC, ROT_CW };

        DStepper(uint8_t PinDir, uint8_t PinClk, uint16_t StepsPerRev = 200);
        ~DStepper();
        void begin();
        void begin(uint16_t FreqHz, bool Enabled);
        void begin(uint8_t Rpm, bool Enabled);

        // CONFIGURATION API
        void SetSwappedDir(bool Enabled);
        void SetStepsPerRevolution(uint16_t Steps);
        void SetClkDutyCycle(uint8_t DutyCyclePerc);

        // GETTERS API
        uint16_t GetClkFreq(void);
        uint8_t GetClkDutyCycle(void);
        DStepper::DRotationDir GetDir(void);
        uint16_t GetRpm(void);
        
        // DIRECT MOTOR CONTROL API
        void On(void);
        void Off(void);

        void SetFreq(uint16_t FreqHz);
        void SetFreq(uint16_t FreqHz, uint8_t DutyCyclePerc);
        void SetFreq(uint16_t FreqHz, DRotationDir RotationDir);
        //void RampFreq(uint16_t Rpm, uint16_t RampMillis);

        void SetRpm(uint16_t Rpm);
        void SetRpm(uint16_t Rpm, DRotationDir RotationDir);
        bool RampRpm(uint16_t RampMillis, uint16_t Rpm, DRotationDir RotationDir, int8_t StopPin, DPinLevel StopLevel);

        void SetDir(DRotationDir RotationDir);
        void SetDirCW(void);
        void SetDirCC(void);
        void RevertDir(void);

        // HARDWARE CONTROL API
        bool InitPin(uint8_t Pin, uint8_t Mode);
        uint8_t ReadPin(uint8_t Pin);
        void WritePin(uint8_t Pin, uint8_t Level);
            
    private:
        const uint8_t DirPin;     
        const uint8_t ClkPin;
        DPwmOut *dPwmOut;
        uint16_t StepsPerRevol;
        uint8_t SwappedDir;
        uint16_t CurrFreqHz;
        uint8_t CurrDutyCycle;
};
#endif