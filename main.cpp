#include <string>
#include <vector>
#include <format>
#include <filesystem>
#include "aviutl.hpp"
#include "exedit.hpp"
#include "detours.4.0.1/detours.h"
#pragma comment(lib, "detours.4.0.1/detours.lib")

constexpr auto plugin_name = "フィルタ名調整";
constexpr auto plugin_information = "フィルタ名調整 r1 by 蛇色 / originator 兎 / AviUtl知識共有(Discord)";

std::string format = "{} / アニメーション効果";

std::filesystem::path get_module_file_name(HINSTANCE instance)
{
	char file_name[MAX_PATH] = {};
	::GetModuleFileNameA(instance, file_name, MAX_PATH);
	return file_name;
}

namespace exin
{
	DWORD exedit = 0;
	std::vector<std::string> animation_names;

	struct {
		ExEdit::Object** object_table = nullptr;
		ExEdit::Object** sorted_object_table = nullptr;
		ExEdit::Filter** filter_table = nullptr;
		int32_t* current_scene_index = nullptr;
		int32_t* current_object_index = nullptr;
		int32_t* current_filter_index = nullptr;
		int32_t* object_count = nullptr;
		int32_t* sorted_object_count = nullptr;
		uint8_t** exdata_table = nullptr;
		const char* animation_names = nullptr;
		HWND* filter_checkbox_table = nullptr;
	} variable;

	struct {
		BOOL (CDECL* update_controls)(int32_t object_index) = nullptr;
	} function;

	void init()
	{
		exedit = (decltype(exedit))::GetModuleHandleW(L"exedit.auf");
		variable.object_table = (decltype(variable.object_table))(exedit + 0x001E0FA4);
		variable.sorted_object_table = (decltype(variable.sorted_object_table))(exedit + 0x00168FA8);
		variable.filter_table = (decltype(variable.filter_table))(exedit + 0x00187C98);
		variable.current_object_index = (decltype(variable.current_object_index))(exedit + 0x00177A10);
		variable.exdata_table = (decltype(variable.exdata_table))(exedit + 0x001E0FA8);
		variable.animation_names = (decltype(variable.animation_names))(exedit + 0x000C1F08);
		variable.filter_checkbox_table = (decltype(variable.filter_checkbox_table))(exedit + 0x0014D368);
		function.update_controls = (decltype(function.update_controls))(exedit + 0x000305E0);

		{
			auto animation_name = variable.animation_names;
			while (animation_name[0])
			{
				auto length = strlen(animation_name);
				animation_names.emplace_back(animation_name, length);
				animation_name += length + 1;
			}
		}
	}

	ExEdit::Object* get_object(int32_t object_index) { return *variable.object_table + object_index; }
	ExEdit::Object* get_sorted_object(int32_t object_index) { return variable.sorted_object_table[object_index]; }
	ExEdit::Filter* get_filter(int32_t filter_id) { return variable.filter_table[filter_id]; }
	int32_t get_current_object_index() { return *variable.current_object_index; }

	uint32_t get_exdata_offset(ExEdit::Object* object, int32_t filter_index)
	{
		return object->exdata_offset + object->filter_param[filter_index].exdata_offset;
	}

	uint8_t* get_exdata(ExEdit::Object* object, int32_t filter_index)
	{
		uint8_t* exdata_table = *variable.exdata_table;
		auto offset = get_exdata_offset(object, filter_index);
		return exdata_table + offset + 0x0004;
	}

	const char* get_animation_name(size_t animation_type)
	{
		if (animation_type < animation_names.size())
			return animation_names[animation_type].c_str();
		else
			return "invalid_animation_type";
	}

	HWND get_filter_checkbox(int32_t filter_index) { return variable.filter_checkbox_table[filter_index]; }

	BOOL update_controls(int32_t object_index) { return function.update_controls(object_index); }
}

namespace set_checkbox_text
{
	BOOL (CDECL* orig_proc)(int32_t filter_index, int32_t y, LPCSTR filter_name, uint32_t no_folding, uint32_t gui) = nullptr;
	BOOL CDECL hook_proc(int32_t filter_index, int32_t y, LPCSTR filter_name, uint32_t no_folding, uint32_t gui)
	{
		try
		{
			if (strcmp(filter_name, "アニメーション効果"))
				throw L"フィルタがアニメーション効果ではありません";

			auto object_index = exin::get_current_object_index();
			if (object_index < 0) throw L"カレントオブジェクトが無効です";

			auto object = exin::get_object(object_index);
			if (!object) throw L"オブジェクトが無効です";
#if 0
			auto filter_id = object->filter_param[filter_index].id;
			if (filter_id < 0) throw L"フィルタIDが無効です";

			auto filter = exin::get_filter(filter_id);
			if (!filter) throw L"フィルタが無効です";
#endif
			auto exdata = exin::get_exdata(object, filter_index);
			if (!exdata) throw L"拡張データが無効です";

			auto animation_type = *(uint16_t*)(exdata + 0x00);
			auto animation_filter = *(uint16_t*)(exdata + 0x02);
			auto animation_name = (LPCSTR)(exdata + 0x04);

			if (!animation_name[0])
				animation_name = exin::get_animation_name(animation_type);

			auto display_name = std::vformat(format, std::make_format_args(animation_name));

			return orig_proc(filter_index, y, display_name.c_str(), no_folding, gui);
		}
		catch (LPCWSTR e)
		{
			(void)e;
		}
		catch (...)
		{
		}

		return orig_proc(filter_index, y, filter_name, no_folding, gui);
	}

