#include "foo_ui_columns.h"

class export_layout : public cui::fcl::dataset
{
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Layout";
	}
	virtual const GUID & get_guid () const
	{
		// {2CF00365-F2D7-4e78-9FCA-C1D56E2707D8}
		static const GUID guid = 
		{ 0x2cf00365, 0xf2d7, 0x4e78, { 0x9f, 0xca, 0xc1, 0xd5, 0x6e, 0x27, 0x7, 0xd8 } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		pfc::list_t<GUID> panels;
		g_layout_window.export_config(p_writer, type, panels, p_abort);
		feedback.add_required_panels(panels);
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::layout;
	}
	virtual void set_data (stream_reader * p_reader, t_size size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		pfc::list_t<GUID> panels;
		bool missingpanels = false;

		t_uint32 version;
		p_reader->read_lendian_t(version, p_abort);
		if (version > 0) throw pfc::exception("Need new columns ui");
		t_uint32 j, pcount, active;
		p_reader->read_lendian_t(active, p_abort);
		p_reader->read_lendian_t(pcount, p_abort);

		pfc::list_t<cfg_layout_t::preset> presets;

		for (j=0; j<pcount; j++)
		{
			cfg_layout_t::preset pres;
			if (!g_layout_window.import_config_to_object(p_reader, size, type, pres, panels, p_abort))
				missingpanels = true;
			presets.add_item(pres);
		}

		t_size i, count = panels.get_count();
		for (i=0; i<count; i++)
		{
			uie::window_ptr ptr;
			if (!uie::window::create_by_guid(panels[i], ptr))
			{
				missingpanels = true;
				feedback.add_required_panel("", panels[i]);
			}
		}

		if (!missingpanels || type == cui::fcl::type_private)
		{
			//cfg_layout.
			cfg_layout.set_presets(presets, active);
			//feedback.add_warning("Some panels missing, main layout not imported");
		}
		//else console::print("misspan");
	}
};

cui::fcl::dataset_factory<export_layout> g_export_layout_t;

extern cfg_rebar g_cfg_rebar;
class export_toolbars : public cui::fcl::dataset
{
	virtual void get_name (pfc::string_base & p_out)const
	{
		p_out = "Toolbars";
	}
	virtual const GUID & get_guid ()const
	{
		// {2F802663-0BD1-4d3d-AE7E-0663007A9C2B}
		static const GUID guid = 
		{ 0x2f802663, 0xbd1, 0x4d3d, { 0xae, 0x7e, 0x6, 0x63, 0x0, 0x7a, 0x9c, 0x2b } };
		return guid;
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::toolbars;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		g_cfg_rebar.export_config(p_writer, type, feedback, p_abort);
	}
	virtual void set_data (stream_reader * p_reader, t_size size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		pfc::list_t<GUID> panels;
		g_cfg_rebar.import_config(p_reader, size, type, panels, p_abort);
		t_size i, count = panels.get_count();
		for (i=0; i<count; i++)
		{
			feedback.add_required_panel("", panels[i]);
		}
	}
};

cui::fcl::dataset_factory<export_toolbars> g_export_toolbars_t;

class export_layout_misc : public cui::fcl::dataset
{
	enum t_colour_pview_identifiers
	{
		identifier_status,
		identifier_status_pane,
	};
	virtual void get_name (pfc::string_base & p_out) const
	{
		p_out = "Misc layout";
	}
	virtual const GUID & get_group () const
	{
		return cui::fcl::groups::layout;
	}
	virtual const GUID & get_guid () const
	{
		// {78AA8894-4B2C-477d-B233-E7A7A7663D24}
		static const GUID guid = 
		{ 0x78aa8894, 0x4b2c, 0x477d, { 0xb2, 0x33, 0xe7, 0xa7, 0xa7, 0x66, 0x3d, 0x24 } };
		return guid;
	}
	virtual void get_data (stream_writer * p_writer, t_uint32 type, cui::fcl::t_export_feedback & feedback, abort_callback & p_abort) const
	{
		fcl::writer out(p_writer, p_abort);
		out.write_item(identifier_status, cfg_status);
		out.write_item(identifier_status_pane, settings::show_status_pane);
	}
	virtual void set_data (stream_reader * p_reader, t_size stream_size, t_uint32 type, cui::fcl::t_import_feedback & feedback, abort_callback & p_abort)
	{
		fcl::reader reader(p_reader, stream_size, p_abort);
		t_uint32 element_id;
		t_uint32 element_size;

		while (reader.get_remaining())
		{
			reader.read_item(element_id);
			reader.read_item(element_size);

			switch (element_id)
			{
			case identifier_status:
				reader.read_item(cfg_status);
				break;
			case identifier_status_pane:
				reader.read_item(settings::show_status_pane);
				break;
			default:
				reader.skip(element_size);
				break;
			};
		}

		on_show_status_change();
		on_show_status_pane_change();
	}
};
cui::fcl::dataset_factory<export_layout_misc> g_export_layout_misc;