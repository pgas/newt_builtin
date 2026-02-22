
#include <config.h>

#include <cstdio>
#include <cstring>
#include <map>
#include <memory>
#include <string>

extern "C" {
#include <newt.h>
#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"
}

#include "newt_arg_parser.hpp"
#include "newt_init_guard.hpp"
#include "newt_wrappers.hpp"

// ─── per-component data storage ───────────────────────────────────────────────
// Checkbox result chars: newtCheckbox requires a non-null char* that it updates
// in-place whenever the checkbox state changes.  We heap-allocate one char per
// checkbox component and keep it alive in this map.
static std::map<newtComponent, std::unique_ptr<char>> g_checkbox_results;

// Entry filter bash functions: maps a newtComponent to the name of the bash
// function that should be called as the entry filter.
static std::map<newtComponent, std::string> g_entry_filters;

// Suspend callback: name of the bash function registered via SetSuspendCallback.
static std::string g_suspend_callback_fn;

// Destroy callbacks: maps a newtComponent to the bash expression to evaluate
// when that component is destroyed by newtComponentAddDestroyCallback.
static std::map<newtComponent, std::string> g_destroy_callbacks;

// Component callbacks: maps a newtComponent to a {bashExpr, dataStr} pair
// registered via ComponentAddCallback.  The shim sets NEWT_COMPONENT and
// NEWT_CB_DATA before evaluating the expression.
static std::map<newtComponent, std::pair<std::string, std::string>> g_component_callbacks;

// ─── entry filter C shim ──────────────────────────────────────────────────────
// Called by libnewt for every keystroke in a filtered entry widget.  Looks up
// the bash function registered for 'co', sets NEWT_ENTRY / NEWT_CH /
// NEWT_CURSOR, evaluates the function and returns the (possibly filtered) char.
static int entry_filter_shim(newtComponent co, void* /*data*/, int ch,
                              int cursor) {
    auto it = g_entry_filters.find(co);
    if (it == g_entry_filters.end()) return ch;

    std::string co_str = to_bash_string(co);
    char ch_str[8];   std::snprintf(ch_str,  sizeof(ch_str),  "%d", ch);
    char cur_str[16]; std::snprintf(cur_str, sizeof(cur_str), "%d", cursor);

    builtin_bind_variable(const_cast<char*>("NEWT_ENTRY"),  const_cast<char*>(co_str.c_str()), 0);
    builtin_bind_variable(const_cast<char*>("NEWT_CH"),     ch_str,  0);
    builtin_bind_variable(const_cast<char*>("NEWT_CURSOR"), cur_str, 0);

    // evalstring needs a writable copy of the command string.
    const std::string& s = it->second;
    char* cmd = reinterpret_cast<char*>(xmalloc(s.size() + 1));
    std::memcpy(cmd, s.c_str(), s.size() + 1);
    int ret = evalstring(cmd, nullptr, 0);
    return (ret == 0) ? ch : 0;
}

// Called by libnewt when CTRL+Z is pressed while a form is running.  Looks up
// the registered bash function and evaluates it.
static void suspend_callback_shim(void* /*data*/) {
    if (g_suspend_callback_fn.empty()) return;
    const std::string& s = g_suspend_callback_fn;
    char* cmd = reinterpret_cast<char*>(xmalloc(s.size() + 1));
    std::memcpy(cmd, s.c_str(), s.size() + 1);
    evalstring(cmd, nullptr, 0);
}

// Called by libnewt when a component fires its change/focus callback.
// Sets NEWT_COMPONENT and NEWT_CB_DATA, then evaluates the registered bash
// expression.
static void component_callback_shim(newtComponent co, void* /*data*/) {
    auto it = g_component_callbacks.find(co);
    if (it == g_component_callbacks.end()) return;

    std::string co_str = to_bash_string(co);
    builtin_bind_variable(const_cast<char*>("NEWT_COMPONENT"),
                          const_cast<char*>(co_str.c_str()), 0);
    builtin_bind_variable(const_cast<char*>("NEWT_CB_DATA"),
                          const_cast<char*>(it->second.second.c_str()), 0);

    const std::string& s = it->second.first;
    char* cmd = reinterpret_cast<char*>(xmalloc(s.size() + 1));
    std::memcpy(cmd, s.c_str(), s.size() + 1);
    evalstring(cmd, nullptr, 0);
}

// Called by libnewt when a component is destroyed.  Looks up the bash
// expression registered for 'co' and evaluates it.
static void component_destroy_shim(newtComponent co, void* /*data*/) {
    auto it = g_destroy_callbacks.find(co);
    if (it == g_destroy_callbacks.end()) return;
    const std::string& s = it->second;
    char* cmd = reinterpret_cast<char*>(xmalloc(s.size() + 1));
    std::memcpy(cmd, s.c_str(), s.size() + 1);
    evalstring(cmd, nullptr, 0);
    g_destroy_callbacks.erase(it);
}

// ─── widget constructors ──────────────────────────────────────────────────────

