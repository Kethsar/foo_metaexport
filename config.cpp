#include "config.h"

cfg_string cfg_dirtxtbox(guid_cfg_dirtxtbox, default_pathtext);
cfg_uint cfg_getartist(guid_cfg_artistcheckbox, default_cfg_artist);
cfg_uint cfg_gettitle(guid_cfg_titlecheckbox, default_cfg_title);
cfg_uint cfg_getalbum(guid_cfg_albumcheckbox, default_cfg_album);
cfg_uint cfg_getcomposer(guid_cfg_compcheckbox, default_cfg_composer);
cfg_uint cfg_getperformer(guid_cfg_perfcheckbox, default_cfg_performer);

class CMyPreferences : public CDialogImpl<CMyPreferences>, public preferences_page_instance {
public:
	CMyPreferences(preferences_page_callback::ptr callback) : m_callback(callback) {}

	enum { IDD = IDD_MYPREFERENCES };

	t_uint32 get_state();
	void apply();
	void reset();

	BEGIN_MSG_MAP(CMyPreferences)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDC_ARTIST, BN_CLICKED, OnBnClicked)
		COMMAND_HANDLER_EX(IDC_TITLE, BN_CLICKED, OnBnClicked)
		COMMAND_HANDLER_EX(IDC_ALBUM, BN_CLICKED, OnBnClicked)
		COMMAND_HANDLER_EX(IDC_COMP, BN_CLICKED, OnBnClicked)
		COMMAND_HANDLER_EX(IDC_PERF, BN_CLICKED, OnBnClicked)
		COMMAND_HANDLER_EX(IDC_PATH, EN_CHANGE, OnEditChange)
	END_MSG_MAP()
private:
	BOOL OnInitDialog(CWindow, LPARAM);
	void OnBnClicked(UINT, int, CWindow);
	void OnEditChange(UINT, int, CWindow);
	void UpdatePathPreview();
	bool HasChanged();
	void OnChanged();

	const preferences_page_callback::ptr m_callback;
};

BOOL CMyPreferences::OnInitDialog(CWindow, LPARAM) {
	wchar_t path[255];

	pfc::stringcvt::convert_utf8_to_wide(path, sizeof path, cfg_dirtxtbox, strlen(cfg_dirtxtbox));

	CheckDlgButton(IDC_ARTIST, cfg_getartist);
	CheckDlgButton(IDC_TITLE, cfg_gettitle);
	CheckDlgButton(IDC_ALBUM, cfg_getalbum);
	CheckDlgButton(IDC_COMP, cfg_getcomposer);
	CheckDlgButton(IDC_PERF, cfg_getperformer);
	SetDlgItemText(IDC_PATH, path);
	UpdatePathPreview();
	return FALSE;
}

void CMyPreferences::OnBnClicked(UINT, int, CWindow) {
	OnChanged();
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

	CheckDlgButton(IDC_ARTIST, default_cfg_artist);
	CheckDlgButton(IDC_TITLE, default_cfg_title);
	CheckDlgButton(IDC_ALBUM, default_cfg_album);
	CheckDlgButton(IDC_COMP, default_cfg_composer);
	CheckDlgButton(IDC_PERF, default_cfg_performer);
	SetDlgItemText(IDC_PATH, path);
	OnChanged();
}

void CMyPreferences::apply() {
	wchar_t path[255];
	char convPath[255];
	GetDlgItemText(IDC_PATH, path, 255);
	pfc::stringcvt::convert_wide_to_utf8(convPath, sizeof convPath, path, wcslen(path));

	cfg_getartist = IsDlgButtonChecked(IDC_ARTIST);
	cfg_gettitle = IsDlgButtonChecked(IDC_TITLE);
	cfg_getalbum = IsDlgButtonChecked(IDC_ALBUM);
	cfg_getcomposer = IsDlgButtonChecked(IDC_COMP);
	cfg_getperformer = IsDlgButtonChecked(IDC_PERF);
	cfg_dirtxtbox = convPath;

	OnChanged();
}

bool CMyPreferences::HasChanged() {
	wchar_t path[255];
	char convPath[255];
	GetDlgItemText(IDC_PATH, path, 255);
	pfc::stringcvt::convert_wide_to_utf8(convPath, sizeof convPath, path, wcslen(path));

	return IsDlgButtonChecked(IDC_ARTIST) != cfg_getartist ||
		IsDlgButtonChecked(IDC_TITLE) != cfg_gettitle ||
		IsDlgButtonChecked(IDC_ALBUM) != cfg_getalbum ||
		IsDlgButtonChecked(IDC_COMP) != cfg_getcomposer ||
		IsDlgButtonChecked(IDC_PERF) != cfg_getperformer ||
		strcmp(cfg_dirtxtbox, convPath) != 0;
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