	BOOL init()
	{
		orig_proc = decltype(orig_proc)(exin::exedit + 0x30380);
		DetourAttach(&(PVOID&)orig_proc, hook_proc);

		return TRUE;
	}

	BOOL exit()
	{
		DetourDetach(&(PVOID&)orig_proc, hook_proc);

		return TRUE;
	}
}

namespace on_command
{
	BOOL (CDECL* orig_proc)(int32_t object_index, int32_t filter_index, uint32_t command_id, uint32_t command_arg) = nullptr;
	BOOL CDECL hook_proc(int32_t object_index, int32_t filter_index, uint32_t command_id, uint32_t command_arg)
	{
		try
		{
			if (LOWORD(command_id) != 0x1e1c)
				throw L"コンボボックスコマンドではありません";

			if (object_index < 0) throw L"オブジェクトインデックスが無効です";

			auto object = exin::get_object(object_index);
			if (!object) throw L"オブジェクトが無効です";

			if (filter_index < 0) throw L"フィルタインデックスが無効です";

			auto filter_id = object->filter_param[filter_index].id;
			if (filter_id < 0) throw L"フィルタIDが無効です";

			auto filter = exin::get_filter(filter_id);
			if (!filter) throw L"フィルタが無効です";

			if (!filter->name || strcmp(filter->name, "アニメーション効果"))
				throw L"フィルタがアニメーション効果ではありません";

			auto result = orig_proc(object_index, filter_index, command_id, command_arg);
#if 1
			// チェックボックスの行だけを更新します。
			// 
			// チェックボックスのウィンドウ位置を取得します。
			auto checkbox = exin::get_filter_checkbox(filter_index);
			auto rc = RECT {}; ::GetWindowRect(checkbox, &rc);
			::MapWindowPoints(nullptr, ::GetParent(checkbox), (LPPOINT)&rc, 2);
			auto y = rc.top + 5;

			uint32_t no_folding = 0x04;
			if (!!(object->filter_status[filter_index] & ExEdit::Object::FilterStatus::Folding))
				no_folding = 0x00;

			uint32_t gui = 0x00;
			if (!!(object->filter_status[filter_index] & ExEdit::Object::FilterStatus::Gui))
				gui = 0x04;

			set_checkbox_text::hook_proc(filter_index, y, filter->name, no_folding, gui);
#else
			// この更新方法のほうが簡潔ですが、設定ダイアログの再描画量が多くなって重くなってしまいます。
			exin::update_controls(object_index);
#endif
			return result;
		}
		catch (LPCWSTR e)
		{
			(void)e;
		}
		catch (...)
		{
		}

		return orig_proc(object_index, filter_index, command_id, command_arg);
	}

	BOOL init()
	{
		orig_proc = decltype(orig_proc)(exin::exedit + 0x4A050);
		DetourAttach(&(PVOID&)orig_proc, hook_proc);

		return TRUE;
	}

	BOOL exit()
	{
		DetourDetach(&(PVOID&)orig_proc, hook_proc);

		return TRUE;
	}
}

BOOL func_init(AviUtl::FilterPlugin* fp)
{
	exin::init();

	{
		// ファイルにアクセスする準備をします。
		auto module_file_name = get_module_file_name(fp->dll_hinst);
		auto ini_file_name = module_file_name.replace_extension(".ini");

		// iniファイルから設定を読み込みます。
		char format[MAX_PATH] = {};
		::GetPrivateProfileStringA("config", "format", ::format.c_str(), format, std::size(format), ini_file_name.string().c_str());
		::format = format;
	}

	{
		DetourTransactionBegin();
		DetourUpdateThread(::GetCurrentThread());

		set_checkbox_text::init();
		on_command::init();

		DetourTransactionCommit();
	}

	return TRUE;
}

BOOL func_exit(AviUtl::FilterPlugin* fp)
{
	{
		DetourTransactionBegin();
		DetourUpdateThread(::GetCurrentThread());

		on_command::exit();
		set_checkbox_text::exit();

		DetourTransactionCommit();
	}

	{
		// ファイルにアクセスする準備をします。
		auto module_file_name = get_module_file_name(fp->dll_hinst);
		auto ini_file_name = module_file_name.replace_extension(".ini");

		// iniファイルに設定を書き込みます。
		char format[MAX_PATH] = {};
		::WritePrivateProfileStringA("config", "format", ::format.c_str(), ini_file_name.string().c_str());
	}

	return TRUE;
}

EXTERN_C AviUtl::FilterPluginDLL* WINAPI GetFilterTable()
{
	static AviUtl::FilterPluginDLL filter =
	{
		.flag =
			AviUtl::FilterPluginDLL::Flag::AlwaysActive |
			AviUtl::FilterPluginDLL::Flag::DispFilter |
			AviUtl::FilterPluginDLL::Flag::NoConfig |
			AviUtl::FilterPluginDLL::Flag::ExInformation,
		.name = plugin_name,
		.func_init = func_init,
		.func_exit = func_exit,
		.information = plugin_information,
	};

	return &filter;
}
