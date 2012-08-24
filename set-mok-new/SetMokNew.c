#include <efi.h>
#include <efilib.h>
#include "ca.h"

#define SHIM_LOCK_GUID \
        { 0x605dab50, 0xe046, 0x4300, {0xab, 0xb6, 0x3d, 0xd8, 0x10, 0xdd, 0x8b, 0x23} }

EFI_STATUS
efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
	EFI_GUID SHIM_GUID = SHIM_LOCK_GUID;
	EFI_STATUS efi_status;

	InitializeLib(image, systab);

	efi_status = uefi_call_wrapper(RT->SetVariable, 5, L"MokNew", &SHIM_GUID,
				       EFI_VARIABLE_NON_VOLATILE
				       | EFI_VARIABLE_BOOTSERVICE_ACCESS
				       | EFI_VARIABLE_RUNTIME_ACCESS,
				       sizeof(cert), cert);
	if (efi_status != EFI_SUCCESS) {
		Print(L"Failed to set variable %d\n", efi_status);
		return efi_status;
	}

	return efi_status;
}
