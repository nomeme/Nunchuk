#ifndef I2C_H
#define I2C_H

/**
 * Abstraction interface for I2C communication.
 */
class I2C {
    public:
        /**
         * Set the clock speed of the I2C interface in Hertz.
         * Set it to:
         *  100000  for standard mode
         *  400000  for fast mode
         * Depending on processor following modes might be available:
         *  10000   for low speed mode
         *  1000000 for fast mode plus
         *  3400000 for high speed mode
         *
         *  @param  the clock speed to be set
         */
        virtual void
        setClock(const int32_t clock) = 0;

        /**
         * Initiates a transmission to an I2C device with given address.
         * The address of an I2C device consists of 8 bits, where the first seven
         * bits define the address and the eight bit defines wheter it is written
         * or read from.
         * [a,a,a,a,a,a,a,r].
         * If we have an 8 bit address we therefore need to shift >> 1.
         *
         * @param   the address of the I2C device
         */
        virtual void
        start(const uint8_t addr) = 0;

        /**
         * Stops the transmission to the I2C device.
         */
        virtual void
        stop() = 0;

        /**
         * Adds the given data to the transmission queue.
         *
         * @param   the data
         * @return  the number of bytes written.
         */
        virtual size_t
        write(const uint8_t data) = 0;

        /**
         * Adds the the data of given length to the transmission queue.
         *
         * @param   the pointer to the data
         * @param   the length of the data
         * @return  the number of bytes written.
         */
        virtual size_t
        write(const uint8_t *data, const size_t len) = 0;

        /**
         * Used by the master device to request bytes from the slave
         * device.
         *
         * @param address   the address of the I2C device
         * @param length    the number of bytes to request
         * @return          the number of bytes returned
         */
        virtual uint8_t
        requestFrom(const uint8_t address, const uint8_t length) = 0;

        /**
         * Reads one byte that was transmitted from a slave device after
         * a call to #requestFrom, from the buffer.
         *
         * @return  the next byte from the buffer
         */
        virtual uint8_t
        read() = 0;

        /**
         * Returns the number of bytes available to read.
         *
         * @return  number of available bytes.
         */
        virtual int
        available() = 0;
};

#endif