// Entry left top initialValue width [flags]
// resultPtr is always NULL; use EntryGetValue to read the value later.
static int wrap_Entry(char* v, WORD_LIST* a) {
    int left, top, width, flags = 0;
    const char* initialValue;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, left)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, top)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, initialValue)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, width)) goto usage;
    if (a->next) { a = a->next; if (!from_string(a->word->word, flags)) goto usage; }

    {
        newtComponent rv = newtEntry(left, top, initialValue, width, nullptr, flags);
        if (v) {
            std::string s = to_bash_string(rv);
            builtin_bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt Entry left top initialValue width [flags]\n");
    return EXECUTION_FAILURE;
}

// Form [vertBar [helpTag [flags]]]  — all args optional
static int wrap_Form(char* v, WORD_LIST* a) {
    newtComponent vertBar = nullptr;
    void* helpTag = nullptr;
    int flags = 0;

    if (a->next) {
        a = a->next; if (!from_string(a->word->word, vertBar)) goto usage;
        if (a->next) {
            a = a->next; if (!from_string(a->word->word, helpTag)) goto usage;
            if (a->next) {
                a = a->next; if (!from_string(a->word->word, flags)) goto usage;
            }
        }
    }
    {
        newtComponent rv = newtForm(vertBar, helpTag, flags);
        if (v) {
            std::string s = to_bash_string(rv);
            builtin_bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt Form [vertBar [helpTag [flags]]]\n");
    return EXECUTION_FAILURE;
}


// Checkbox left top text [defValue [seq]]
// Allocates a persistent char to hold the checkbox state (required by libnewt).
static int wrap_Checkbox(char* v, WORD_LIST* a) {
    int left, top;
    const char* text;
    char defValue = ' ';
    const char* seq = nullptr;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, left)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, top)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text)) goto usage;
    if (a->next) {
        a = a->next; if (!from_string(a->word->word, defValue)) goto usage;
        if (a->next) { a = a->next; if (!from_string(a->word->word, seq)) goto usage; }
    }
    {
        // Allocate result storage first so we have a stable char* to pass.
        auto storage = std::make_unique<char>(defValue);
        char* result_ptr = storage.get();
        newtComponent rv = newtCheckbox(left, top, text, defValue, seq, result_ptr);
        g_checkbox_results[rv] = std::move(storage);
        if (v) {
            std::string s = to_bash_string(rv);
            builtin_bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt Checkbox left top text [defValue [seq]]\n");
    return EXECUTION_FAILURE;
}

// Radiobutton left top text isDefault prevButton
static int wrap_Radiobutton(char* v, WORD_LIST* a) {
    return call_newt("Radiobutton", "left top text isDefault prevButton",
                     newtRadiobutton, v, a);
}

// Scale left top width fullValue
static int wrap_Scale(char* v, WORD_LIST* a) {
    return call_newt("Scale", "left top width fullValue", newtScale, v, a);
}

// Textbox left top width height flags
static int wrap_Textbox(char* v, WORD_LIST* a) {
    return call_newt("Textbox", "left top width height flags", newtTextbox, v, a);
}

// ─── entry helpers ────────────────────────────────────────────────────────────

// EntrySet co value cursorAtEnd
static int wrap_EntrySet(char* v, WORD_LIST* a) {
    return call_newt("EntrySet", "co value cursorAtEnd", newtEntrySet, v, a);
}

// EntrySetFilter co bashFunctionName
// Registers a bash function as the C-level entry filter shim.
static int wrap_EntrySetFilter(char* /*v*/, WORD_LIST* a) {
    newtComponent co;
    const char* filter_name;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, filter_name)) goto usage;

    g_entry_filters[co] = filter_name;
    newtEntrySetFilter(co, entry_filter_shim, nullptr);
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt EntrySetFilter co bashFunctionName\n");
    return EXECUTION_FAILURE;
}

// ─── form helpers ─────────────────────────────────────────────────────────────

// FormAddComponents form comp1 [comp2 ...]
// Implemented via repeated newtFormAddComponent calls (avoids the variadic API).
static int wrap_FormAddComponents(char* /*v*/, WORD_LIST* a) {
    newtComponent form;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, form)) goto usage;

    while (a->next) {
        a = a->next;
        newtComponent comp;
        if (!from_string(a->word->word, comp)) goto usage;
        newtFormAddComponent(form, comp);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt FormAddComponents form comp1 [comp2 ...]\n");
    return EXECUTION_FAILURE;
}

// ─── wrappers for zero-/one-arg functions ────────────────────────────────────

// wrap_Init: call newtInit() and mark the session as active.
// Both newtInit and newtFinished return int; we bind the result when -v is
// given but always return EXECUTION_SUCCESS so callers can use
//   if newt Init; then ... fi
static int wrap_Init(char* v, WORD_LIST* /*a*/) {
    int rc = newtInit();
    newt_init_guard::set_initialized();
    if (v) {
        std::string s = to_bash_string(rc);
        builtin_bind_variable(v, const_cast<char*>(s.c_str()), 0);
    }
    return EXECUTION_SUCCESS;
}

