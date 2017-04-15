#include "stdafx.h"
#include "config.h"

DECLARE_COMPONENT_VERSION(
	"Metadata Exporter",
	"0.01",
	"Writes song metadata to a file for use by other applications");

class vstream_play_callback :public play_callback_static {
public:
	virtual unsigned get_flags() {
		return play_callback::flag_on_playback_new_track |
			play_callback::flag_on_playback_edited;
	}

	void export_metadata(metadb_handle_ptr p_track) {
		if (p_track != 0) {
			try {
				file_info *p_info = new file_info_impl();
				wchar_t pVal[255];
				wchar_t expandStr[255];
				char dirloc[255];
				pfc::string filePath;

				pfc::stringcvt::convert_utf8_to_wide(pVal, sizeof pVal, cfg_dirtxtbox, strlen(cfg_dirtxtbox));

				ExpandEnvironmentStrings(pVal, expandStr, 255);

				pfc::stringcvt::convert_wide_to_utf8(dirloc, sizeof(dirloc), expandStr, wcslen(expandStr));
				filePath = pfc::string::g_concatenateRaw(dirloc, strlen(dirloc), "\\meta.txt", strlen("\\meta.txt")); // Need to check if file name is included first. Eventually.

				if (p_track->get_info_async(*p_info)) {
					pfc::string meta = "";

					if (cfg_getartist == BST_CHECKED) {

						meta += "ARTIST=";
						meta += get_metadata_for_field(*p_info, "artist");
					}

					if (cfg_gettitle == BST_CHECKED) {
						if (strcmp(meta.ptr(), "") != 0) meta += "\n";

						meta += "TITLE=";
						meta += get_metadata_for_field(*p_info, "title");
					}

					if (cfg_getalbum == BST_CHECKED) {
						if (strcmp(meta.ptr(), "") != 0) meta += "\n";

						meta += "ALBUM=";
						meta += get_metadata_for_field(*p_info, "album");
					}

					if (cfg_getcomposer == BST_CHECKED) {
						if (strcmp(meta.ptr(), "") != 0) meta += "\n";

						meta += "COMPOSER=";
						meta += get_metadata_for_field(*p_info, "composer");
					}

					if (cfg_getperformer == BST_CHECKED) {
						if (strcmp(meta.ptr(), "") != 0) meta += "\n";

						meta += "PERFORMER=";
						meta += get_metadata_for_field(*p_info, "performer");
					}

					file_ptr *f = new file_ptr();
					abort_callback_dummy *a = new abort_callback_dummy();

					filesystem::g_open_write_new(*f, filePath.ptr(), *a);

					f->get_ptr()->write_string_raw(meta.ptr(), *a);
					f->release();
				}
			}
			catch (const std::exception &ex)
			{
				console::print("[foo_metaexport] Somebody fucked up");
				console::print("[foo_metaexport] Contact Kethsar @ Rizon IRC for help\n");
			}
		}
	}

	pfc::string get_metadata_for_field(file_info &p_info, pfc::string field) {
		pfc::string retval;

		for (unsigned i = 0; i < p_info.meta_get_count(); i++) {
			pfc::string name = p_info.meta_enum_name(i);

			for (unsigned j = 0; j < p_info.meta_enum_value_count(i); j++) {
				if (pfc::string::g_equalsCaseInsensitive(name, field))
					retval = p_info.meta_enum_value(i, j);
			}
		}

		return retval;
	}

	virtual void on_playback_new_track(metadb_handle_ptr p_track) { export_metadata(p_track); }
	virtual void on_playback_edited(metadb_handle_ptr p_track) { export_metadata(p_track); }


	// Needed to consider this class a 'proper' implementation, I think
	virtual void on_playback_pause(bool p_state) {}
	virtual void on_playback_dynamic_info_track(const file_info&p_info) {}

	virtual void on_playback_starting(play_control::t_track_command p_command, bool p_paused) {}
	virtual void on_playback_stop(play_control::t_stop_reason p_reason) {}


	virtual void on_playback_seek(double p_time) {}
	virtual void on_playback_dynamic_info(const file_info & p_info) {}
	virtual void on_playback_time(double p_time) {}
	virtual void on_volume_change(float p_new_val) {}
};

static play_callback_static_factory_t<vstream_play_callback> g_vstream_play_callback_factory;