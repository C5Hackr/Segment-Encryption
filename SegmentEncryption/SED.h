#include <Windows.h>

#define USE_XOR_ENCRYPTION TRUE

#if USE_XOR_ENCRYPTION
unsigned char xor_key[] = "YwAYwAonvsgHUbnoYwAonvsgHUbnnvsgHUbn";
size_t xor_key_size = NULL;
#endif

typedef struct {
	uintptr_t FunctionAddress;
	uintptr_t ReturnAddress;
	__int64 functionSize;
	char* originalInstructions;
	BOOL IsJMPReturn;
} EncryptedFunctionList;

EncryptedFunctionList* EncryptedFunctions = NULL;
size_t num_EncryptedFunctions = 0;

BOOL EncryptHandlerInitialized = FALSE;

#pragma optimize("", off)
__declspec(dllexport) int endfunction(int a, int b)
{
	return a + b;
}
#pragma optimize("", on)

#if USE_XOR_ENCRYPTION
void xor_encrypt(unsigned char* data, size_t data_len, unsigned char* key, size_t key_len)
{
	for (size_t i = 0; i < data_len; i++)
	{
		data[i] ^= key[i % key_len];
	}
}

void xor_decrypt(unsigned char* data, size_t data_len, unsigned char* key, size_t key_len)
{
	for (size_t i = 0; i < data_len; i++)
	{
		data[i] ^= key[i % key_len];
	}
}
#endif

__declspec(noinline) void EncryptCodeSection(LPVOID address, char* originalInstructions, int SIZE_OF_FUNCTION)
{
	memcpy(originalInstructions, address, SIZE_OF_FUNCTION);
#if USE_XOR_ENCRYPTION
	xor_encrypt((unsigned char*)originalInstructions, SIZE_OF_FUNCTION, xor_key, xor_key_size);
#endif
	DWORD oldProtect;
	VirtualProtect(address, SIZE_OF_FUNCTION, PAGE_EXECUTE_READWRITE, &oldProtect);
	for (int i = 0; i < SIZE_OF_FUNCTION; i++)
	{
#if _WIN64
		* ((char*)((uintptr_t)address + i)) = 0x1F;
#else
		* ((char*)((uintptr_t)address + i)) = 0xFE;
#endif
	}
	VirtualProtect(address, SIZE_OF_FUNCTION, oldProtect, &oldProtect);
}

__declspec(noinline) BOOL SetBreakpoint(LPVOID address)
{
	DWORD oldProtect;
	VirtualProtect(address, sizeof(char), PAGE_EXECUTE_READWRITE, &oldProtect);
	*((char*)address) = 0xCC;
	VirtualProtect(address, sizeof(char), oldProtect, &oldProtect);
	return TRUE;
}

__declspec(noinline) LONG WINAPI VEHDecryptionHandler(PEXCEPTION_POINTERS exceptions)
{
	if (exceptions->ExceptionRecord->ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION)
	{
		for (size_t i = 0; i < num_EncryptedFunctions; i++)
		{
			if ((uintptr_t)((uintptr_t)exceptions->ExceptionRecord->ExceptionAddress) == (uintptr_t)EncryptedFunctions[i].FunctionAddress)
			{
				DWORD oldProtect;
				VirtualProtect((LPVOID)EncryptedFunctions[i].FunctionAddress, EncryptedFunctions[i].functionSize, PAGE_EXECUTE_READWRITE, &oldProtect);
#if USE_XOR_ENCRYPTION
				xor_decrypt((unsigned char*)EncryptedFunctions[i].originalInstructions, EncryptedFunctions[i].functionSize, xor_key, xor_key_size);
#endif
				memcpy((LPVOID)EncryptedFunctions[i].FunctionAddress, EncryptedFunctions[i].originalInstructions, EncryptedFunctions[i].functionSize);
				VirtualProtect((LPVOID)EncryptedFunctions[i].FunctionAddress, EncryptedFunctions[i].functionSize, oldProtect, &oldProtect);
				SetBreakpoint((LPVOID)EncryptedFunctions[i].ReturnAddress);
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}
	else if (exceptions->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		for (size_t i = 0; i < num_EncryptedFunctions; i++)
		{
			if ((uintptr_t)((uintptr_t)exceptions->ExceptionRecord->ExceptionAddress) == (uintptr_t)EncryptedFunctions[i].ReturnAddress)
			{
				DWORD oldProtect;
				VirtualProtect(exceptions->ExceptionRecord->ExceptionAddress, EncryptedFunctions[i].functionSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				if (EncryptedFunctions[i].IsJMPReturn)
				{
					*((char*)exceptions->ExceptionRecord->ExceptionAddress) = 0xE9;
				}
				else
				{
					*((char*)exceptions->ExceptionRecord->ExceptionAddress) = 0xE8;
				}
				VirtualProtect(exceptions->ExceptionRecord->ExceptionAddress, EncryptedFunctions[i].functionSize, oldProtect, &oldProtect);
				EncryptCodeSection((LPVOID)EncryptedFunctions[i].FunctionAddress, EncryptedFunctions[i].originalInstructions, EncryptedFunctions[i].functionSize);
				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}
	else
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

__declspec(noinline) void EncryptFunction(uintptr_t functionPointer)
{
	if (!EncryptHandlerInitialized)
	{
		xor_key_size = strlen((char*)xor_key);
		EncryptHandlerInitialized = TRUE;
		AddVectoredExceptionHandler(1, &VEHDecryptionHandler);
	}
	num_EncryptedFunctions++;
	EncryptedFunctions = (EncryptedFunctionList*)realloc(EncryptedFunctions, num_EncryptedFunctions * sizeof(EncryptedFunctionList));
	EncryptedFunctionList* currentHookInfo = &EncryptedFunctions[num_EncryptedFunctions - 1];
	int SIZE_OF_FUNCTION = 0;
	unsigned char* current_address = (unsigned char*)((void*)functionPointer);
	while (TRUE)
	{
		BYTE* ptr = (BYTE*)current_address;
		if (ptr[0] == 0xE9 && *((DWORD*)(current_address + 1)) == ((DWORD)endfunction - ((DWORD)current_address + 5)))
		{
			currentHookInfo->IsJMPReturn = TRUE;
			currentHookInfo->ReturnAddress = (uintptr_t)current_address;
			break;
		}
		else if (ptr[0] == 0xE8 && *((DWORD*)(current_address + 1)) == ((DWORD)endfunction - ((DWORD)current_address + 5)))
		{
			currentHookInfo->IsJMPReturn = FALSE;
			currentHookInfo->ReturnAddress = (uintptr_t)current_address;
			break;
		}
		current_address++;
		SIZE_OF_FUNCTION++;
	}
	currentHookInfo->FunctionAddress = functionPointer;
	currentHookInfo->functionSize = SIZE_OF_FUNCTION;
	currentHookInfo->originalInstructions = (char*)malloc(SIZE_OF_FUNCTION * sizeof(char));
	memcpy(currentHookInfo->originalInstructions, (void*)functionPointer, SIZE_OF_FUNCTION);
	EncryptCodeSection((LPVOID)functionPointer, currentHookInfo->originalInstructions, SIZE_OF_FUNCTION);
}
