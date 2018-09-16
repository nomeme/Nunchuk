#ifndef I2C_WIRE_H
#define I2C_WIRE_H

#include <Wire.h>
#include "I2C.h"

/**
 * Implementation of the I2C interface using Wire.h.
 */
class WireI2C : public I2C
{
    public: // Keep these inline so they can be replaced directly
        void
        setClock(const int32_t clock)
        {
            Wire.setClock(clock);
        }

        void
        start(const uint8_t addr)
        {
            Wire.beginTransmission(addr);
        }

        void
        stop()
        {
            Wire.endTransmission(true);
        }

        size_t
        write(const uint8_t data)
        {
            return Wire.write(data);
        }

        size_t
        write(const uint8_t *data, const size_t len)
        {
            return Wire.write(data, len);
        }

        uint8_t
        requestFrom(const uint8_t address, const uint8_t length)
        {
            return Wire.requestFrom(address, length);
        }

        uint8_t
        read()
        {
            return Wire.read();
        }

        int
        available()
        {
            return Wire.available();
        }
};

#endif
