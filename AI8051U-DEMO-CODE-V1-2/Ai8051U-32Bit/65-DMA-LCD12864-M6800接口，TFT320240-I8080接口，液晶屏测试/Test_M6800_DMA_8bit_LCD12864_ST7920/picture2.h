/********************
点阵：128x64
取模方式：水平扫描，从左到右，从上到下，低位在前。
#define IMG_WIDTH       128
#define IMG_HEIGHT      64
#define IMG_DEPTH       1
*********************/

    //128*64点阵图形数据
u8 const gImage_picture2[1024] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x3F,0x03,0xF0,0x3F,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x7F,0xF0,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x01,0xFF,0xF0,0x41,0x00,0x7F,0xC0,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x01,0x08,0x10,0x41,0x00,0x40,0x40,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x01,0x08,0x10,0x41,0x00,0x40,0x40,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF9,0x1F,0x90,0x41,0x00,0x7F,0xC0,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF9,0x31,0x10,0x7F,0xF0,0x40,0x41,0xFF,0xFC,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x4A,0x10,0x40,0x00,0x40,0x40,0x02,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x04,0x10,0x40,0x00,0x7F,0xC0,0x02,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x1B,0x10,0x40,0x00,0x11,0x00,0x22,0x20,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0xE0,0xF0,0x7F,0x81,0x11,0x10,0x22,0x10,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x0C,0x10,0x40,0x80,0x91,0x10,0x42,0x08,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x02,0x10,0x40,0x80,0x51,0x20,0x82,0x04,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x18,0x10,0x40,0x80,0x51,0x41,0x02,0x04,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x06,0x10,0x80,0x80,0x11,0x00,0x0A,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0xFF,0xF0,0x80,0x83,0xFF,0xF8,0x04,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x00,0x11,0x00,0x80,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x63,0x07,0x87,0x83,0xCF,0xC1,0x8C,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x67,0x83,0x0C,0xC6,0x6C,0x03,0x8C,0xD8,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,0x00,
    0x6C,0xC3,0x0C,0xC6,0xEC,0x0F,0x8C,0xD8,0x00,0x3C,0x00,0x00,0x00,0xFF,0x80,0x00,
    0x6C,0xC3,0x0E,0xC6,0xEC,0x01,0x8C,0xD8,0x00,0xFF,0x80,0x00,0x03,0xFF,0xE0,0x00,
    0x6C,0xC3,0x07,0x86,0x6F,0x81,0x8C,0xD8,0x03,0xFF,0xE0,0x00,0x07,0xFF,0xF0,0x00,
    0x6F,0xC3,0x0D,0xC7,0x60,0xC1,0x8C,0xD8,0x07,0x80,0xF0,0x00,0x0F,0xFF,0xF8,0x00,
    0x6C,0xC3,0x0C,0xC7,0x60,0xC1,0x8C,0xD8,0x0E,0x18,0x38,0x00,0x1F,0xFF,0xFC,0x00,
    0x6C,0xC3,0x0C,0xC6,0x61,0x81,0x8C,0xD8,0x1C,0x18,0x1C,0x00,0x1F,0xFF,0xFC,0x00,
    0x6C,0xC7,0x87,0x83,0xCF,0x01,0x87,0x98,0x38,0x18,0x0E,0x00,0x3F,0xFF,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x30,0x18,0x06,0x00,0x3F,0xFF,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x70,0x18,0x07,0x00,0x7C,0x3E,0x1F,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x60,0x18,0x03,0x00,0x7C,0x3E,0x1F,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x60,0x18,0x03,0x00,0x7E,0x7F,0x3F,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF0,0x18,0x07,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF8,0x1F,0xCF,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF8,0x1F,0xCF,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF0,0x00,0x07,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x60,0x00,0x03,0x00,0x7F,0x80,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x70,0x00,0x07,0x00,0x3F,0x80,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x30,0x00,0x06,0x00,0x3F,0xC1,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x38,0x00,0x0E,0x00,0x1F,0xE3,0xFC,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x1C,0x00,0x1C,0x00,0x0F,0xFF,0xF8,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x0E,0x18,0x38,0x00,0x07,0xFF,0xF0,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x07,0xBC,0xF0,0x00,0x03,0xFF,0xE0,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x03,0xFF,0xE0,0x00,0x01,0xFF,0xC0,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0xFF,0x80,0x00,0x00,0x3E,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x3F,0x03,0xF0,0x3F,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

