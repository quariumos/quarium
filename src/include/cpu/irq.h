#ifndef IRQ_H
#define IRQ_H

#ifdef SILICON_SERIAL_LOG
#include <io/kprintf.h>
#include <io/duplex/serial.h>
#endif

#include <types.h>
#include <cpu/port.h>

#define PIC1 0x20 // Base address of master PIC
#define PIC2 0xA0 // Base address of slave PIC
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

void eoi(u16 n)
{
    if (n > 7)
        outb(PIC2, 0x20);
    outb(PIC1, 0x20); //EOI
}
void pic_mask(unsigned char n)
{
    u16 port;
    u8 value;

    if (n < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        n -= 8;
    }
    value = inb(port) | (1 << n);
    outb(port, value);
}

void pic_unmask(unsigned char n)
{
    u16 port;
    u8 value;

    if (n < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        n -= 8;
    }
    value = inb(port) | ~(1 << n);
    outb(port, value);
}

extern void load_idt();
typedef struct
{
    unsigned short int offset_lowerbits;
    unsigned short int selector;
    u8 zero;
    u8 type_attr;
    unsigned short int offset_higherbits;
} __attribute__((packed)) idt_entry_t;
#define IDT_SIZE 256
extern idt_entry_t idt[IDT_SIZE];
void remap_pic(u16 start_offset, u16 end_offset)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, start_offset);
    outb(0xA1, end_offset);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

typedef void (*irq_handler_t)();

irq_handler_t irq_handler_list[IDT_SIZE] = {
    noop,
};

void set_irq_handler(int vector, irq_handler_t handler)
{
    irq_handler_list[vector] = handler;
}

void global_irq_manager(int n)
{
#ifdef SILICON_SERIAL_LOG
    kprintf(serial.out_device, "Log: IRQ %d\n", n);
#endif
    irq_handler_t f = irq_handler_list[n];
    if (f != noop)
        irq_handler_list[n]();
    eoi(n - 32);
}
#endif