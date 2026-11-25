#include "plugin.h"
#include <vector>
#include <string>

enum MenuId : int
{
    MENU_PUSH_ALL = 1,
    MENU_POP_ALL = 2
};

static const std::vector<std::string> kPushRegs = {
    "push rax", "push rcx", "push rdx", "push rbx",
    "push rbp", "push rsi", "push rdi",
    "push r8",  "push r9",  "push r10", "push r11",
    "push r12", "push r13", "push r14", "push r15"
};

static const std::vector<std::string> kPopRegs = {
    "pop r15", "pop r14", "pop r13", "pop r12",
    "pop r11", "pop r10", "pop r9",  "pop r8",
    "pop rdi", "pop rsi", "pop rbp",
    "pop rbx", "pop rdx", "pop rcx", "pop rax"
};

static void WriteSeq(const std::vector<std::string>& seq)
{
    if (!DbgIsDebugging())
        return;

    duint addr = DbgValFromString("dis.sel()");
    if (!addr)
        addr = DbgValFromString("cip");

    if (!addr)
        return;

    for (const auto& asmLine : seq)
    {
        if (!DbgAssembleAt(addr, asmLine.c_str()))
            break;

        BASIC_INSTRUCTION_INFO info{};
        DbgDisasmFastAt(addr, &info);
        if (info.size <= 0)
            break;

        addr += static_cast<duint>(info.size);
    }

    GuiUpdateAllViews();
}

extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    if (cbType != CB_MENUENTRY || !info)
        return;

    if (info->hEntry == MENU_PUSH_ALL)
        WriteSeq(kPushRegs);
    else if (info->hEntry == MENU_POP_ALL)
        WriteSeq(kPopRegs);
}

bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    return true;
}

void pluginStop()
{
}

void pluginSetup()
{
    _plugin_menuaddentry(hMenu, MENU_PUSH_ALL, "Push All Registers");
    _plugin_menuaddentry(hMenu, MENU_POP_ALL, "Pop All Registers");
}
