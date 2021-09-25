#include <range/v3/view/span.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/view.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/contains.hpp>
#include <thread>
#define USE_STANDARD_FILE_FUNCTIONS
#include <hexrays.hpp>
#include <Windows.h>
#include <iostream>
#include <frame.hpp>
#include <struct.hpp>
#include <name.hpp>
#include <iostream>
#include <bytes.hpp>
#include <segment.hpp>
#include <cstdlib>

extern "C" HANDLE hCurrModule;

__declspec(dllexport) BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
) {
	hCurrModule = hinstDLL;
	return TRUE;
}

 // Hex-Rays API pointer
hexdsp_t* hexdsp = nullptr;

extern "C" EXCEPTION_DISPOSITION
__GSHandlerCheck_EH4(
	IN PEXCEPTION_RECORD ExceptionRecord,
	IN PVOID EstablisherFrame,
	IN OUT PCONTEXT ContextRecord,
	IN OUT PDISPATCHER_CONTEXT DispatcherContext
) {
	return ExceptionContinueSearch;
}

//--------------------------------------------------------------------------
struct plugin_ctx_t : public plugmod_t
{
	~plugin_ctx_t()
	{
		if (hexdsp != nullptr)
			term_hexrays_plugin();
	}
	virtual bool idaapi run(size_t) override;
};

struct encapsulatelvars : user_lvar_modifier_t {
	tid_t newstructid;

	func_t* pfunc;

	virtual bool idaapi modify_lvars(lvar_uservec_t* lvinf) {

		lvar_locator_t mainvar;

		qstring newstructname = get_struc_name(newstructid);

		//get_named_type(get_idati(), newstructname.c_str(), BTF_STRUCT)

		bool errtinfo;

		tinfo_t newstructtinfo{};

		errtinfo = newstructtinfo.get_named_type(get_idati(), newstructname.c_str());

		for (auto& lvarsavedinfo : lvinf->lvvec)
			if (auto& lvar = lvarsavedinfo.ll; lvar.is_stk_var() && lvar.location.stkoff() == 0) {
				mainvar = lvar;
				lvarsavedinfo.type = newstructtinfo;
				lvarsavedinfo.flags = LVINF_FORCE;
				lvarsavedinfo.size = get_struc_size(newstructid);
				break;
			}
		for (auto& lvarsavedinfo : lvinf->lvvec)
			if (auto& lvar = lvarsavedinfo.ll; lvar != mainvar && lvar.is_stk_var())
				lvinf->lmaps.insert({ mainvar, lvar });

		save_user_lvar_settings(pfunc->start_ea, *lvinf);

		return true;
		/*auto lvars = funcdec->get_lvars();

		qstring newstructname = get_struc_name(newstructid);

		//get_named_type(get_idati(), newstructname.c_str(), BTF_STRUCT)

		bool errtinfo;

		tinfo_t newstructtinfo{};

		errtinfo = newstructtinfo.get_named_type(get_idati(), newstructname.c_str());

		for (auto& lvar : *lvars)
			if (lvar.location.has_stkoff() && lvar.location.stkoff() == 0)
				errtinfo = lvar.set_lvar_type(newstructtinfo);

		funcdec->build_c_tree();*/
	}
};

extern "C" int main(int argc, const char** argv, char** env);

int YourReportHook(int reportType, char* message, int* returnValue) {
	printf("%s\n", message);
	__debugbreak();
	throw std::tuple{ reportType, message, returnValue };
}

