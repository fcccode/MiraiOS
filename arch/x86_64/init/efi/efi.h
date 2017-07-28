#ifndef EFI_H
#define EFI_H

#include <stdint.h>

typedef void *EFI_FUNC;

typedef int64_t EFI_STATUS;

int64_t efiCall0(EFI_FUNC func);
int64_t efiCall1(EFI_FUNC func, uint64_t a1);
int64_t efiCall2(EFI_FUNC func, uint64_t a1, uint64_t a2);
int64_t efiCall3(EFI_FUNC func, uint64_t a1, uint64_t a2, uint64_t a3);
int64_t efiCall4(EFI_FUNC func, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4);
int64_t efiCall5(EFI_FUNC func, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5);

typedef void *EFI_HANDLE;

typedef struct {
	uint64_t Signature;
	uint32_t Revision;
	uint32_t HeaderSize;
	uint32_t CRC32;
	uint32_t Reserved;
} EFI_TABLE_HEADER;

typedef struct {
	EFI_FUNC Reset;
	EFI_FUNC OutputString;
	EFI_FUNC TestString;
	EFI_FUNC QueryMode;
	EFI_FUNC SetMode;
	EFI_FUNC SetAttribute;
	EFI_FUNC ClearScreen;
	EFI_FUNC SetCursorPosition;
	EFI_FUNC EnableCursor;
	void *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
	EFI_TABLE_HEADER Hdr;
	EFI_FUNC RaiseTPL;
	EFI_FUNC RestoreTPL;
	EFI_FUNC AllocatePages;
	EFI_FUNC FreePages;
	EFI_FUNC GetMemoryMap;
	EFI_FUNC AllocatePool;
	EFI_FUNC FreePool;
	EFI_FUNC CreateEvent;
	EFI_FUNC SetTimer;
	EFI_FUNC WaitForEvent;
	EFI_FUNC SignalEvent;
	EFI_FUNC CloseEvent;
	EFI_FUNC CheckEvent;
	EFI_FUNC InstalProtocolInterface;
	EFI_FUNC ReinstallProtocolInterface;
	EFI_FUNC UninstallProtocolInterface;
	EFI_FUNC HandleProtocol;
	void *Reserved;
	EFI_FUNC RegisterProtocolNotify;
	EFI_FUNC LocateHandle;
	EFI_FUNC LocateDevicePath;
	EFI_FUNC InstallConfigurationTable;
	EFI_FUNC LoadImage;
	EFI_FUNC StartImage;
	EFI_FUNC Exit;
	EFI_FUNC UnloadImage;
	EFI_FUNC ExitBootServices;
	EFI_FUNC GetNextMonotonicCount;
	EFI_FUNC Stall;
	EFI_FUNC SetWatchdogTimer;
	EFI_FUNC ConnectController;
	EFI_FUNC DisconnectController;
	EFI_FUNC OpenProtocol;
	EFI_FUNC CloseProtocol;
	EFI_FUNC OpenProtocolInformation;
	EFI_FUNC ProtocolsPerHandle;
	EFI_FUNC LocateHandleBuffer;
	EFI_FUNC LocateProtocol;
	EFI_FUNC InstallMultipleProtocolInterfaces;
	EFI_FUNC UninstallMultipleProtocolInterfaces;
	EFI_FUNC CalculateCRC32;
	EFI_FUNC CopyMem;
	EFI_FUNC SetMem;
	EFI_FUNC CreateEventEx;
} EFI_BOOT_SERVICES;

typedef struct {
	EFI_TABLE_HEADER Hdr;
	void *FirmwareVendor;
	uint32_t FirmwareRevision;

	EFI_HANDLE ConsoleInHandle;
	void *ConIn;
	EFI_HANDLE ConsoleOutHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
	EFI_HANDLE StandardErrorHandle;
	EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;

	void *RuntimeServices;
	EFI_BOOT_SERVICES *BootServices;

	uint64_t NumberOfTableEntries;
	void *ConfigurationTable;
} EFI_SYSTEM_TABLE;

#endif