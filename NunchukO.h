/**
 * @license NunchukO Arduino library v0.0.1 16/10/2016
 * This work is based on the excellent work of Robert Eisele.
 * http://www.xarg.org/2016/12/arduino-nunchuk-library/
 * https://github.com/infusion/Fritzing/tree/master/Nunchuk
 *
 * Copyright (c) 2018, David Jäckel (https://github.com/nomeme)
 * Dual licensed under the MIT or GPL Version 2 licenses.
 **/

/**
 * This library provides an object oriented way to access the Nunchuk from an Arduino.
 * It also provides an abstraction layer between protocol implementation and Nunchuk
 * object in order to exchange the underlying protocol implementation without
 * the need to modify the actual implementation of the Nunchuk class.
 *
 * There is currently just an implementation for Wire.h but the intention to expand this to TinyWire if needed.
 */

#ifndef NUNCHUKO_H
#define NUNCHUKO_H

#include <Wire.h>
#include "I2C.h"

/*
 * According to Robert Eisle cheap Nunchuk clones have problems with this.
 * Additionally it obviously causes additional load on the cpu.
 */
#define NUNCHUK_DISABLE_ENCRYPTION
/*
 * NUNCHUK_DEBUG causes the memory load to be quite large, so use with care.
 * This adds the print functions to the Nunchuk object and prints additional information.
 */
#define NUNCHUK_DEBUG

#define NUNCHUK_MESSAGE_SIZE 6
#define NUNCHUK_ADDRESS 0x52
/*
 * Define the TWI (ATMEL I2C) speed for the nunchuk.
 * It uses Fast-TWI at 400kHz.
 */
#define NUNCHUK_SPEED 400000

#define NUNCHUK_JOYSTICK_X_ZERO 128
#define NUNCHUK_JOYSTICK_Y_ZERO 128

#define NUNCHUK_ACCEL_X_ZERO 512
#define NUNCHUK_ACCEL_Y_ZERO 512
#define NUNCHUK_ACCEL_Z_ZERO 512

/**
 * Nunchuk implementation providing an object oriented implementation of the
 * Nunchuk Arduino library providing the possibility to switch the underlying
 * I2C implementation depending on the libraries supported by the chipset used.
 */
template<class T>
class Nunchuk {
    private:
        /**
         * A class extending I2C providing access to the Nunchuk.
         */
        T m_Wire;
        /**
         * Storage for the raw Nunchuk data.
         */
        uint8_t m_NunchukData[NUNCHUK_MESSAGE_SIZE];

    public:
        /**
         * Constructor.
         */
        Nunchuk() 
        {
            /**
             * Use this to print compiler errors.
             * This makes sure that T is of type I2C.
             */
            I2C *pointer = &m_Wire;
        }

        /**
         * Initialize the Nunchuck.
         */
        void
        init()
        {
            // Adapt the TWI speed for the Nunchuk.
            m_Wire.setClock(400000);
#ifdef NUNCHUK_DISABLE_ENCRYPTION
            m_Wire.start(NUNCHUK_ADDRESS);
            m_Wire.write(0xF0);
            m_Wire.write(0x55);
            m_Wire.stop();
            m_Wire.start(NUNCHUK_ADDRESS);
            m_Wire.write(0xFB);
            m_Wire.write(0x00);
            m_Wire.stop();
#else
            m_Wire.start(NUNCHUK_ADDRESS);
            m_Wire.write(0x40);
            m_Wire.write(0x00);
            m_Wire.stop();
#endif
#ifdef NUNCHUK_DEBUG
            // This prints out the Nunchuk type.
            m_Wire.start(NUNCHUK_ADDRESS);
            m_Wire.write(0xFA);
            m_Wire.stop();
            m_Wire.requestFrom(NUNCHUK_ADDRESS, NUNCHUK_MESSAGE_SIZE);
            for(uint8_t i = 0; i<6;i++)
            {
                if(m_Wire.available()) {
                    Serial.print(m_Wire.read(), HEX);
                    Serial.print(" ");
                }
            }
            m_Wire.stop();
            Serial.println();
            delay(100);
#endif
        }

        /**
         * Decrypts a byte if the encryption is used.
         *
         * @param   x the byte to be decrypted
         * @return  the decrypted byte
         */
        // This methot should be kept as inline function.
        // TODO: make private
        uint8_t
        decodeByte(uint8_t byte)
        {
#ifdef NUNCHUK_DISABLE_ENCRYPTION
            return byte;
#else
            return (byte ^ 0x17) + 0x17;
#endif
        }

        /**
         * Read new data from the Nunchuk.
         *
         * @return  Returns true if the data could be read
         */
        bool
        read()
        {
            uint8_t i;
            m_Wire.requestFrom(NUNCHUK_ADDRESS, NUNCHUK_MESSAGE_SIZE);
            for(i = 0; i < NUNCHUK_MESSAGE_SIZE && m_Wire.available(); i++)
            {
                m_NunchukData[i] = decodeByte(m_Wire.read());
            }
            m_Wire.start(NUNCHUK_ADDRESS);
            m_Wire.write(0x00);
            m_Wire.stop();
            return i == 6;
        }

        bool
        getButtonZ()
        {
            // We need to negate the data, as the transmitted values are inverted
            // TODO: does it work with != 1?
            return (~m_NunchukData[5] >> 0) & 1 == 1;
        }

