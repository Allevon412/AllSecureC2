#pragma once
#include "../crypt/Crypto.h"

typedef struct {
    PCHAR   Original;
    PCHAR   Buffer;
    UINT32  Size;
    UINT32  Length;

    UINT32  TaskID;
    BOOL    Endian;
} PARSER, * PPARSER;


VOID NewParser(PPARSER parser, PBYTE Buffer, UINT32 size);
VOID ParserDecrypt(PPARSER parser, PBYTE Key, PBYTE Iv);
INT16 ParserReadInt16(PPARSER parser);
BYTE ParserReadByte(PPARSER parser);
INT ParserReadInt32(PPARSER parser);
INT64 ParserReadInt64(PPARSER parser);
BOOL ParserReadBool(PPARSER parser);
PBYTE ParserReadBytes(PPARSER parser, PUINT32 size);
PCHAR ParserReadString(PPARSER parser, PUINT32 size);
PWCHAR ParserReadWString(PPARSER parser, PUINT32 size);
VOID ParserFree(PPARSER parser);
