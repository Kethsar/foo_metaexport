#include "config.h"

cfg_string cfg_dirtxtbox(guid_cfg_dirtxtbox, default_pathtext);

class CMyPreferences : public CDialogImpl<CMyPreferences>, public preferences_page_instance {
public:
	CMyPreferences(preferences_page_callback::ptr callback) : m_callback(callback) {}

	enum { IDD = IDD_MYPREFERENCES };

	t_uint32 get_state();
	void apply();
	void reset();

	BEGIN_MSG_MAP(CMyPreferences)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_PATH, EN_CHANGE, OnEditChange)
	END_MSG_MAP()
private:
	BOOL OnInitDialog(CWindow, LPARAM);
	void OnEditChange(UINT, int, CWindow);
	void UpdatePathPreview();
	bool HasChanged();
	void OnChanged();

	const preferences_page_callback::ptr m_callback;
};

BOOL CMyPreferences::OnInitDialog(CWindow, LPARAM) {
	wchar_t path[255];

	pfc::stringcvt::convert_utf8_to_wide(path, sizeof path, cfg_dirtxtbox, strlen(cfg_dirtxtbox));

	SetDlgItemText(IDC_PATH, path);
	UpdatePathPreview();
	return FALSE;
}

void CMyPreferences::OnEditChange(UINT, int, CWindow) {
	UpdatePathPreview();
	OnChanged();
}

void CMyPreferences::UpdatePathPreview() {
	wchar_t path[255];
	wchar_t expandedPath[255];

	GetDlgItemText(IDC_PATH, path, 255);
	ExpandEnvironmentStrings(path, expandedPath, 255);

	SetDlgItemText(IDC_PATH_PREVIEW, expandedPath);
}

t_uint32 CMyPreferences::get_state() {
	t_uint32 state = preferences_state::resettable;
	if (HasChanged()) state |= preferences_state::changed;
	return state;
}

void CMyPreferences::reset() {
	wchar_t path[255];

	pfc::stringcvt::convert_utf8_to_wide(path, sizeof path, default_pathtext, strlen(default_pathtext));

	SetDlgItemText(IDC_PATH, path);
	OnChanged();
}

void CMyPreferences::apply() {
	wchar_t path[255];
	char convPath[255];
	GetDlgItemText(IDC_PATH, path, 255);
	pfc::stringcvt::convert_wide_to_utf8(convPath, sizeof convPath, path, wcslen(path));

	cfg_dirtxtbox = convPath;

	OnChanged();
}

bool CMyPreferences::HasChanged() {
	wchar_t path[255];
	char convPath[255];
	GetDlgItemText(IDC_PATH, path, 255);
	pfc::stringcvt::convert_wide_to_utf8(convPath, sizeof convPath, path, wcslen(path));

	return strcmp(cfg_dirtxtbox, convPath) != 0;
}

void CMyPreferences::OnChanged() {
	m_callback->on_state_changed();
}

class preferences_page_myimpl : public preferences_page_impl<CMyPreferences> {
public:
	const char * get_name() { return "Metadata Export"; }
	GUID get_guid() {
		static const GUID guid = { 0x8015c90b, 0xa8a4, 0x4741,{ 0x93, 0x38, 0x95, 0x71, 0x4b, 0xf5, 0x57, 0x71 } };
		return guid;
	}

	GUID get_parent_guid() { return guid_tools; }
};

static preferences_page_factory_t<preferences_page_myimpl> g_preferences_page_myimpl_factory;