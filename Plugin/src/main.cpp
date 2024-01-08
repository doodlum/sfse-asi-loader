#include "SFSE/Stub.h"
#include <codecvt>

//MIT License
//
//	Copyright(c) 2018 ThirteenAG
//
//	Permission is hereby granted,
//	free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
//	The above copyright notice and this permission notice shall be included in all copies
//	or
//	substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS",
//	WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//	DAMAGES OR OTHER
//	LIABILITY,
//	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

DLLEXPORT constinit auto SFSEPlugin_Version = []() noexcept {
	SFSE::PluginVersionData data{};

	data.PluginVersion(Plugin::Version);
	data.PluginName(Plugin::NAME);
	data.AuthorName(Plugin::AUTHOR);
	data.UsesSigScanning(true);
	//data.UsesAddressLibrary(true);
	data.HasNoStructUse(true);
	//data.IsLayoutDependent(true);
	data.CompatibleVersions({ RUNTIME_VERSION_1_7_23, RUNTIME_VERSION_1_7_29, RUNTIME_VERSION_1_7_33, RUNTIME_VERSION_1_7_36, RUNTIME_VERSION_1_8_86 });

	return data;
}();

std::wstring GetCurrentDirectoryW()
{
	static constexpr auto INITIAL_BUFFER_SIZE = MAX_PATH;
	static constexpr auto MAX_ITERATIONS = 7;
	std::wstring ret;
	auto bufferSize = INITIAL_BUFFER_SIZE;
	for (size_t iterations = 0; iterations < MAX_ITERATIONS; ++iterations) {
		ret.resize(bufferSize);
		auto charsReturned = GetCurrentDirectoryW(bufferSize, ret.data());
		if (charsReturned < ret.length()) {
			ret.resize(charsReturned);
			return ret;
		} else {
			bufferSize *= 2;
		}
	}
	return L"";
}

HMODULE LoadLibraryW(const std::wstring& lpLibFileName)
{
	return LoadLibraryW(lpLibFileName.c_str());
}

std::wstring GetModuleFileNameW(HMODULE hModule)
{
	static constexpr auto INITIAL_BUFFER_SIZE = MAX_PATH;
	static constexpr auto MAX_ITERATIONS = 7;
	std::wstring ret;
	auto bufferSize = INITIAL_BUFFER_SIZE;
	for (size_t iterations = 0; iterations < MAX_ITERATIONS; ++iterations) {
		ret.resize(bufferSize);
		auto charsReturned = GetModuleFileNameW(hModule, &ret[0], bufferSize);
		if (charsReturned < ret.length()) {
			ret.resize(charsReturned);
			return ret;
		} else {
			bufferSize *= 2;
		}
	}
	return L"";
}

std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

void FindFiles(WIN32_FIND_DATAW* fd)
{
	auto dir = GetCurrentDirectoryW();

	HANDLE asiFile = FindFirstFileW(L"*.asi", fd);
	if (asiFile != INVALID_HANDLE_VALUE) {
		do {
			if (!(fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				auto pos = wcslen(fd->cFileName);

				if (fd->cFileName[pos - 4] == '.' &&
					(fd->cFileName[pos - 3] == 'a' || fd->cFileName[pos - 3] == 'A') &&
					(fd->cFileName[pos - 2] == 's' || fd->cFileName[pos - 2] == 'S') &&
					(fd->cFileName[pos - 1] == 'i' || fd->cFileName[pos - 1] == 'I')) {
					auto path = dir + L'\\' + fd->cFileName;

					if (GetModuleHandle(path.c_str()) == NULL) {
						INFO("Loading {}", ws2s(path));

						auto h = LoadLibraryW(path);
						SetCurrentDirectoryW(dir.c_str());  // In case asi switched it

						if (h == NULL) {
							auto e = GetLastError();
							if (e != ERROR_DLL_INIT_FAILED && e != ERROR_BAD_EXE_FORMAT)  // In case dllmain returns false or IMAGE_MACHINE is not compatible
							{
								ERROR("Failed to load plugin {} : Error {}", ws2s(path), e);
							} else if (e != ERROR_DLL_INIT_FAILED) {
								INFO("Failed to load plugin {} : ERROR_DLL_INIT_FAILED", ws2s(path));
							} else {
								INFO("Failed to load plugin {} : ERROR_BAD_EXE_FORMAT", ws2s(path));
							}
						} else {
							INFO("Loaded plugin successfully");

							auto procedure = (void (*)())GetProcAddress(h, "InitializeASI");

							if (procedure != NULL) {
								INFO("Calling InitializeASI");
								procedure();
							}
						}
					} else {
						INFO("Plugin was already loaded, ignoring");
					}
				}
			}
		} while (FindNextFileW(asiFile, fd));
		FindClose(asiFile);
	}
}

void LoadPlugins()
{
	auto oldDir = GetCurrentDirectoryW();  // Store the current directory

	auto szSelfPath = GetModuleFileNameW(nullptr).substr(0, GetModuleFileNameW(nullptr).find_last_of(L"/\\") + 1);
	SetCurrentDirectoryW(szSelfPath.c_str());

	WIN32_FIND_DATAW fd;

	{
		INFO("Loading plugins from {}", ws2s(GetCurrentDirectoryW()));
		FindFiles(&fd);
	}

	SetCurrentDirectoryW(szSelfPath.c_str());

	if (SetCurrentDirectoryW(L"Plugins\\")) {
		INFO("Loading plugins from {}", ws2s(GetCurrentDirectoryW()));
		FindFiles(&fd);
	}

	SetCurrentDirectoryW(szSelfPath.c_str());

	if (SetCurrentDirectoryW(L"Data\\")) {
		INFO("Loading plugins from {}", ws2s(GetCurrentDirectoryW()));
		FindFiles(&fd);
	}

	SetCurrentDirectoryW(oldDir.c_str());  // Reset the current directory
}

DLLEXPORT bool SFSEAPI SFSEPlugin_Preload(SFSEInterface* a_sfse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {
		Sleep(100);
	}
#endif

	SFSE::Init(a_sfse);

	DKUtil::Logger::Init(Plugin::NAME, std::to_string(Plugin::Version));

	INFO("{} v{} loaded", Plugin::NAME, Plugin::Version);

	LoadPlugins();

	return true;
}

DLLEXPORT bool SFSEAPI SFSEPlugin_Load(SFSEInterface* a_sfse)
{
	return true;
}