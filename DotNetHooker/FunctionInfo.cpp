#include "pch.h"
#include "FunctionInfo.h"

static bool IsCallingConv(ULONG CallingConvByte, CorCallingConvention Convention)
{
    ULONG maskedByte = CallingConvByte & IMAGE_CEE_CS_CALLCONV_MASK;

    return maskedByte == (ULONG)Convention;
}


// http://www.dotnetexperts.com/ecma/
// CLI partition II - 23.2.1 MethodDefSig
HRESULT FunctionInfo::ParseFunctionSignature(
    _In_ PCCOR_SIGNATURE Signature,
    _In_ ULONG SignatureSize,
    _In_ ICorProfilerInfo2* ProfilerInfo,
    _In_ IMetaDataImport* MetaData
)
{
    if (SignatureSize == 0 || !Signature)
    {
        return E_INVALIDARG;
    }

    PCCOR_SIGNATURE currentSig = Signature;
    PCCOR_SIGNATURE signatureEnd = currentSig + SignatureSize;
    ULONG callingConv = CorSigUncompressCallingConv(currentSig);
    if (!IsCallingConv(callingConv, IMAGE_CEE_CS_CALLCONV_DEFAULT))
    {
        // no time for the rest of the cases.
        return E_NOTIMPL;
    }

    if ((callingConv & IMAGE_CEE_CS_CALLCONV_GENERIC) || (callingConv & IMAGE_CEE_CS_CALLCONV_EXPLICITTHIS))
    {
        // no time for generics or corner cases.
        return E_NOTIMPL;
    }

    if (callingConv & IMAGE_CEE_CS_CALLCONV_HASTHIS)
    {
        hasThis = true;
    }

    if (currentSig >= signatureEnd)
    {
        return E_INVALIDARG;
    }

    ULONG argumentCount = CorSigUncompressData(currentSig);
    ArgumentType returnType = ArgumentType::Unknown;
    HRESULT hres = ParseType(currentSig, (signatureEnd - currentSig), MetaData, returnType);
    if (FAILED(hres))
    {
        return hres;
    }

    if (hasThis)
    {
        // instance methods receive as first param "this", which we do not want to dump in most cases.
        // so we add an argument of type "Class" that will result in a dummy ptr parser, that skips it.
        AddArgumentType(ArgumentType::Class, ProfilerInfo);
    }

    for (ULONG argIndex = 0; argIndex < argumentCount; argIndex++)
    {
        ArgumentType currentArg = ArgumentType::Unknown;
        hres = ParseType(currentSig, (signatureEnd - currentSig), MetaData, currentArg);
        if (FAILED(hres))
        {
            return hres;
        }

        AddArgumentType(currentArg, ProfilerInfo);
        // we could use IMetaDataImport::GetParamForMethodIndex and IMetaDataImport::GetMethodProps here to get the name of the parameter
    }

    return S_OK;
}

