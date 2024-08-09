#include "../../headers/helpers/parser.h"
#include "../../headers/cstdreplacement/localcstd.h"

VOID NewParser(PPARSER parser, PBYTE Buffer, UINT32 size) {
	if (parser == NULL) {
		return;
	}

	parser->Original = agent->apis->pLocalAlloc(LPTR, size);
	MemoryCopy(parser->Original, Buffer, size);

	parser->Buffer = parser->Original;
	parser->Length = size;
	parser->Size = size;
}

VOID ParserDecrypt(PPARSER parser, PBYTE Key, PBYTE Iv) {
	if (parser == NULL) {
		return;
	}
	AESCTR(parser->Buffer, parser->Length, Key, AES_256_KEY_SIZE, Iv);

}

INT16 ParserReadInt16(PPARSER parser) {
	INT16 value = 0;
	if (parser == NULL) {
		return value;
	}

	if (parser->Length < sizeof(INT16)) {
		return value;
	}

	MemoryCopy(&value, parser->Buffer, sizeof(INT16));
	parser->Buffer += sizeof(INT16);
	parser->Length -= sizeof(INT16);

	return value;
}

BYTE ParserReadByte(PPARSER parser) {
	BYTE value = 0;
	if (parser == NULL) {
		return value;
	}

	if (parser->Length < sizeof(BYTE)) {
		return value;
	}

	value = *parser->Buffer;
	parser->Buffer += sizeof(BYTE);
	parser->Length -= sizeof(BYTE);

	return value;
}

INT ParserReadInt32(PPARSER parser) {
	INT value = 0;
	if (parser == NULL) {
		return value;
	}

	if (parser->Length < sizeof(INT)) {
		return value;
	}

	MemoryCopy(&value, parser->Buffer, sizeof(INT));
	parser->Buffer += sizeof(INT);
	parser->Length -= sizeof(INT);

	return value;
}

INT64 ParserReadInt64(PPARSER parser) {
	INT64 value = 0;
	if (parser == NULL) {
		return value;
	}

	if (parser->Length < sizeof(INT64)) {
		return value;
	}

	MemoryCopy(&value, parser->Buffer, sizeof(INT64));
	parser->Buffer += sizeof(INT64);
	parser->Length -= sizeof(INT64);

	return value;
}

BOOL ParserReadBool(PPARSER parser) {
	BOOL value = FALSE;
	if (parser == NULL) {
		return value;
	}

	if (parser->Length < sizeof(BOOL)) {
		return value;
	}

	MemoryCopy(&value, parser->Buffer, sizeof(BOOL));
	parser->Buffer += sizeof(BOOL);
	parser->Length -= sizeof(BOOL);

	return value;
}

PBYTE ParserReadBytes(PPARSER parser, PUINT32 size) {

	UINT32 Length = 0;
	PBYTE value = NULL;

	if (parser == NULL) {
		return value;
	}

	if (parser->Length < 4) {
		return value;
	}

	MemoryCopy(&Length, parser->Buffer, 4);
	parser->Buffer += 4;

	if(parser->Endian)
		Length = bswap32(Length);

	value = (PBYTE)parser->Buffer;
	if (value == NULL)
		return NULL;

	parser->Length -= 4;
	parser->Length -= Length;
	parser->Buffer += Length;


	if (size != NULL) {
		*size = Length;
	}

	return value;
}

PBYTE ParserReadNBytes(PPARSER parser, UINT32 size) {

	PBYTE value = NULL;

	if (parser == NULL)
		return value;

	if(parser->Length < size)
		return value;

	MemoryCopy(&value, parser->Buffer, size);
	parser->Buffer += size;

	if(parser->Endian)
		value = bswap32(value);

	if(value == NULL)
		return NULL;

	parser->Length -= size;

	return value;
}

PCHAR ParserReadString(PPARSER parser, PUINT32 size) {
	return (PCHAR)ParserReadBytes(parser, size);
}

PWCHAR ParserReadWString(PPARSER parser, PUINT32 size) {
	return (PWCHAR)ParserReadBytes(parser, size);
}

VOID ParserFree(PPARSER parser) {
	
	if (parser->Original) {
		MemorySet(parser->Original, 0, parser->Size);
		agent->apis->pLocalFree(parser->Original);
		parser->Original = NULL;
		parser->Buffer = NULL;
	}
}