        bool
        getButtonC()
        {
            // We need to negate the data, as the transmitted values are inverted
            // TODO: does it work with != 1?
            return (~m_NunchukData[5] >> 1) & 1 == 1;
        }

        /**
         * Returns the position of the joystick on the X axis.
         *
         * @return  position
         */
        int8_t
        getJoystickY();

        /*
         * Returns the position of the joystick on the Y axis.
         *
         * @return  position
         */
        int8_t
        getJoystickX();

        /**
         * Returns the angle of the joystick.
         *
         * @return  angle
         */
        float
        getJoystickAngle();

        /**
         * Returns the acceleration on the X axis.
         *
         * @return  the acceleration
         */
        int16_t
        getAccelX();

        /**
         * Returns the acceleration on the Y axis.
         *
         * @return  the acceleration
         */
        int16_t
        getAccelY();

        /**
         * Returns the acceleration on the Z axis.
         *
         * @return  the acceleration
         */
        int16_t
        getAccelZ();

        /**
         * Returns the pitch of the nunchuk.
         *
         * @return  the pitch
         */
        float
        getPitch();

        /**
         * Returns the roll of the nunchuk.
         *
         * @return  the roll
         */
        float
        getRoll();

#ifdef NUNCHUK_DEBUG
        /**
         * Print the raw data sent by the Nunchuk.
         */
        void
        printRaw()
        {
            Serial.print("Joystick: x: ");
            Serial.print(getJoystickRawX());
            Serial.print(", y: ");
            Serial.print(getJoystickRawY());
            Serial.print(", Acceleration: x: ");
            Serial.print(getAccelRawX());
            Serial.print(", y: ");
            Serial.print(getAccelRawY());
            Serial.print(", z: ");
            Serial.print(getAccelRawZ());
            Serial.print(", Button: c: ");
            Serial.print(getButtonC());
            Serial.print(", z: ");
            Serial.print(getButtonZ());
            Serial.println();
        }
        /**
         * Print the Nunchuk data.
         */
        void
        print()
        {
            Serial.print("Joystick: x: ");
            Serial.print(getJoystickX());
            Serial.print(", y: ");
            Serial.print(getJoystickY());
            Serial.print(", Acceleration: x: ");
            Serial.print(getAccelX());
            Serial.print(", y: ");
            Serial.print(getAccelY());
            Serial.print(", z: ");
            Serial.print(getAccelZ());
            Serial.print(", pitch: ");
            Serial.print(getPitch());
            Serial.print(", roll: ");
            Serial.print(getRoll());
            Serial.print(", Button: c: ");
            Serial.print(getButtonC());
            Serial.print(", z: ");
            Serial.print(getButtonZ());
            Serial.println();
        }
#endif

    private:    //Private inline functions
        /**
         * Returns the raw X position of the Nunchuk joystick in the range of [0..255].
         *
         * @return  the raw data
         */
        uint8_t
        getJoystickRawX()
        {
            return m_NunchukData[0];
        }

        /**
         * Returns the raw Y position of the Nunchuk joystick in the range of [0..255].
         *
         * @return  the raw data
         */
        uint8_t
        getJoystickRawY()
        {
            return m_NunchukData[1];
        }

        /**
         * Returns the raw acceleration data for the X axis in the range of [0..1024].
         *
         * @return  the raw data
         */
        uint16_t
        getAccelRawX()
        {
            return ((uint16_t) m_NunchukData[2] << 2) | ((m_NunchukData[5] >> 2) & 3);
        }

        /**
         * Returns the raw acceleration data for the Y axis in the range of [0..1024].
         *
         * @return  the raw data
         */
        uint16_t
        getAccelRawY()
        {
            return ((uint16_t) m_NunchukData[3] << 2) | ((m_NunchukData[5] >> 4) & 3);
        }

        /**
         * Returns the raw acceleration data for the Z axis in the range of [0..1024].
         *
         * @return  the raw data
         */
        uint16_t
        getAccelRawZ()
        {
            return ((uint16_t) m_NunchukData[4] << 2) | ((m_NunchukData[5] >> 6) & 3);
        }
};

template<class T>
int8_t
Nunchuk<T>::getJoystickY()
{
    return (int8_t)((int16_t) getJoystickRawY() - NUNCHUK_JOYSTICK_Y_ZERO);
}

template<class T>
int8_t
Nunchuk<T>::getJoystickX()
{
    return (int8_t)((int16_t) getJoystickRawX() - NUNCHUK_JOYSTICK_X_ZERO);
}

template<class T>
float
Nunchuk<T>::getJoystickAngle()
{
    return atan2((float) getJoystickY(), (float) getJoystickX());
}

template<class T>
int16_t
Nunchuk<T>::getAccelX()
{
    return (int16_t) getAccelRawX() - (int16_t) NUNCHUK_ACCEL_X_ZERO;
}

template<class T>
int16_t
Nunchuk<T>::getAccelY()
{
    return (int16_t) getAccelRawY() - (int16_t) NUNCHUK_ACCEL_Y_ZERO;
}

template<class T>
int16_t
Nunchuk<T>::getAccelZ()
{
    return (int16_t) getAccelRawZ() - (int16_t) NUNCHUK_ACCEL_Z_ZERO;
}

template<class T>
float
Nunchuk<T>::getPitch(){
    return atan2((float) getAccelY(), (float) getAccelZ());
}

template<class T>
float
Nunchuk<T>::getRoll()
{
    return atan2((float) getAccelX(), (float) getAccelZ());
}

#endif