// wrap_Finished: idempotent — safe to call more than once (e.g. from both an
// explicit call before printing results AND a trap-on-EXIT safety net).
// Only the first call after a successful Init actually invokes newtFinished();
// subsequent calls are silent no-ops.
static int wrap_Finished(char* v, WORD_LIST* /*a*/) {
    if (newt_init_guard::is_initialized()) {
        int rc = newtFinished();
        newt_init_guard::clear_initialized();
        if (v) {
            std::string s = to_bash_string(rc);
            builtin_bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
}
static int wrap_Cls(char* v, WORD_LIST* a)             { return call_newt("Cls",             "",           newtCls,              v, a); }
static int wrap_WaitForKey(char* v, WORD_LIST* a)      { return call_newt("WaitForKey",      "",           newtWaitForKey,       v, a); }
static int wrap_ClearKeyBuffer(char* v, WORD_LIST* a)  { return call_newt("ClearKeyBuffer",  "",           newtClearKeyBuffer,   v, a); }
static int wrap_Refresh(char* v, WORD_LIST* a)         { return call_newt("Refresh",         "",           newtRefresh,          v, a); }
static int wrap_Suspend(char* v, WORD_LIST* a)         { return call_newt("Suspend",         "",           newtSuspend,          v, a); }
static int wrap_Resume(char* v, WORD_LIST* a)          { return call_newt("Resume",          "",           newtResume,           v, a); }
static int wrap_Bell(char* v, WORD_LIST* a)            { return call_newt("Bell",            "",           newtBell,             v, a); }
static int wrap_CursorOff(char* v, WORD_LIST* a)       { return call_newt("CursorOff",       "",           newtCursorOff,        v, a); }
static int wrap_CursorOn(char* v, WORD_LIST* a)        { return call_newt("CursorOn",        "",           newtCursorOn,         v, a); }
static int wrap_PopWindow(char* v, WORD_LIST* a)       { return call_newt("PopWindow",       "",           newtPopWindow,        v, a); }
static int wrap_PopWindowNoRefresh(char* v, WORD_LIST* a) { return call_newt("PopWindowNoRefresh", "",      newtPopWindowNoRefresh, v, a); }
static int wrap_RedrawHelpLine(char* v, WORD_LIST* a)  { return call_newt("RedrawHelpLine",  "",           newtRedrawHelpLine,   v, a); }
static int wrap_PopHelpLine(char* v, WORD_LIST* a)     { return call_newt("PopHelpLine",     "",           newtPopHelpLine,      v, a); }
static int wrap_ResizeScreen(char* v, WORD_LIST* a)    { return call_newt("ResizeScreen",    "redraw",     newtResizeScreen,     v, a); }
static int wrap_Delay(char* v, WORD_LIST* a)           { return call_newt("Delay",           "usecs",      newtDelay,            v, a); }
static int wrap_OpenWindow(char* v, WORD_LIST* a)      { return call_newt("OpenWindow",      "left top width height [title]", newtOpenWindow, v, a); }
static int wrap_CenteredWindow(char* v, WORD_LIST* a)  { return call_newt("CenteredWindow",  "width height [title]",          newtCenteredWindow, v, a); }
static int wrap_PushHelpLine(char* v, WORD_LIST* a)    { return call_newt("PushHelpLine",    "text",       newtPushHelpLine,     v, a); }
static int wrap_DrawRootText(char* v, WORD_LIST* a)    { return call_newt("DrawRootText",    "col row text", newtDrawRootText,   v, a); }
static int wrap_SetColor(char* v, WORD_LIST* a)        { return call_newt("SetColor",        "colorset fg bg", newtSetColor,     v, a); }
static int wrap_SetHelpCallback(char* v, WORD_LIST* a) { return call_newt("SetHelpCallback", "f",          newtSetHelpCallback,  v, a); }
static int wrap_RadioSetCurrent(char* v, WORD_LIST* a) { return call_newt("RadioSetCurrent", "setMember",  newtRadioSetCurrent,  v, a); }
static int wrap_CompactButton(char* v, WORD_LIST* a)   { return call_newt("CompactButton",   "left top text", newtCompactButton, v, a); }
static int wrap_Button(char* v, WORD_LIST* a)          { return call_newt("Button",          "left top text", newtButton,       v, a); }
static int wrap_CheckboxGetValue(char* v, WORD_LIST* a){ return call_newt("CheckboxGetValue", "co",              newtCheckboxGetValue, v, a); }
static int wrap_CheckboxSetValue(char* v, WORD_LIST* a){ return call_newt("CheckboxSetValue", "co value",        newtCheckboxSetValue, v, a); }
static int wrap_CheckboxSetFlags(char* v, WORD_LIST* a){ return call_newt("CheckboxSetFlags", "co flags sense",   newtCheckboxSetFlags, v, a); }
static int wrap_RadioGetCurrent(char* v, WORD_LIST* a) { return call_newt("RadioGetCurrent",  "setMember",        newtRadioGetCurrent,  v, a); }
static int wrap_Label(char* v, WORD_LIST* a)           { return call_newt("Label",           "left top text", newtLabel,        v, a); }
static int wrap_LabelSetText(char* v, WORD_LIST* a)    { return call_newt("LabelSetText",    "co text",    newtLabelSetText,     v, a); }
static int wrap_LabelSetColors(char* v, WORD_LIST* a)  { return call_newt("LabelSetColors",  "co colorset",newtLabelSetColors,   v, a); }
static int wrap_VerticalScrollbar(char* v, WORD_LIST* a) { return call_newt("VerticalScrollbar", "left top height normalColorset thumbColorset", newtVerticalScrollbar, v, a); }
static int wrap_ScrollbarSet(char* v, WORD_LIST* a)    { return call_newt("ScrollbarSet",    "co where total", newtScrollbarSet, v, a); }
static int wrap_ScrollbarSetColors(char* v, WORD_LIST* a) { return call_newt("ScrollbarSetColors", "co normal thumb", newtScrollbarSetColors, v, a); }
static int wrap_Listbox(char* v, WORD_LIST* a)         { return call_newt("Listbox",         "left top height flags", newtListbox, v, a); }
static int wrap_ListboxGetCurrent(char* v, WORD_LIST* a){ return call_newt("ListboxGetCurrent", "co",        newtListboxGetCurrent, v, a); }
static int wrap_ListboxSetCurrent(char* v, WORD_LIST* a){ return call_newt("ListboxSetCurrent", "co num",    newtListboxSetCurrent, v, a); }
static int wrap_ListboxSetWidth(char* v, WORD_LIST* a) { return call_newt("ListboxSetWidth", "co width",   newtListboxSetWidth,   v, a); }
static int wrap_ListboxClear(char* v, WORD_LIST* a)    { return call_newt("ListboxClear",    "co",         newtListboxClear,      v, a); }
static int wrap_ListboxItemCount(char* v, WORD_LIST* a){ return call_newt("ListboxItemCount",     "co",                    newtListboxItemCount,      v, a); }
static int wrap_ListboxSetCurrentByKey(char* v, WORD_LIST* a) { return call_newt("ListboxSetCurrentByKey", "co key",            newtListboxSetCurrentByKey, v, a); }
static int wrap_ListboxSetEntry(char* v, WORD_LIST* a) { return call_newt("ListboxSetEntry",           "co num text",          newtListboxSetEntry,        v, a); }
static int wrap_ListboxSetData(char* v, WORD_LIST* a)  { return call_newt("ListboxSetData",            "co num data",          newtListboxSetData,         v, a); }
static int wrap_ListboxAppendEntry(char* v, WORD_LIST* a) { return call_newt("ListboxAppendEntry",    "co text data",         newtListboxAppendEntry,     v, a); }
static int wrap_ListboxInsertEntry(char* v, WORD_LIST* a) { return call_newt("ListboxInsertEntry",    "co text data key",     newtListboxInsertEntry,     v, a); }
static int wrap_ListboxDeleteEntry(char* v, WORD_LIST* a) { return call_newt("ListboxDeleteEntry",    "co key",               newtListboxDeleteEntry,     v, a); }
static int wrap_ListboxClearSelection(char* v, WORD_LIST* a) { return call_newt("ListboxClearSelection", "co",               newtListboxClearSelection,  v, a); }
static int wrap_ListboxSelectItem(char* v, WORD_LIST* a)   { return call_newt("ListboxSelectItem",  "co key sense",         newtListboxSelectItem,      v, a); }

// ListboxGetEntry co num textVar dataVar
// Calls newtListboxGetEntry and binds the text and data pointer to two shell variables.
static int wrap_ListboxGetEntry(char* /*v*/, WORD_LIST* a) {
    newtComponent co;
    int num;
    const char* text_var;
    const char* data_var;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co))       goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, num))      goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, data_var)) goto usage;
    {
        char* text = nullptr;
        void* data = nullptr;
        newtListboxGetEntry(co, num, &text, &data);
        std::string ts = to_bash_string(text ? text : "");
        std::string ds = to_bash_string(data);
        builtin_bind_variable(const_cast<char*>(text_var), const_cast<char*>(ts.c_str()), 0);
        builtin_bind_variable(const_cast<char*>(data_var), const_cast<char*>(ds.c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt ListboxGetEntry co num textVar dataVar\n");
    return EXECUTION_FAILURE;
}
static int wrap_TextboxGetNumLines(char* v, WORD_LIST* a) { return call_newt("TextboxGetNumLines", "co",   newtTextboxGetNumLines, v, a); }
static int wrap_TextboxSetHeight(char* v, WORD_LIST* a){ return call_newt("TextboxSetHeight", "co height", newtTextboxSetHeight,  v, a); }
static int wrap_TextboxSetText(char* v, WORD_LIST* a)  { return call_newt("TextboxSetText",   "co text",   newtTextboxSetText,   v, a); }
static int wrap_TextboxSetColors(char* v, WORD_LIST* a){ return call_newt("TextboxSetColors", "co normal active", newtTextboxSetColors, v, a); }

// SetColors rootFg rootBg borderFg borderBg windowFg windowBg shadowFg shadowBg
//           titleFg titleBg buttonFg buttonBg actButtonFg actButtonBg
//           checkboxFg checkboxBg actCheckboxFg actCheckboxBg entryFg entryBg
//           labelFg labelBg listboxFg listboxBg actListboxFg actListboxBg
//           textboxFg textboxBg actTextboxFg actTextboxBg helpLineFg helpLineBg
//           rootTextFg rootTextBg emptyScale fullScale
//           disabledEntryFg disabledEntryBg compactButtonFg compactButtonBg
//           actSelListboxFg actSelListboxBg selListboxFg selListboxBg
static int wrap_SetColors(char* /*v*/, WORD_LIST* a) {
    static constexpr int NFIELDS = 44;
    const char* f[NFIELDS];
    for (int i = 0; i < NFIELDS; ++i) {
        if (!a->next) goto usage;
        a = a->next;
        if (!from_string(a->word->word, f[i])) goto usage;
    }
    {
        struct newtColors c;
        c.rootFg           = const_cast<char*>(f[0]);
        c.rootBg           = const_cast<char*>(f[1]);
        c.borderFg         = const_cast<char*>(f[2]);
        c.borderBg         = const_cast<char*>(f[3]);
        c.windowFg         = const_cast<char*>(f[4]);
        c.windowBg         = const_cast<char*>(f[5]);
        c.shadowFg         = const_cast<char*>(f[6]);
        c.shadowBg         = const_cast<char*>(f[7]);
        c.titleFg          = const_cast<char*>(f[8]);
        c.titleBg          = const_cast<char*>(f[9]);
        c.buttonFg         = const_cast<char*>(f[10]);
        c.buttonBg         = const_cast<char*>(f[11]);
        c.actButtonFg      = const_cast<char*>(f[12]);
        c.actButtonBg      = const_cast<char*>(f[13]);
        c.checkboxFg       = const_cast<char*>(f[14]);
        c.checkboxBg       = const_cast<char*>(f[15]);
        c.actCheckboxFg    = const_cast<char*>(f[16]);
        c.actCheckboxBg    = const_cast<char*>(f[17]);
        c.entryFg          = const_cast<char*>(f[18]);
        c.entryBg          = const_cast<char*>(f[19]);
        c.labelFg          = const_cast<char*>(f[20]);
        c.labelBg          = const_cast<char*>(f[21]);
        c.listboxFg        = const_cast<char*>(f[22]);
        c.listboxBg        = const_cast<char*>(f[23]);
        c.actListboxFg     = const_cast<char*>(f[24]);
        c.actListboxBg     = const_cast<char*>(f[25]);
        c.textboxFg        = const_cast<char*>(f[26]);
        c.textboxBg        = const_cast<char*>(f[27]);
        c.actTextboxFg     = const_cast<char*>(f[28]);
        c.actTextboxBg     = const_cast<char*>(f[29]);
        c.helpLineFg       = const_cast<char*>(f[30]);
        c.helpLineBg       = const_cast<char*>(f[31]);
        c.rootTextFg       = const_cast<char*>(f[32]);
        c.rootTextBg       = const_cast<char*>(f[33]);
        c.emptyScale       = const_cast<char*>(f[34]);
        c.fullScale        = const_cast<char*>(f[35]);
        c.disabledEntryFg  = const_cast<char*>(f[36]);
        c.disabledEntryBg  = const_cast<char*>(f[37]);
        c.compactButtonFg  = const_cast<char*>(f[38]);
        c.compactButtonBg  = const_cast<char*>(f[39]);
        c.actSelListboxFg  = const_cast<char*>(f[40]);
        c.actSelListboxBg  = const_cast<char*>(f[41]);
        c.selListboxFg     = const_cast<char*>(f[42]);
        c.selListboxBg     = const_cast<char*>(f[43]);
        newtSetColors(c);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr,
        "newt: usage: newt SetColors "
        "rootFg rootBg borderFg borderBg windowFg windowBg shadowFg shadowBg "
        "titleFg titleBg buttonFg buttonBg actButtonFg actButtonBg "
        "checkboxFg checkboxBg actCheckboxFg actCheckboxBg entryFg entryBg "
        "labelFg labelBg listboxFg listboxBg actListboxFg actListboxBg "
        "textboxFg textboxBg actTextboxFg actTextboxBg helpLineFg helpLineBg "
        "rootTextFg rootTextBg emptyScale fullScale "
        "disabledEntryFg disabledEntryBg compactButtonFg compactButtonBg "
        "actSelListboxFg actSelListboxBg selListboxFg selListboxBg\n");
    return EXECUTION_FAILURE;
}

// SetSuspendCallback bashFunctionName
// Registers a bash function as the C-level suspend callback shim.
static int wrap_SetSuspendCallback(char* /*v*/, WORD_LIST* a) {
    const char* fn_name;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, fn_name)) goto usage;

    g_suspend_callback_fn = fn_name;
    newtSetSuspendCallback(suspend_callback_shim, nullptr);
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt SetSuspendCallback bashFunctionName\n");
    return EXECUTION_FAILURE;
}

