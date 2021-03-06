#include "aes.h"
#include <stdlib.h>
#include "AS3/AS3.h"

void decryptAES() __attribute__((used,
	annotate("as3sig:public function decryptAES(data:ByteArray, key:ByteArray, iv:ByteArray):void"),
    annotate("as3import:flash.utils.ByteArray"),
	annotate("as3package:com.kaltura.crypto.DecryptUtil")));

void decryptAES()
{
    // Copy our data, key, and iv ByteArrays over to C memory

    inline_as3(
        "var dataLength:int = data.length;"
        "data.position = 0;"
        "var dataPtr:int = CModule.malloc(dataLength);"
        "CModule.writeBytes(dataPtr, dataLength, data);"
        "key.position = 0;"
        "var keyPtr:int = CModule.malloc(key.length);"
        "CModule.writeBytes(keyPtr, key.length, key);"
        "iv.position = 0;"
        "var ivPtr:int = CModule.malloc(iv.length);"
        "CModule.writeBytes(ivPtr, iv.length, iv);"
        ::
    );

    unsigned int cDataLength;

    unsigned char *cData = 0;
    unsigned char *cKey = 0;
    unsigned char *cIV = 0;

    AS3_GetScalarFromVar(cDataLength, dataLength);
    AS3_GetScalarFromVar(cData, dataPtr);
    AS3_GetScalarFromVar(cKey, keyPtr);
    AS3_GetScalarFromVar(cIV, ivPtr);

    // initialize context and decrypt cipher

    AesCtx ctx;

    if( AesCtxIni(&ctx, cIV, cKey, KEY128, CBC) < 0)
        inline_as3("trace('init error');");

    if (AesDecrypt(&ctx, cData, cData, cDataLength) < 0)
        inline_as3("trace('error in decryption, data size is ' + data.length);");

    // Write our modified memory back to the data ByteArray
    inline_as3(
        "data.position = 0;"
        "CModule.readBytes(%0, %1, data);"
        :: "r"(cData), "r"(cDataLength)
    );

    free(cData);
    free(cKey);
    free(cIV);
}

void unpad() __attribute__((used,
    annotate("as3sig:public function unpad(data:ByteArray):Boolean"),
    annotate("as3import:flash.utils.ByteArray"),
    annotate("as3package:com.kaltura.crypto.DecryptUtil")));


void unpad()
{
    inline_as3(
        "var dataLength:int = data.length;"
        "data.position = 0;"
        "var dataPtr:int = CModule.malloc(dataLength);"
        "CModule.writeBytes(dataPtr, dataLength, data);"
        ::
    );

    unsigned int cDataLength;
    unsigned char *cData = 0;

    AS3_GetScalarFromVar(cDataLength, dataLength);
    AS3_GetScalarFromVar(cData, dataPtr);

    unsigned int i;
    unsigned char c, v;

    int unpadSuccess = 1;

    c = cDataLength % 16;
    
    if ( c == 0 )
    {
        c = cData[cDataLength-1];

        for ( i = c; i > 0; i-- )
        {
            v = cData[cDataLength-1];
            cDataLength--;
            if ( c != v )
            {
                //inline_as3("trace(\"Invalid padding value. expected [%0], found [%1]\")" :: "r"(c), "r"(v));
                unpadSuccess = 0;
                break;
            }
        }
    }
    else
    {
        inline_as3("trace(\"Data length needs to be a multiple of 16\");");
        unpadSuccess = 0;
    }

    free(cData);

    if ( unpadSuccess == 1 )
    {
        inline_as3("data.length = %0;" :: "r"(cDataLength));
        inline_as3("return true;");
    }
    else
    {
        inline_as3("return false;");
    }
}
