#ifdef _WIN32

#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include <windows.h>

#ifndef NO_SDL
#include <SDL.h>
#endif

#include "lfwatch_win32.h"

namespace lfw {
//Callback for Win32 to tell us about file events
void CALLBACK watch_callback(DWORD err, DWORD num_bytes, LPOVERLAPPED overlapped);

//Log out the error message for an error code
std::string get_error_msg(DWORD err){
	LPSTR err_msg;
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&err_msg, 0, nullptr);
	std::string msg{err_msg, size};
	LocalFree(err_msg);
	return msg;
}
void register_watch(WatchData &watch){
	std::memset(&watch.info_buf[0], 0, watch.info_buf.size());
	bool status = ReadDirectoryChangesW(watch.dir_handle, &watch.info_buf[0],
		watch.info_buf.size(), false, watch.filter, nullptr,
		&watch.overlapped, watch_callback);
	if (!status){
		std::cerr << "Error registering watch on " << watch.dir_name
			<< ": " << get_error_msg(GetLastError()) << "\n";
	}
}
//Win32 returns different enums in the file notify action member than what you
//request in ReadDirectoryW so fix its stupidity and re-map to our enums
uint32_t remap_file_action(DWORD action){
	uint32_t remap = 0;
	if (action & FILE_ACTION_ADDED){
		//Add support?
	}
	if (action & FILE_ACTION_MODIFIED){
		remap |= Notify::CHANGE_LAST_WRITE | Notify::CHANGE_ATTRIBUTES;
	}
	if (action & FILE_ACTION_REMOVED){
		//Add support?
	}
	if (action & FILE_ACTION_RENAMED_NEW_NAME){
		//Migrate our file name change to this method?
	}
	if (action & FILE_ACTION_RENAMED_OLD_NAME){
		//Migrate our file name change to this method?
	}
	return remap;
}
void emit_events(WatchData &watch){
	PFILE_NOTIFY_INFORMATION info;
	size_t offset = 0;
	do {
		info = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(&watch.info_buf[0] + offset);
		char fname[MAX_PATH + 1] = { 0 };
		std::wcstombs(fname, info->FileName, info->FileNameLength);
		//TODO: The NOTIFY_INFORMATION Action member is different from
		//the listener mask we can set! And has fewer values!
		//Maybe I'll only listen for write, add, delete, and moving of
		//files
#ifdef NO_SDL
		watch.callback(EventData{watch.dir_name, fname, watch.filter,
			remap_file_action(info->Action)});
#else
		SDL_Event sdl_evt;
		SDL_zero(sdl_evt);
		sdl_evt.type = WatchWin32::event();
		sdl_evt.user.data1 = new EventData{watch.dir_name, fname, watch.filter,
			remap_file_action(info->Action)};
		sdl_evt.user.data2 = nullptr;
		SDL_PushEvent(&sdl_evt);
#endif
		offset += info->NextEntryOffset;
	}
	while (info->NextEntryOffset != 0);
}
void CALLBACK watch_callback(DWORD err, DWORD num_bytes, LPOVERLAPPED overlapped){
	//Other errors ignored, Win32 gives me an invalid err when cancelling
	if (err == ERROR_SUCCESS){
		WatchData *watch = reinterpret_cast<WatchData*>(overlapped);
		emit_events(*watch);
		//Re-register to listen again
		register_watch(*watch);
	}
}
void cancel(WatchData &watch){
	CancelIo(watch.dir_handle);
	//How can we determine what/how long to wait for any running stuff to finish?
	if (!HasOverlappedIoCompleted(&watch.overlapped)){
		SleepEx(5, true);
	}
	CloseHandle(watch.dir_handle);
}

#ifdef NO_SDL
WatchData::WatchData(HANDLE handle, const std::string &dir, uint32_t filter, const Callback &cb)
	: dir_handle(handle), dir_name(dir), filter(filter), callback(cb)
{
	std::memset(&overlapped, 0, sizeof(overlapped));
}
#else
WatchData::WatchData(HANDLE handle, const std::string &dir, uint32_t filter)
	: dir_handle(handle), dir_name(dir), filter(filter)
{
	std::memset(&overlapped, 0, sizeof(overlapped));
}
#endif

#ifndef NO_SDL
//SDL uses (Uint32)-1 for invalid event code
uint32_t WatchWin32::event_code = -1;
#endif

WatchWin32::WatchWin32(){
#ifndef NO_SDL
	//Check if we've initialized the SDL event code, invalid ids are
	//(Uint32)-1
	if (event_code == static_cast<uint32_t>(-1)){
		event_code = SDL_RegisterEvents(1);
		if (event_code == static_cast<uint32_t>(-1)){
			std::cerr << "Failed to get event code from SDL: "
				<< SDL_GetError() << std::endl;
			assert(false);
		}
	}
#endif

}
WatchWin32::~WatchWin32(){
	for (auto &pair : watchers){
		cancel(pair.second);
	}
}
#ifdef NO_SDL
void WatchWin32::watch(const std::string &dir, uint32_t filters, const Callback &callback)
#else
void WatchWin32::watch(const std::string &dir, uint32_t filters)
#endif
{
	auto fnd = watchers.find(dir);
	if (fnd != watchers.end()){
		//If we're updating an existing watch with new filters or subtree status
		if (fnd->second.filter != filters){
			fnd->second.filter = filters;
			register_watch(fnd->second);
		}
		return;
	}
	HANDLE handle = CreateFile(dir.c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		nullptr);
	if (handle == INVALID_HANDLE_VALUE){
		std::cerr << "Error creating handle for " << dir
			<< ": " << get_error_msg(GetLastError()) << "\n";
		return;
	}
#ifdef NO_SDL
	auto it = watchers.emplace(std::make_pair(dir, WatchData{handle, dir, filters, callback}));
#else
	auto it = watchers.emplace(std::make_pair(dir, WatchData{handle, dir, filters}));
#endif
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
	MsgWaitForMultipleObjectsEx(0, nullptr, 0, QS_ALLINPUT, MWMO_ALERTABLE);
}
#ifndef NO_SDL
uint32_t WatchWin32::event(){
	return event_code;
}
#endif
}

#endif