//--------------------------------------------------------------------------
bool idaapi plugin_ctx_t::run(size_t) {
	//__try
	//{

	const char* pargv[] = { "cparser2.exe", "src/main.pl", nullptr, nullptr };

	int argc = _countof(pargv) - 1;

	qstring ctarget;

	qgetenv("CTARGET", &ctarget);

	2[pargv] = ctarget.c_str();

	_CrtSetReportHook(YourReportHook);

	std::thread{ main, argc, pargv, nullptr }.join();

	return true;

	PVOID phandler = AddVectoredExceptionHandler(1, [](_EXCEPTION_POINTERS* ExceptionInfo) -> LONG {
		std::cout << "exception at" << ExceptionInfo->ExceptionRecord->ExceptionAddress << std::endl;
		Sleep(INFINITE);
		return EXCEPTION_CONTINUE_SEARCH;
		});

	ea_t currea;

	HANDLE hmemlow = CreateMemoryResourceNotification(LowMemoryResourceNotification);

	std::cout << " starting\n" << std::endl;

	goto realstart;

	for (auto i : ranges::iota_view{ 0, get_segm_qty() })
		if (auto segment = getnseg(i); segment->type == SEG_DATA
			&& !ranges::contains(std::array{ 0x0628000/*, 0x06283C8*/ }, segment->start_ea)) {
			currea = segment->start_ea;

			auto newstructid = add_struc(BADADDR, nullptr);

			bool bgetyp;

			set_struc_align(get_struc(newstructid), 1);

			do {
				struc_error_t strucerr;

				tinfo_t type;

				qstring name;

				opinfo_t optionalinfo;

				//auto newstruct = get_struc(newstructid);

				bgetyp = get_type(currea, &type, GUESSED_WEAK);
				get_ea_name(&name, currea);
				flags_t flags;
				opinfo_t* opinfo;
				opinfo_t tmp;
				//if (currea == 0x0065D180)
					//std::cout << "arr" << std::endl;
				//if (is_align(get_full_flags(currea)))
					/*std::cout << std::hex << currea << " " << get_item_size(currea) << std::endl,
					flags = strlit_flag(),
					(opinfo = &tmp)->strtype = STRTYPE_C;*/
					/*for (asize_t i : ranges::iota_view{ (asize_t)0, get_item_size(currea) })
						if ((strucerr = strucerr = add_struc_member(get_struc(newstructid), !name.empty() ? name.c_str() : nullptr,
							currea - segment->start_ea + i, byte_flag(), nullptr, 1)) != STRUC_ERROR_MEMBER_OK)
							std::cout << strucerr << " error creating member\n" << std::endl, Sleep(INFINITE);
						else;
				else */if ((strucerr = add_struc_member(get_struc(newstructid), !name.empty() ? name.c_str() : nullptr,
					currea - segment->start_ea, get_full_flags(currea),
					get_opinfo(&optionalinfo, currea, 0, get_full_flags(currea)), get_item_size(currea))) != STRUC_ERROR_MEMBER_OK)
					std::cout << strucerr << " error creating member\n" << std::endl, Sleep(INFINITE);

				//del_value(currea);
				del_items(currea);
				BOOL blow;
				//if (QueryMemoryResourceNotification(hmemlow, &blow), blow)
					//break;
				//if (currea > 0x065D429) break;

			} while ((currea = next_head(currea, segment->end_ea)) != BADADDR);

			/*opinfo_t newstructopinfo{ .tid = newstructid };

			qstring newstructname = get_struc_name(newstructid);

			tinfo_t newstructtinfo{};

			bgetyp = newstructtinfo.get_named_type(get_idati(), newstructname.c_str());*/

			std::cout << " replacing\n" << std::endl;

			//set_type(segment->start_ea, newstructtinfo, GUESSED_DATA, true);

			create_struct(segment->start_ea, get_struc_size(newstructid), newstructid, true);

			//goto realend;
			//break;

			//set_opinfo(segment->start_ea, 0, get_full_flags(segment->start_ea), &newstructopinfo);
			//return true;
		}
	std::cout << " ending\n" << std::endl;



	//goto realend;

	//return true;
realstart:
	for (auto i : ranges::iota_view{ (size_t)0, get_func_qty() }) {

		func_t* pfn = getn_func(i);//get_func(0x0499670)

		range_t framepart;

		get_frame_part(&framepart, pfn, FPC_LVARS);

		auto frame = get_frame(pfn);

		if (!frame) continue;

		auto newstructid = add_struc(BADADDR, nullptr);

		auto newstruct = get_struc(newstructid);

		opinfo_t tmp[1];

		struc_error_t strucerr;

		for (auto& member : ranges::span{ frame->members, frame->memqty })
			if (qstring name = get_member_name(member.id); !name.empty()) if (member.get_soff() < framepart.end_ea)
				strucerr = add_struc_member(newstruct, !name.empty() ? name.c_str() : nullptr, member.get_soff(), member.flag, retrieve_member_info(tmp, &member), member.eoff - member.get_soff());
			else break;
			else std::cout << "empty struc member name" << std::endl, Sleep(INFINITE);

		del_struc_members(frame, framepart.start_ea, framepart.end_ea);

		opinfo_t newstructop = { .tid = newstructid };

		strucerr = add_struc_member(frame, "vars", framepart.start_ea, stru_flag(), &newstructop, get_struc_size(newstructid));

		/*qstring newstructname = get_struc_name(newstructid);

		//get_named_type(get_idati(), newstructname.c_str(), BTF_STRUCT)

		bool errtinfo;

		tinfo_t newstructtinfo{};

		errtinfo = newstructtinfo.get_named_type(get_idati(), newstructname.c_str());

		smt_code_t errcodesmt = set_member_tinfo(frame, frame->members, framepart.start_ea, newstructtinfo, 0x0010 | 0x0001);

		/*hexrays_failure_t hf;

		cfuncptr_t funcdec = decompile_func(pfn, &hf);

		encapsulatelvars lvarsmodifier;

		lvarsmodifier.newstructid = newstructid;

		lvarsmodifier.pfunc = pfn;

		modify_user_lvars(pfn->start_ea, lvarsmodifier);*/

		//return true;
	}
	//}
	//__except (([](unsigned int code, struct _EXCEPTION_POINTERS* ep) {


	//}
realend:
	//decompile_many("traod_out.c", nullptr, VDRUN_NEWFILE | VDRUN_SILENT | VDRUN_CMDLINE | 0);

	std::cout << "saving database" << std::endl;

	save_database("traod_out.idb", 0);

	std::cout << "real end\n" << std::endl;

	RemoveVectoredContinueHandler(phandler);

	return true;
}