// ─── existing wrappers (generated) ───────────────────────────────────────────

static int wrap_FormWatchFd(char* v, WORD_LIST* a) {
    return call_newt("FormWatchFd", "form fd fdFlags", newtFormWatchFd, v, a);
}
// FormRun co reasonVar valueVar
// Calls newtFormRun and reports the exit condition via two shell variables.
//   reasonVar: HOTKEY | COMPONENT | FDREADY | TIMER | ERROR
//   valueVar:  key code (HOTKEY), component ptr (COMPONENT), fd index
//              (FDREADY), or 0 (TIMER / ERROR).
static int wrap_FormRun(char* /*v*/, WORD_LIST* a) {
    newtComponent co;
    const char* reason_var;
    const char* value_var;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, reason_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, value_var)) goto usage;
    {
        struct newtExitStruct es;
        newtFormRun(co, &es);
        const char* reason_str = "ERROR";
        std::string value_str = "0";
        switch (es.reason) {
            case newtExitStruct::NEWT_EXIT_HOTKEY:
                reason_str = "HOTKEY";
                value_str = to_bash_string(es.u.key);
                break;
            case newtExitStruct::NEWT_EXIT_COMPONENT:
                reason_str = "COMPONENT";
                value_str = to_bash_string(es.u.co);
                break;
            case newtExitStruct::NEWT_EXIT_FDREADY:
                reason_str = "FDREADY";
                value_str = to_bash_string(es.u.watch);
                break;
            case newtExitStruct::NEWT_EXIT_TIMER:
                reason_str = "TIMER";
                break;
            case newtExitStruct::NEWT_EXIT_ERROR:
                reason_str = "ERROR";
                break;
        }
        builtin_bind_variable(const_cast<char*>(reason_var),
                      const_cast<char*>(reason_str), 0);
        builtin_bind_variable(const_cast<char*>(value_var),
                      const_cast<char*>(value_str.c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt FormRun form reasonVar valueVar\n");
    return EXECUTION_FAILURE;
}
// ComponentAddCallback co bashExpr [data]
// Registers a bash expression as the component callback.  Before the
// expression is evaluated, NEWT_COMPONENT is set to the component pointer and
// NEWT_CB_DATA is set to the optional data string (or "" if omitted).
static int wrap_ComponentAddCallback(char* /*v*/, WORD_LIST* a) {
    newtComponent co;
    const char* expr;
    const char* data = "";

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, expr)) goto usage;
    if (a->next) {
        a = a->next;
        from_string(a->word->word, data);
    }
    g_component_callbacks[co] = {expr, data};
    newtComponentAddCallback(co, component_callback_shim, nullptr);
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr,
                 "newt: usage: newt ComponentAddCallback co bashExpr [data]\n");
    return EXECUTION_FAILURE;
}
// ComponentGetPosition co leftVar topVar
static int wrap_ComponentGetPosition(char* /*v*/, WORD_LIST* a) {
    newtComponent co;
    const char* left_var;
    const char* top_var;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, left_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, top_var)) goto usage;
    {
        int left = 0, top = 0;
        newtComponentGetPosition(co, &left, &top);
        builtin_bind_variable(const_cast<char*>(left_var),
                      const_cast<char*>(to_bash_string(left).c_str()), 0);
        builtin_bind_variable(const_cast<char*>(top_var),
                      const_cast<char*>(to_bash_string(top).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr,
                 "newt: usage: newt ComponentGetPosition co leftVar topVar\n");
    return EXECUTION_FAILURE;
}
// ComponentGetSize co widthVar heightVar
static int wrap_ComponentGetSize(char* /*v*/, WORD_LIST* a) {
    newtComponent co;
    const char* width_var;
    const char* height_var;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, width_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, height_var)) goto usage;
    {
        int width = 0, height = 0;
        newtComponentGetSize(co, &width, &height);
        builtin_bind_variable(const_cast<char*>(width_var),
                      const_cast<char*>(to_bash_string(width).c_str()), 0);
        builtin_bind_variable(const_cast<char*>(height_var),
                      const_cast<char*>(to_bash_string(height).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr,
                 "newt: usage: newt ComponentGetSize co widthVar heightVar\n");
    return EXECUTION_FAILURE;
}
static int wrap_FormSetTimer(char* v, WORD_LIST* a) {
    return call_newt("FormSetTimer", "form milliseconds", newtFormSetTimer, v, a);
}
static int wrap_FormSetSize(char* v, WORD_LIST* a) {
    return call_newt("FormSetSize", "form", newtFormSetSize, v, a);
}
static int wrap_FormGetCurrent(char* v, WORD_LIST* a) {
    return call_newt("FormGetCurrent", "form", newtFormGetCurrent, v, a);
}
static int wrap_FormSetBackground(char* v, WORD_LIST* a) {
    return call_newt("FormSetBackground", "form color", newtFormSetBackground, v, a);
}
static int wrap_FormSetCurrent(char* v, WORD_LIST* a) {
    return call_newt("FormSetCurrent", "form comp", newtFormSetCurrent, v, a);
}
static int wrap_FormAddComponent(char* v, WORD_LIST* a) {
    return call_newt("FormAddComponent", "form comp", newtFormAddComponent, v, a);
}
static int wrap_FormSetHeight(char* v, WORD_LIST* a) {
    return call_newt("FormSetHeight", "form height", newtFormSetHeight, v, a);
}
static int wrap_FormSetWidth(char* v, WORD_LIST* a) {
    return call_newt("FormSetWidth", "form width", newtFormSetWidth, v, a);
}
static int wrap_RunForm(char* v, WORD_LIST* a) {
    return call_newt("RunForm", "form", newtRunForm, v, a);
}
static int wrap_DrawForm(char* v, WORD_LIST* a) {
    return call_newt("DrawForm", "form", newtDrawForm, v, a);
}
static int wrap_FormAddHotKey(char* v, WORD_LIST* a) {
    return call_newt("FormAddHotKey", "form key", newtFormAddHotKey, v, a);
}
static int wrap_FormGetScrollPosition(char* v, WORD_LIST* a) {
    return call_newt("FormGetScrollPosition", "form", newtFormGetScrollPosition, v, a);
}
static int wrap_FormSetScrollPosition(char* v, WORD_LIST* a) {
    return call_newt("FormSetScrollPosition", "form position",
                     newtFormSetScrollPosition, v, a);
}
static int wrap_FormDestroy(char* v, WORD_LIST* a) {
    return call_newt("FormDestroy", "form", newtFormDestroy, v, a);
}
static int wrap_ComponentDestroy(char* v, WORD_LIST* a) {
    return call_newt("ComponentDestroy", "co", newtComponentDestroy, v, a);
}
static int wrap_ComponentTakesFocus(char* v, WORD_LIST* a) {
    return call_newt("ComponentTakesFocus", "co val",
                     newtComponentTakesFocus, v, a);
}
static int wrap_GridPlace(char* v, WORD_LIST* a) {
    return call_newt("GridPlace", "grid left top", newtGridPlace, v, a);
}
static int wrap_GridFree(char* v, WORD_LIST* a) {
    return call_newt("GridFree", "grid recurse", newtGridFree, v, a);
}
static int wrap_GridBasicWindow(char* v, WORD_LIST* a) {
    return call_newt("GridBasicWindow", "text buttons entryField",
                     newtGridBasicWindow, v, a);
}
static int wrap_GridSimpleWindow(char* v, WORD_LIST* a) {
    return call_newt("GridSimpleWindow", "text buttons entryField",
                     newtGridSimpleWindow, v, a);
}
static int wrap_EntryGetValue(char* v, WORD_LIST* a) {
    return call_newt("EntryGetValue", "co", newtEntryGetValue, v, a);
}
static int wrap_EntrySetFlags(char* v, WORD_LIST* a) {
    return call_newt("EntrySetFlags", "co flags sense", newtEntrySetFlags, v, a);
}
static int wrap_EntrySetColors(char* v, WORD_LIST* a) {
    return call_newt("EntrySetColors", "co normal disabledColor",
                     newtEntrySetColors, v, a);
}
static int wrap_EntryGetCursorPosition(char* v, WORD_LIST* a) {
    return call_newt("EntryGetCursorPosition", "co",
                     newtEntryGetCursorPosition, v, a);
}
static int wrap_EntrySetCursorPosition(char* v, WORD_LIST* a) {
    return call_newt("EntrySetCursorPosition", "co position",
                     newtEntrySetCursorPosition, v, a);
}
static int wrap_ScaleSet(char* v, WORD_LIST* a) {
    return call_newt("ScaleSet", "co amount", newtScaleSet, v, a);
}
static int wrap_ScaleSetColors(char* v, WORD_LIST* a) {
    return call_newt("ScaleSetColors", "co empty full",
                     newtScaleSetColors, v, a);
}

// GetScreenSize colsVar rowsVar
// Calls newtGetScreenSize and binds the results to the named variables.
static int wrap_GetScreenSize(char* /*v*/, WORD_LIST* a) {
    const char* cols_var;
    const char* rows_var;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, cols_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, rows_var)) goto usage;

    {
        int cols = 0, rows = 0;
        newtGetScreenSize(&cols, &rows);
        std::string cols_s = to_bash_string(cols);
        std::string rows_s = to_bash_string(rows);
        builtin_bind_variable(const_cast<char*>(cols_var), const_cast<char*>(cols_s.c_str()), 0);
        builtin_bind_variable(const_cast<char*>(rows_var), const_cast<char*>(rows_s.c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt GetScreenSize colsVar rowsVar\n");
    return EXECUTION_FAILURE;
}

// ComponentAddDestroyCallback co bashExpression
// Registers a bash expression to be evaluated when the component is destroyed.
static int wrap_ComponentAddDestroyCallback(char* /*v*/, WORD_LIST* a) {
    newtComponent co;
    const char* expr;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, expr)) goto usage;

    g_destroy_callbacks[co] = expr;
    newtComponentAddDestroyCallback(co, component_destroy_shim, nullptr);
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt ComponentAddDestroyCallback co bashExpression\n");
    return EXECUTION_FAILURE;
}

