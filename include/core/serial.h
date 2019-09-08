#ifndef SERIAL_H
#define SERIAL_H

#include <core/def.h>
#include <core/std.h>

#define COM1 0x3f8

typedef enum
{
    RECIEVED = 1,
    EMPTY = 32
} SERIAL_STATE;

void serial(u32 port)
{
    outb(port + 1, 0);    // Disable interrupts
    outb(port + 3, 0x80); // Baud Rate Divisor Setting enabled
    outb(port + 0, 3);    // Divisor / Low Byte
    outb(port + 1, 0);    // High Byte
    outb(port + 3, 3);    // Set control options
    outb(port + 2, 0xC7); // Enable  FIFO(FirstInFirstOut)
    outb(port + 4, 0x0B); // Enable interrupts
}

u32 serial_check(u32 port, SERIAL_STATE state)
{
    return inb(port + 5) & state;
}

s8 serial_read(u32 port)
{
    while (serial_check(port, RECIEVED) == 0)
        ;
    return inb(port);
}

void serial_write(u32 port, s8 value)
{
    while (serial_check(port, EMPTY) == 0)
        ;
    outb(port, value);
}

void serial_write_string(u32 port, s8 *string)
{
    u32 l = strlen(string);
    for (u32 i = 0; i < l; i++)
    {
        serial_write(port, string[i]);
    }
}


#endif