// return E_NOTIMPL for complex types that should be encountered rarely
// returns S_OK for implemented types: primitive, class, and simple array.
HRESULT FunctionInfo::ParseType(
    _In_ PCCOR_SIGNATURE& Signature,
    _In_ SIZE_T SignatureSize,
    _In_ IMetaDataImport* MetaData,
    _Out_ ArgumentType& ArgType
)
{
    if (SignatureSize == 0)
    {
        return E_INVALIDARG;
    }

    // mark all types as Unknown for which there is no implemented argument extractor.
    ArgType = ArgumentType::Unknown;
    PCCOR_SIGNATURE endSignature = Signature + SignatureSize;
    CorElementType elementType = CorSigUncompressElementType(Signature);
    switch (elementType)
    {
    case ELEMENT_TYPE_END:
        return S_OK;
    case ELEMENT_TYPE_VOID:
        return S_OK;
    case ELEMENT_TYPE_BOOLEAN:
        ArgType = ArgumentType::DummyByte;
        return S_OK;
    case ELEMENT_TYPE_CHAR:
        ArgType = ArgumentType::DummyWord;
        return S_OK;
    case ELEMENT_TYPE_I1:
        ArgType = ArgumentType::DummyByte;
        return S_OK;
    case ELEMENT_TYPE_U1:
        ArgType = ArgumentType::DummyByte;
        return S_OK;
    case ELEMENT_TYPE_I2:
        ArgType = ArgumentType::DummyWord;
        return S_OK;
    case ELEMENT_TYPE_U2:
        ArgType = ArgumentType::DummyWord;
        return S_OK;
    case ELEMENT_TYPE_I4:
        ArgType = ArgumentType::DummyDword;
        return S_OK;
    case ELEMENT_TYPE_U4:
        ArgType = ArgumentType::DummyDword;
        return S_OK;
    case ELEMENT_TYPE_I8:
        ArgType = ArgumentType::DummyQword;
        return S_OK;
    case ELEMENT_TYPE_U8:
        ArgType = ArgumentType::DummyQword;
        return S_OK;
    case ELEMENT_TYPE_R4:
        ArgType = ArgumentType::DummyDword;
        return S_OK;
    case ELEMENT_TYPE_R8:
        ArgType = ArgumentType::DummyQword;
        return S_OK;
    case ELEMENT_TYPE_STRING:
        ArgType = ArgumentType::String;
        return S_OK;
    case ELEMENT_TYPE_PTR:
    case ELEMENT_TYPE_BYREF:
        ArgType = ArgumentType::DummyPtr;
        if (Signature >= endSignature)
        {
            return E_INVALIDARG;
        }

        CorSigUncompressElementType(Signature);
        return S_OK;

    // valuetype and class is identical for our purposes
    case ELEMENT_TYPE_VALUETYPE:
    case ELEMENT_TYPE_CLASS:
        ArgType = ArgumentType::Class;
        if (Signature >= endSignature)
        {
            return E_INVALIDARG;
        }
        UNREFERENCED_PARAMETER(MetaData);
        // right now just consume token. We could use token to get the class name with TypeFromToken() then use IMetaDataImport::GetTypeDefProps
        CorSigUncompressToken(Signature);
        return S_OK;
    case ELEMENT_TYPE_VAR:
        return E_NOTIMPL;
    case ELEMENT_TYPE_ARRAY:
        return E_NOTIMPL;
    case ELEMENT_TYPE_GENERICINST:
        return E_NOTIMPL;
    case ELEMENT_TYPE_TYPEDBYREF:
        return E_NOTIMPL;
    case ELEMENT_TYPE_I:
        ArgType = ArgumentType::DummyDword;
        return S_OK;
    case ELEMENT_TYPE_U:
        ArgType = ArgumentType::DummyDword;
        return S_OK;
    case ELEMENT_TYPE_FNPTR:
        return E_NOTIMPL;
    case ELEMENT_TYPE_OBJECT:
        ArgType = ArgumentType::DummyPtr;
        return S_OK;
    case ELEMENT_TYPE_SZARRAY:
    {
        if (Signature >= endSignature)
        {
            return E_INVALIDARG;
        }

        CorElementType arrayElementType = CorSigUncompressElementType(Signature);
        if (arrayElementType == ELEMENT_TYPE_U1) // byte
        {
            ArgType = ArgumentType::ByteArray;
        }
        return S_OK;
    }
    case ELEMENT_TYPE_MVAR:
        return E_NOTIMPL;
    case ELEMENT_TYPE_CMOD_REQD:
        return E_NOTIMPL;
    case ELEMENT_TYPE_CMOD_OPT:
        return E_NOTIMPL;
    case ELEMENT_TYPE_INTERNAL:
        return E_NOTIMPL;
    case ELEMENT_TYPE_MAX:
        return E_NOTIMPL;
    case ELEMENT_TYPE_MODIFIER:
        return E_NOTIMPL;
    case ELEMENT_TYPE_SENTINEL:
        return E_NOTIMPL;
    case ELEMENT_TYPE_PINNED:
        return E_NOTIMPL;
    default:
        return E_NOTIMPL;
    }
    
}