// ─── dispatch table ───────────────────────────────────────────────────────────

struct DispatchEntry {
    const char* name;
    WrapperFn   fn;
};

static const DispatchEntry dispatch_table[] = {
    { "Init",                   wrap_Init              },
    { "Finished",               wrap_Finished          },
    { "Cls",                    wrap_Cls               },
    { "WaitForKey",             wrap_WaitForKey        },
    { "ClearKeyBuffer",         wrap_ClearKeyBuffer    },
    { "Refresh",                wrap_Refresh           },
    { "Suspend",                wrap_Suspend           },
    { "Resume",                 wrap_Resume            },
    { "Bell",                   wrap_Bell              },
    { "CursorOff",              wrap_CursorOff         },
    { "CursorOn",               wrap_CursorOn          },
    { "PopWindow",              wrap_PopWindow         },
    { "PopWindowNoRefresh",     wrap_PopWindowNoRefresh},
    { "RedrawHelpLine",         wrap_RedrawHelpLine    },
    { "PopHelpLine",            wrap_PopHelpLine       },
    { "ResizeScreen",           wrap_ResizeScreen      },
    { "Delay",                  wrap_Delay             },
    { "OpenWindow",             wrap_OpenWindow        },
    { "CenteredWindow",         wrap_CenteredWindow    },
    { "PushHelpLine",           wrap_PushHelpLine      },
    { "DrawRootText",           wrap_DrawRootText      },
    { "SetColor",               wrap_SetColor          },
    { "SetHelpCallback",        wrap_SetHelpCallback   },
    { "RadioSetCurrent",        wrap_RadioSetCurrent   },
    { "CompactButton",          wrap_CompactButton     },
    { "Button",                 wrap_Button            },
    { "CheckboxGetValue",       wrap_CheckboxGetValue  },
    { "CheckboxSetValue",       wrap_CheckboxSetValue  },
    { "CheckboxSetFlags",       wrap_CheckboxSetFlags  },
    { "RadioGetCurrent",        wrap_RadioGetCurrent   },
    { "Label",                  wrap_Label             },
    { "LabelSetText",           wrap_LabelSetText      },
    { "LabelSetColors",         wrap_LabelSetColors    },
    { "VerticalScrollbar",      wrap_VerticalScrollbar },
    { "ScrollbarSet",           wrap_ScrollbarSet      },
    { "ScrollbarSetColors",     wrap_ScrollbarSetColors},
    { "Listbox",                wrap_Listbox           },
    { "ListboxGetCurrent",      wrap_ListboxGetCurrent },
    { "ListboxSetCurrent",      wrap_ListboxSetCurrent },
    { "ListboxSetWidth",        wrap_ListboxSetWidth   },
    { "ListboxClear",           wrap_ListboxClear      },
    { "ListboxItemCount",        wrap_ListboxItemCount        },
    { "ListboxSetCurrentByKey",  wrap_ListboxSetCurrentByKey  },
    { "ListboxSetEntry",         wrap_ListboxSetEntry         },
    { "ListboxSetData",          wrap_ListboxSetData          },
    { "ListboxAppendEntry",      wrap_ListboxAppendEntry      },
    { "ListboxAddEntry",         wrap_ListboxAppendEntry      },
    { "ListboxInsertEntry",      wrap_ListboxInsertEntry      },
    { "ListboxDeleteEntry",      wrap_ListboxDeleteEntry      },
    { "ListboxGetEntry",         wrap_ListboxGetEntry         },
    { "ListboxClearSelection",   wrap_ListboxClearSelection   },
    { "ListboxSelectItem",       wrap_ListboxSelectItem       },
    { "TextboxGetNumLines",     wrap_TextboxGetNumLines},
    { "TextboxSetHeight",       wrap_TextboxSetHeight  },
    { "TextboxSetText",         wrap_TextboxSetText    },
    { "TextboxSetColors",       wrap_TextboxSetColors  },
    { "FormSetTimer",           wrap_FormSetTimer      },
    { "FormSetSize",            wrap_FormSetSize       },
    { "FormGetCurrent",         wrap_FormGetCurrent    },
    { "FormSetBackground",      wrap_FormSetBackground },
    { "FormSetCurrent",         wrap_FormSetCurrent    },
    { "FormAddComponent",       wrap_FormAddComponent  },
    { "FormSetHeight",          wrap_FormSetHeight     },
    { "FormSetWidth",           wrap_FormSetWidth      },
    { "FormWatchFd",             wrap_FormWatchFd       },
    { "RunForm",                wrap_RunForm           },
    { "FormRun",                wrap_FormRun           },
    { "DrawForm",               wrap_DrawForm          },
    { "FormAddHotKey",          wrap_FormAddHotKey     },
    { "FormGetScrollPosition",  wrap_FormGetScrollPosition },
    { "FormSetScrollPosition",  wrap_FormSetScrollPosition },
    { "FormDestroy",            wrap_FormDestroy       },
    { "ComponentDestroy",       wrap_ComponentDestroy  },
    { "ComponentTakesFocus",    wrap_ComponentTakesFocus},
    { "ComponentAddCallback",   wrap_ComponentAddCallback},
    { "ComponentGetPosition",   wrap_ComponentGetPosition},
    { "ComponentGetSize",       wrap_ComponentGetSize  },
    { "GridPlace",              wrap_GridPlace         },
    { "GridFree",               wrap_GridFree          },
    { "GridBasicWindow",        wrap_GridBasicWindow   },
    { "GridSimpleWindow",       wrap_GridSimpleWindow  },
    { "EntryGetValue",          wrap_EntryGetValue     },
    { "EntrySetFlags",          wrap_EntrySetFlags     },
    { "EntrySetColors",         wrap_EntrySetColors    },
    { "EntryGetCursorPosition", wrap_EntryGetCursorPosition },
    { "EntrySetCursorPosition", wrap_EntrySetCursorPosition },
    { "ScaleSet",               wrap_ScaleSet          },
    { "ScaleSetColors",         wrap_ScaleSetColors    },    { "GetScreenSize",           wrap_GetScreenSize     },
    { "ComponentAddDestroyCallback", wrap_ComponentAddDestroyCallback },    { "SetColors",              wrap_SetColors         },
    { "SetSuspendCallback",     wrap_SetSuspendCallback},
    // ── constructors ──────────────────────────────────────────────────────────
    { "Entry",                  wrap_Entry             },
    { "Form",                   wrap_Form              },
    { "Checkbox",               wrap_Checkbox          },
    { "Radiobutton",            wrap_Radiobutton       },
    { "Scale",                  wrap_Scale             },
    { "Textbox",                wrap_Textbox           },
    // ── entry helpers ─────────────────────────────────────────────────────────
    { "EntrySet",               wrap_EntrySet          },
    { "EntrySetFilter",         wrap_EntrySetFilter    },
    // ── form helpers ──────────────────────────────────────────────────────────
    { "FormAddComponents",      wrap_FormAddComponents },
};
static constexpr std::size_t dispatch_table_size =
    sizeof(dispatch_table) / sizeof(dispatch_table[0]);

WrapperFn find_command(const char* name) {
    for (std::size_t i = 0; i < dispatch_table_size; ++i)
        if (std::strcmp(dispatch_table[i].name, name) == 0)
            return dispatch_table[i].fn;
    return nullptr;
}
