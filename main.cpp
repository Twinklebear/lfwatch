#include <clocale>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstring>
#include <windows.h>
#include "lfwatch_win32.h"

char notify_info[8 * 1024];
lfw::WatchData data;

void watch_dir(const std::string &dir);
void CALLBACK watch_callback(DWORD err_code, DWORD num_bytes, LPOVERLAPPED overlapped);

int main(int argc, char **argv){
	if (argc < 2){
		std::cout << "Usage ./exe <dir>\n";
		return 1;
	}
	lfw::WatchWin32 watcher;
	watcher.watch_dir("./", false);
	for (int i = 0; i < 4; ++i){
		std::cout << "Sleeping\n";
		SleepEx(6000, true);
		std::cout << "Updating\n";
		watcher.update();
		if (i == 2){
			std::cout << "Removing watch\n";
			watcher.remove_watch("./");
		}
	}
	CancelIo(data.dir_handle);
	CloseHandle(data.dir_handle);
	return 0;
}
void watch_dir(const std::string &dir){
	data.dir_name = dir;
	data.dir_handle = CreateFile(dir.c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	if (data.dir_handle == INVALID_HANDLE_VALUE){
		std::cout << "Error creating handle\n";
		return;
	}
	std::memset(&data.overlapped, 0, sizeof(data.overlapped));
	std::cout << "Waiting for notifications\n";
	bool status = ReadDirectoryChangesW(data.dir_handle, data.info_buf, sizeof(data.info_buf),
			false, FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &data.overlapped, lfw::watch_callback);
	if (!status){
		std::cout << "Error ReadDirectoryChangesW\n";
		return;
	}
	SleepEx(8000, true);
}
void CALLBACK watch_callback(DWORD err_code, DWORD num_bytes, LPOVERLAPPED overlapped){
	switch (err_code){
		case ERROR_SUCCESS:
			std::cout << "Operation successful\n";
			break;
		case ERROR_OPERATION_ABORTED:
			std::cout << "Operation cancelling\n";
			return;
		default:
			std::cout << "Something went wrong\n";
			return;
	}
	size_t offset = 0;
	do {
		PFILE_NOTIFY_INFORMATION info = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(notify_info + offset);
		char fname[MAX_PATH] = { 0 };
		std::wcstombs(fname, info->FileName, info->FileNameLength);
		fname[info->FileNameLength + 1] = '\0';
		std::cout << "File: " << fname << "\n";
		offset = info->NextEntryOffset;
	}
	while (offset != 0);
}