//--------------------------------------------------------------------------
static plugmod_t* idaapi init()
{
	//FreeConsole();
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	if (!init_hexrays_plugin())
		return nullptr; // no decompiler
	const char* hxver = get_hexrays_version();
	//msg("Hex-rays version %s has been detected, %s ready to use\n",
		//hxver, PLUGIN.wanted_name);
	//qstring scriptname;
	//qgetenv("SCRIPTNAME", &scriptname);
	//std::cout << scriptname.c_str() << std::endl;
	//Sleep(INFINITE);
	return new plugin_ctx_t;
}

//--------------------------------------------------------------------------
static char comment[] = "Sample9 plugin for Hex-Rays decompiler";

//--------------------------------------------------------------------------
//
//      PLUGIN DESCRIPTION BLOCK
//
//--------------------------------------------------------------------------
plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  PLUGIN_MULTI,         // The plugin can work with multiple idbs in parallel
  init,                 // initialize
  nullptr,
  nullptr,
  comment,              // long comment about the plugin
  nullptr,              // multiline help about the plugin
  "cparser2",  // the preferred short name of the plugin
  nullptr,              // the preferred hotkey to run the plugin
};


/*
	// Generate microcode. This call returns fully optimized microcode.
	// If desired, we could hook to decompiler events and return MERR_STOP
	// to return microcode from previous analysis stages. Another and easier
	// way of obtaining microcode of earlier stages is to explicitly specify
	// the required maturity level in the gen_mircocode() call.
	hexrays_failure_t hf;
	mba_t* mba = gen_microcode(pfn, &hf, NULL, DECOMP_NO_WAIT);
	FILE* stream;
	int  fd, count = 0;
	char inbuf[128];
	_sopen_s(&fd, "test_out.txt", _O_WRONLY | _O_TEXT | _O_TRUNC, _SH_DENYRW, _S_IWRITE);
	_flushall();

	_close(fd);
	for (auto block = mba->blocks; block; block = block->nextb) {
		for (auto instr = block->head; instr; instr = instr->next) {

			auto op = instr->l;

			//exit(0);
		}

		char* message;

		// Open a file.
		if (_sopen_s(&fd, "test_out.txt", _O_WRONLY | _O_TEXT | _O_APPEND, _SH_DENYRW, _S_IWRITE))
			message = strerror(errno);
		//exit(1);

	// Get stream from file descriptor.
		if ((stream = _fdopen(fd, "w")) == NULL)
			exit(1);


		file_printer_t fp{ stream };
		block->print(fp);


		_flushall();

		_close(fd);
	}

	exit(0);
	return true;*/

	/*func_t* pfn = getn_func(0);

	hexrays_failure_t hf;
	cfuncptr_t cfunc = decompile(pfn, &hf, DECOMP_WARNINGS);

	lvars_t* plvars = cfunc->get_lvars();

	for(auto &lvar : *plvars)*/