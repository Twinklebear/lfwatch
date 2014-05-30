#ifdef _WIN32

#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include <windows.h>
#include "lfwatch_win32.h"

namespace lfw {
void CALLBACK watch_callback(DWORD err, DWORD num_bytes, LPOVERLAPPED overlapped);

void register_watch(WatchData &watch){
	std::memset(&watch.info_buf[0], 0, watch.info_buf.size());
	bool status = ReadDirectoryChangesW(watch.dir_handle, &watch.info_buf[0],
		watch.info_buf.size(), watch.watch_subtree,
		FILE_NOTIFY_CHANGE_LAST_WRITE, NULL,
		&watch.overlapped, watch_callback);
	if (!status){
		std::cout << "Error registering watch\n";
	}
}
void CALLBACK watch_callback(DWORD err, DWORD num_bytes, LPOVERLAPPED overlapped){
	WatchData *watch = reinterpret_cast<WatchData*>(overlapped);
	switch (err){
		case ERROR_SUCCESS:
			std::cout << watch->dir_name << " success\n";
			break;
		case ERROR_OPERATION_ABORTED:
			std::cout << "aborted\n";
			return;
		default:
			std::cout << "error\n";
			return;
	}
	size_t offset = 0;
	do {
		PFILE_NOTIFY_INFORMATION info =
			reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&watch->info_buf[0] + offset);
		char fname[MAX_PATH] = { 0 };
		std::wcstombs(fname, info->FileName, info->FileNameLength);
		fname[info->FileNameLength + 1] = '\0';
		std::cout << "File: " << fname << "\n";
		offset = info->NextEntryOffset;
	}
	while (offset != 0);
	//Re-register to listen again
	register_watch(*watch);
}
void cancel(WatchData &watch){
	CancelIo(watch.dir_handle);
	//How can we determine what/how long to wait for any running stuff to finish?
	if (!HasOverlappedIoCompleted(&watch.overlapped)){
		SleepEx(5, true);
	}
	CloseHandle(watch.dir_handle);
}

WatchData::WatchData(HANDLE handle, const std::string &dir, bool watch_subtree)
	: dir_handle(handle), dir_name(dir), watch_subtree(watch_subtree)
{
	std::memset(&overlapped, 0, sizeof(overlapped));
}

WatchWin32::~WatchWin32(){
	for (auto &pair : watchers){
		cancel(pair.second);
	}
}
void WatchWin32::watch(const std::string &dir, bool watch_subtree){
	if (watchers.find(dir) != watchers.end()){
		std::cout << "already watching " << dir << "\n";
		return;
	}
	HANDLE handle = CreateFile(dir.c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL);
	if (handle == INVALID_HANDLE_VALUE){
		std::cerr << "Error creating handle\n";
		return;
	}
	auto it = watchers.emplace(std::make_pair(dir,
		WatchData{handle, dir, watch_subtree}));
	register_watch(it.first->second);
}
void WatchWin32::remove(const std::string &dir){
	auto fnd = watchers.find(dir);
	if (fnd != watchers.end()){
		cancel(fnd->second);
		watchers.erase(fnd);
	}
}
void WatchWin32::update(){
	MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
}
}

#endif

