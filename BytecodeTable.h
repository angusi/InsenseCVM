#ifndef CVM_BYTECODETABLE_H
#define CVM_BYTECODETABLE_H

// Types
static const unsigned char BYTECODE_TYPE_UNKNOWN          = 0;
static const unsigned char BYTECODE_TYPE_INTEGER          = 1;
static const unsigned char BYTECODE_TYPE_UNSIGNED_INTEGER = 2;
static const unsigned char BYTECODE_TYPE_REAL             = 3;
static const unsigned char BYTECODE_TYPE_BOOL             = 4;
static const unsigned char BYTECODE_TYPE_BYTE             = 5;
static const unsigned char BYTECODE_TYPE_STRING           = 6;
static const unsigned char BYTECODE_TYPE_ARRAY            = 7;
static const unsigned char BYTECODE_TYPE_COMPONENT        = 8;
static const unsigned char BYTECODE_TYPE_INTERFACE        = 9;
static const unsigned char BYTECODE_TYPE_IN               = 10;
static const unsigned char BYTECODE_TYPE_OUT              = 11;

// Bytecodes
// Note that BYTECODE_POP and BYTECODE_TYPE are not defined in the Java VM or Bytecode compiler.
static const unsigned char BYTECODE_STOP           = 0; //STOP [COMPONENT_VARIABLE_NAME]
static const unsigned char BYTECODE_ENTERSCOPE     = 1;
static const unsigned char BYTECODE_EXITSCOPE      = 2;
static const unsigned char BYTECODE_PUSH           = 3; //PUSH [TYPE] [VALUE]
//static const unsigned char BYTECODE_POP          = ??;
static const unsigned char BYTECODE_DECLARE        = 4; //DECLARE [VARIABLE_NAME] [TYPE]
static const unsigned char BYTECODE_LOAD           = 5; //LOAD [VARIABLE_NAME]
static const unsigned char BYTECODE_STORE          = 6; //STORE [VARIABLE_NAME]
static const unsigned char BYTECODE_ADD            = 7;
static const unsigned char BYTECODE_SUB            = 8;
static const unsigned char BYTECODE_MUL            = 9;
static const unsigned char BYTECODE_DIV            = 10;
static const unsigned char BYTECODE_MOD            = 11;
static const unsigned char BYTECODE_LESS           = 12;
static const unsigned char BYTECODE_LESSEQUAL      = 13;
static const unsigned char BYTECODE_MORE           = 14;
static const unsigned char BYTECODE_MOREEQUAL      = 15;
static const unsigned char BYTECODE_EQAL           = 16;
static const unsigned char BYTECODE_UNEQUAL        = 17;
static const unsigned char BYTECODE_AND            = 18;
static const unsigned char BYTECODE_OR             = 19;
static const unsigned char BYTECODE_NOT            = 20;
static const unsigned char BYTECODE_BITAND         = 21;
static const unsigned char BYTECODE_BITXOR         = 22;
static const unsigned char BYTECODE_BITNOT         = 23;
//static const unsigned char BYTECODE_TYPE         = ?? //TYPE [TYPE_VARIABLE_NAME] [TYPE] {[NO_OF_CHANNEL] {[DIRECTION] [TYPE] [NAME_OF_CHANNEL] ...}}
static const unsigned char BYTECODE_COMPONENT      = 25; //COMPONENT [COMPONENT_NAME] [NO_OF_INTERFACE] {[NO_OF_CHANNEL] {[DIRECTION] [TYPE] [CHANNEL_NAME] ...}}
static const unsigned char BYTECODE_CALL           = 26; //CALL [COMPONENT_NAME] [NUMBER_OF_PARAMETERS]
static const unsigned char BYTECODE_CONSTRUCTOR    = 27; //CONSTRUCTOR [NUMBER_OF_PARAMETERS] {[TYPE] [PARAMETER_NAME] ...}
static const unsigned char BYTECODE_BEHAVIOUR_JUMP = 28; //BEHAVIOUR_JUMP [INTEGER_TYPE] [4-byte INTEGER]
static const unsigned char BYTECODE_JUMP           = 29; //JUMP [INTEGER_TYPE] [4-byte INTEGER]
static const unsigned char BYTECODE_IF             = 30; //IF [BYTE_JUMP]
static const unsigned char BYTECODE_ELSE           = 31; //ELSE [BYTE_JUMP]
static const unsigned char BYTECODE_CONNECT        = 32; //CONNECT [COMPONENT_VARIABLE_NAME] [CHANNEL_NAME] [COMPONENT_VARIABLE_NAME] [CHANNEL_NAME]
static const unsigned char BYTECODE_DISCONNECT     = 33; //DISCONNECT [COMPONENT_VARIABLE_NAME] [CHANNEL_NAME]
static const unsigned char BYTECODE_SEND           = 34; //SEND [CHANNEL_NAME]
static const unsigned char BYTECODE_RECEIVE        = 35; //RECEIVE [CHANNEL_NAME]

#endif //CVM_BYTECODETABLE_H
