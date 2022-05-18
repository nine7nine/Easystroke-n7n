/*
 * Copyright (c) 2008-2009, Thomas Jaeger <ThJaeger@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "prefdb.h"
#include "main.h"
#include "win.h"
#include <glibmm/i18n.h>

#include <fstream>
#include <iostream>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/export.hpp>

const ButtonInfo default_button(Button2);

PrefDB::PrefDB() :
	TimeoutWatcher(5000),
	good_state(true),
	button(default_button),
	advanced_ignore(false),
	proximity(false),
	feedback(true),
	left_handed(false),
	init_timeout(250),
	final_timeout(250),
	timeout_profile(TimeoutDefault),
	timeout_gestures(false),
	tray_icon(false),
	color(Gdk::Color("#980101")),
	trace_width(3),
	advanced_popups(true),
	scroll_invert(true),
	scroll_speed(2.0),
	tray_feedback(false),
	show_osd(true),
	move_back(false),
	whitelist(false)
{}

template<class Archive> void PrefDB::serialize(Archive & ar, const unsigned int version) {
	if (version < 11) {
		std::set<std::string> old;
		ar & boost::serialization::make_nvp("exceptions", old);
		for (std::set<std::string>::iterator i = old.begin(); i != old.end(); i++)
			exceptions.unsafe_ref()[*i] = RButtonInfo();
	} else ar & boost::serialization::make_nvp("exceptions", exceptions.unsafe_ref());
	if (version < 14) {
		double p = 0.5;
		ar & boost::serialization::make_nvp("p", p);
	}
	ar & boost::serialization::make_nvp("button", button.unsafe_ref());
	if (version < 2) {
		bool help;
		ar & boost::serialization::make_nvp("help", help);
	}
	if (version < 3) {
		int delay;
		ar & boost::serialization::make_nvp("delay", delay);
	}
	if (version == 1) {
		ButtonInfo foo;
		ar & boost::serialization::make_nvp("foo", foo);
		ar & boost::serialization::make_nvp("foo", foo);
		return;
	}
	if (version < 2) return;
	if (version != 6)
		ar & boost::serialization::make_nvp("advanced_ignore", advanced_ignore.unsafe_ref());
	int radius = 16;
	ar & boost::serialization::make_nvp("radius", radius);
	if (version < 4) return;
	bool ignore_grab = false;
	ar & boost::serialization::make_nvp("ignore_grab", ignore_grab);
	bool timing_workaround = false;
	ar & boost::serialization::make_nvp("timing_workaround", timing_workaround);
	bool show_clicks = false;
	ar & boost::serialization::make_nvp("show_clicks", show_clicks);
	bool pressure_abort = false;
	ar & boost::serialization::make_nvp("pressure_abort", pressure_abort);
	int pressure_threshold = 192;
	ar & boost::serialization::make_nvp("pressure_threshold", pressure_threshold);
	ar & boost::serialization::make_nvp("proximity", proximity.unsafe_ref());
	if (version < 5) return;
	ar & boost::serialization::make_nvp("feedback", feedback.unsafe_ref());
	ar & boost::serialization::make_nvp("left_handed", left_handed.unsafe_ref());
	ar & boost::serialization::make_nvp("init_timeout", init_timeout.unsafe_ref());
	ar & boost::serialization::make_nvp("final_timeout", final_timeout.unsafe_ref());
	if (version < 8) return;
	ar & boost::serialization::make_nvp("timeout_profile", timeout_profile.unsafe_ref());
	if (version < 9) return;
	ar & boost::serialization::make_nvp("timeout_gestures", timeout_gestures.unsafe_ref());
	ar & boost::serialization::make_nvp("tray_icon", tray_icon.unsafe_ref());
	if (version < 10) return;
	ar & boost::serialization::make_nvp("excluded_devices", excluded_devices.unsafe_ref());
	if (version < 12) {
		unsigned long c = 0;
		ar & boost::serialization::make_nvp("color", c);
		color.unsafe_ref().color.set_rgb(257*(c >> 16), 257*((c >> 8) % 256), 257*(c % 256));
		return;
	} else {
		ar & boost::serialization::make_nvp("color", color.unsafe_ref());
	}
	ar & boost::serialization::make_nvp("trace_width", trace_width.unsafe_ref());
	if (version < 13) return;
	ar & boost::serialization::make_nvp("extra_buttons", extra_buttons.unsafe_ref());
	ar & boost::serialization::make_nvp("advanced_popups", advanced_popups.unsafe_ref());
	ar & boost::serialization::make_nvp("scroll_invert", scroll_invert.unsafe_ref());
	ar & boost::serialization::make_nvp("scroll_speed", scroll_speed.unsafe_ref());
	ar & boost::serialization::make_nvp("tray_feedback", tray_feedback.unsafe_ref());
	ar & boost::serialization::make_nvp("show_osd", show_osd.unsafe_ref());
	if (version < 16) return;
	ar & boost::serialization::make_nvp("move_back", move_back.unsafe_ref());
	if (version < 17) return;
	ar & boost::serialization::make_nvp("device_timeout", device_timeout.unsafe_ref());
	if (version < 18) return;
	ar & boost::serialization::make_nvp("whitelist", whitelist.unsafe_ref());
}

void PrefDB::timeout() {
	std::string filename = config_dir+"preferences"+prefs_versions[0];
	std::string tmp = filename + ".tmp";
	try {
		std::ofstream ofs(tmp.c_str());
		{
			boost::archive::xml_oarchive oa(ofs);
			oa << boost::serialization::make_nvp("PrefDB", *this);
		}
		ofs.close();
		if (rename(tmp.c_str(), filename.c_str()))
			throw std::runtime_error("rename() failed");
		g_debug("Saved preferences.\n");
	} catch (std::exception &e) {
		g_warning("Error: Couldn't save preferences: %s.\n", e.what());
		if (!good_state)
			return;
		good_state = false;
		error_dialog(Glib::ustring::compose(_( "Couldn't save %1.  Your changes will be lost.  "
				"Make sure that \"%2\" is a directory and that you have write access to it.  "
				"You can change the configuration directory "
				"using the -c or --config-dir command line options."), _("preferences"), config_dir));
	}
}


bool ButtonInfo::overlap(const ButtonInfo &bi) const {
	if (button != bi.button)
		return false;
	if (state == AnyModifier || bi.state == AnyModifier)
		return true;
	return !((state ^ bi.state) & ~LockMask & ~Mod2Mask);
}

#include <boost/algorithm/string/predicate.hpp>

void PrefDB::init() {
	std::string filename = config_dir+"preferences";
	for (const char **v = prefs_versions; *v; v++) {
		if (is_file(filename + *v)) {
			filename += *v;
			try {
				std::ifstream ifs(filename.c_str(), std::ios::binary);
				if (!ifs.fail()) {
					if(boost::algorithm::ends_with(filename, ".xml")) {
						boost::archive::xml_iarchive ia(ifs);
						ia >> boost::serialization::make_nvp("PrefDB", *this);
					}
					else {
						boost::archive::text_iarchive ia(ifs);
						ia >> boost::serialization::make_nvp("PrefDB", *this);
					}
					
				 g_debug("Loaded preferences.");
				}
			} catch (...) {
				g_warning("Error: Couldn't read preferences.\n");
			}
			break;
		}
	}
	std::map<std::string, RButtonInfo>::iterator i = exceptions.unsafe_ref().find("(window manager frame)");
	if (i != exceptions.unsafe_ref().end()) {
		RButtonInfo bi = i->second;
		exceptions.unsafe_ref().erase(i);
		exceptions.unsafe_ref()[""] = bi;
	}
}

PrefDB prefs;
