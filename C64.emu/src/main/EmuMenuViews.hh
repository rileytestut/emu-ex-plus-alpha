#pragma once
#include <OptionView.hh>

static void setTrueDriveEmu(bool on)
{
	optionDriveTrueEmulation = on;
	if(c64IsInit)
	{
		resources_set_int("DriveTrueEmulation", on);
	}
}

class SystemOptionView : public OptionView
{
	BoolMenuItem trueDriveEmu
	{
		"True Drive Emulation (TDE)",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			setTrueDriveEmu(item.on);
		}
	};

	BoolMenuItem autostartWarp
	{
		"Autostart Fast-forward",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			resources_set_int("AutostartWarp", item.on);
		}
	};

	BoolMenuItem autostartTDE
	{
		"Autostart Handles TDE",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			resources_set_int("AutostartHandleTrueDriveEmulation", item.on);
		}
	};

	BoolMenuItem cropNormalBorders
	{
		"Crop Normal Borders",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			optionCropNormalBorders = item.on;
			c64VidActiveX = 0; // force pixmap to update on next frame
		}
	};

	MultiChoiceSelectMenuItem c64Model
	{
		"C64 Model",
		[](MultiChoiceMenuItem &, int val)
		{
			setC64Model(val);
		}
	};

	void c64ModelInit()
	{
		static const char *str[] =
		{
			"C64 PAL",
			"C64C PAL",
			"C64 old PAL",
			"C64 NTSC",
			"C64C NTSC",
			"C64 old NTSC",
			"Drean"
		};
		auto model = c64model_get();
		if(model >= (int)sizeofArray(str))
		{
			model = 0;
		}
		c64Model.init(str, model, sizeofArray(str));
	}

	MultiChoiceSelectMenuItem borderMode
	{
		"Border Mode",
		[](MultiChoiceMenuItem &, int val)
		{
			resources_set_int("VICIIBorderMode", val);
		}
	};

	void borderModeInit()
	{
		static const char *str[] =
		{
			"Normal",
			"Full",
			"Debug",
			"None"
		};
		auto mode = intResource("VICIIBorderMode");
		if(mode >= (int)sizeofArray(str))
		{
			mode = VICII_NORMAL_BORDERS;
		}
		borderMode.init(str, mode, sizeofArray(str));
	}

	MultiChoiceSelectMenuItem sidEngine
	{
		"SID Engine",
		[this](MultiChoiceMenuItem &, int val)
		{
			assert(val <= (int)sizeofArray(sidEngineChoiceMap));
			logMsg("setting SID engine: %d", sidEngineChoiceMap[val]);
			resources_set_int("SidEngine", sidEngineChoiceMap[val]);
		}
	};

	template <size_t S>
	static void printSysPathMenuEntryStr(char (&str)[S])
	{
		FsSys::cPath basenameTemp;
		string_printf(str, "System File Path: %s", strlen(firmwareBasePath) ? string_basename(firmwareBasePath, basenameTemp) : "Default");
	}

	FirmwarePathSelector systemFileSelector;
	char systemFilePathStr[256] {0};
	TextMenuItem systemFilePath
	{
		"",
		[this](TextMenuItem &, const Input::Event &e)
		{
			systemFileSelector.init("System File Path", !e.isPointer());
			systemFileSelector.onPathChange =
				[this](const char *newPath)
				{
					printSysPathMenuEntryStr(systemFilePathStr);
					systemFilePath.compile();
					setupSysFilePaths(sysFilePath[0], firmwareBasePath);
					if(!strlen(newPath))
					{
						if(Config::envIsLinux && !Config::MACHINE_IS_PANDORA)
							popup.printf(5, false, "Using default paths:\n%s\n%s\n%s", Base::appPath, "~/.local/share/C64.emu", "/usr/share/games/vice");
						else
							popup.printf(4, false, "Using default path:\n%s/C64.emu", Base::storagePath());
					}
				};
			postDraw();
		}
	};

public:
	static constexpr int sidEngineChoiceMap[]
	{
		SID_ENGINE_FASTSID,
		#if defined(HAVE_RESID)
		SID_ENGINE_RESID,
		#endif
	};

private:
	void sidEngineInit()
	{
		static const char *str[] =
		{
			"FastSID",
			#if defined(HAVE_RESID)
			"ReSID",
			#endif
		};
		auto engine = intResource("SidEngine");
		logMsg("current SID engine: %d", engine);
		uint idx = 0;
		forEachInArray(sidEngineChoiceMap, e)
		{
			if(*e == engine)
			{
				idx = e_i;
				break;
			}
		}
		sidEngine.init(str, idx, sizeofArray(str));
	}

public:
	SystemOptionView(Base::Window &win): OptionView(win) {}

	void loadVideoItems(MenuItem *item[], uint &items)
	{
		OptionView::loadVideoItems(item, items);
		cropNormalBorders.init(optionCropNormalBorders); item[items++] = &cropNormalBorders;
		borderModeInit(); item[items++] = &borderMode;
	}

	void loadAudioItems(MenuItem *item[], uint &items)
	{
		OptionView::loadAudioItems(item, items);
		#ifdef HAVE_RESID
		sidEngineInit(); item[items++] = &sidEngine;
		#endif
	}

	void loadSystemItems(MenuItem *item[], uint &items)
	{
		OptionView::loadSystemItems(item, items);
		c64ModelInit(); item[items++] = &c64Model;
		trueDriveEmu.init(optionDriveTrueEmulation); item[items++] = &trueDriveEmu;
		autostartTDE.init(intResource("AutostartHandleTrueDriveEmulation")); item[items++] = &autostartTDE;
		autostartWarp.init(intResource("AutostartWarp")); item[items++] = &autostartWarp;
		printSysPathMenuEntryStr(systemFilePathStr);
		systemFilePath.init(systemFilePathStr, true); item[items++] = &systemFilePath;
	}
};

constexpr int SystemOptionView::sidEngineChoiceMap[];

static const char *insertEjectMenuStr[] { "Insert File", "Eject" };
static int c64DiskExtensionFsFilter(const char *name, int type);
static int c64TapeExtensionFsFilter(const char *name, int type);
static int c64CartExtensionFsFilter(const char *name, int type);
extern int mem_cartridge_type;

class C64IOControlView : public BaseMenuView
{
private:

	char tapeSlotStr[1024] {0};

	void updateTapeText()
	{
		auto name = tape_get_file_name();
		FsSys::cPath basenameTemp;
		string_printf(tapeSlotStr, "Tape: %s", name ? string_basename(name, basenameTemp) : "");
	}

public:
	void onTapeMediaChange(const char *name)
	{
		updateTapeText();
		tapeSlot.compile();
	}

	void addTapeFilePickerView(const Input::Event &e)
	{
		auto &fPicker = *allocModalView<EmuFilePicker>(window());
		fPicker.init(!e.isPointer(), false, c64TapeExtensionFsFilter, 1);
		fPicker.onSelectFile() =
			[this](FSPicker &picker, const char* name, const Input::Event &e)
			{
				if(tape_image_attach(1, name) == 0)
				{
					onTapeMediaChange(name);
				}
				picker.dismiss();
			};
		modalViewController.pushAndShow(fPicker);
	}

private:
	TextMenuItem tapeSlot
	{
		[this](TextMenuItem &item, const Input::Event &e)
		{
			if(!item.active) return;
			if(tape_get_file_name() && strlen(tape_get_file_name()))
			{
				auto &multiChoiceView = *menuAllocator.allocNew<MultiChoiceView>("Tape Drive", window());
				multiChoiceView.init(insertEjectMenuStr, sizeofArray(insertEjectMenuStr), !e.isPointer());
				multiChoiceView.onSelect() =
					[this](int action, const Input::Event &e)
					{
						if(action == 0)
						{
							viewStack.popAndShow();
							addTapeFilePickerView(e);
							window().postDraw();
						}
						else
						{
							tape_image_detach(1);
							onTapeMediaChange("");
							viewStack.popAndShow();
						}
						return 0;
					};
				viewStack.pushAndShow(multiChoiceView, &menuAllocator);
			}
			else
			{
				addTapeFilePickerView(e);
			}
			window().postDraw();
		}
	};

	char romSlotStr[1024] {0};

	void updateROMText()
	{
		auto name = cartridge_get_file_name(mem_cartridge_type);
		FsSys::cPath basenameTemp;
		string_printf(romSlotStr, "ROM: %s", name ? string_basename(name, basenameTemp) : "");
	}

public:
	void onROMMediaChange(const char *name)
	{
		updateROMText();
		romSlot.compile();
	}

	void addCartFilePickerView(const Input::Event &e)
	{
		auto &fPicker = *allocModalView<EmuFilePicker>(window());
		fPicker.init(!e.isPointer(), false, c64CartExtensionFsFilter, 1);
		fPicker.onSelectFile() =
			[this](FSPicker &picker, const char* name, const Input::Event &e)
			{
				if(cartridge_attach_image(CARTRIDGE_CRT, name) == 0)
				{
					onROMMediaChange(name);
				}
				picker.dismiss();
			};
		modalViewController.pushAndShow(fPicker);
	}

private:
	TextMenuItem romSlot
	{
		[this](TextMenuItem &, const Input::Event &e)
		{
			if(cartridge_get_file_name(mem_cartridge_type) && strlen(cartridge_get_file_name(mem_cartridge_type)))
			{
				auto &multiChoiceView = *menuAllocator.allocNew<MultiChoiceView>("Cartridge Slot", window());
				multiChoiceView.init(insertEjectMenuStr, sizeofArray(insertEjectMenuStr), !e.isPointer());
				multiChoiceView.onSelect() =
					[this](int action, const Input::Event &e)
					{
						if(action == 0)
						{
							viewStack.popAndShow();
							addCartFilePickerView(e);
							window().postDraw();
						}
						else if(action == 1)
						{
							cartridge_detach_image(-1);
							onROMMediaChange("");
							viewStack.popAndShow();
						}
						return 0;
					};
				viewStack.pushAndShow(multiChoiceView, &menuAllocator);
			}
			else
			{
				addCartFilePickerView(e);
			}
			window().postDraw();
		}
	};

	static constexpr const char *diskSlotPrefix[2] {"Disk #8:", "Disk #9:"};
	char diskSlotStr[2][1024] { {0} };

	void updateDiskText(int slot)
	{
		auto name = file_system_get_disk_name(slot+8);
		FsSys::cPath basenameTemp;
		string_printf(diskSlotStr[slot], "%s %s", diskSlotPrefix[slot], name ? string_basename(name, basenameTemp) : "");
	}

	void onDiskMediaChange(const char *name, int slot)
	{
		updateDiskText(slot);
		diskSlot[slot].compile();
	}

	void addDiskFilePickerView(const Input::Event &e, uint8 slot)
	{
		auto &fPicker = *allocModalView<EmuFilePicker>(window());
		fPicker.init(!e.isPointer(), false, c64DiskExtensionFsFilter, 1);
		fPicker.onSelectFile() =
			[this, slot](FSPicker &picker, const char* name, const Input::Event &e)
			{
				logMsg("inserting disk in unit %d", slot+8);
				if(file_system_attach_disk(slot+8, name) == 0)
				{
					onDiskMediaChange(name, slot);
				}
				picker.dismiss();
			};
		modalViewController.pushAndShow(fPicker);
	}

public:
	void onSelectDisk(const Input::Event &e, uint8 slot)
	{
		if(file_system_get_disk_name(slot+8) && strlen(file_system_get_disk_name(slot+8)))
		{
			auto &multiChoiceView = *menuAllocator.allocNew<MultiChoiceView>("Disk Drive", window());
			multiChoiceView.init(insertEjectMenuStr, sizeofArray(insertEjectMenuStr), !e.isPointer());
			multiChoiceView.onSelect() =
				[this, slot](int action, const Input::Event &e)
				{
					if(action == 0)
					{
						viewStack.popAndShow();
						addDiskFilePickerView(e, slot);
						window().postDraw();
					}
					else
					{
						file_system_detach_disk(slot+8);
						onDiskMediaChange("", slot);
						viewStack.popAndShow();
					}
					return 0;
				};
			viewStack.pushAndShow(multiChoiceView, &menuAllocator);
		}
		else
		{
			addDiskFilePickerView(e, slot);
		}
		window().postDraw();
	}

private:
	TextMenuItem diskSlot[2]
	{
		{[this](TextMenuItem &, const Input::Event &e) { onSelectDisk(e, 0); }},
		{[this](TextMenuItem &, const Input::Event &e) { onSelectDisk(e, 1); }},
	};

	MenuItem *item[9] {nullptr};
public:
	C64IOControlView(Base::Window &win): BaseMenuView("IO Control", win) { }

	void init(bool highlightFirst)
	{
		uint i = 0;
		updateROMText();
		romSlot.init(romSlotStr); item[i++] = &romSlot;

		iterateTimes(1, slot)
		{
			updateDiskText(slot);
			diskSlot[slot].init(diskSlotStr[slot]); item[i++] = &diskSlot[slot];
		}

		updateTapeText();
		tapeSlot.init(tapeSlotStr); item[i++] = &tapeSlot;
		assert(i <= sizeofArray(item));
		BaseMenuView::init(item, i, highlightFirst);
	}
};

constexpr const char *C64IOControlView::diskSlotPrefix[2];

class SystemMenuView : public MenuView
{
	BoolMenuItem swapJoystickPorts
	{
		"Swap Joystick Ports",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			optionSwapJoystickPorts = item.on;
		}
	};

	TextMenuItem c64IOControl
	{
		"ROM/Disk/Tape Control",
		[this](TextMenuItem &item, const Input::Event &e)
		{
			if(item.active)
			{
				FsSys::chdir(EmuSystem::gamePath());// Stay in active media's directory
				auto &c64IoMenu = *menuAllocator.allocNew<C64IOControlView>(window());
				c64IoMenu.init(!e.isPointer());
				viewStack.pushAndShow(c64IoMenu, &menuAllocator);
			}
		}
	};

	TextMenuItem quickSettings
	{
		"Apply Quick C64 Settings",
		[this](TextMenuItem &item, const Input::Event &e)
		{
			static const char *str[] =
			{
				"1. NTSC & True Drive Emu",
				"2. NTSC",
				"3. PAL & True Drive Emu",
				"4. PAL",
			};
			auto &multiChoiceView = *menuAllocator.allocNew<MultiChoiceView>(item.t.str, window());
			multiChoiceView.init(str, sizeofArray(str), !e.isPointer(), LC2DO);
			multiChoiceView.onSelect() =
				[](int action, const Input::Event &e)
				{
					viewStack.popAndShow();
					switch(action)
					{
						bcase 0:
							setTrueDriveEmu(1);
							setC64Model(C64MODEL_C64_NTSC);
						bcase 1:
							setTrueDriveEmu(0);
							setC64Model(C64MODEL_C64_NTSC);
						bcase 2:
							setTrueDriveEmu(1);
							setC64Model(C64MODEL_C64_PAL);
						bcase 3:
							setTrueDriveEmu(0);
							setC64Model(C64MODEL_C64_PAL);
					}
					if(EmuSystem::gameIsRunning())
					{
						FsSys::cPath gamePath;
						string_copy(gamePath, EmuSystem::fullGamePath());
						EmuSystem::loadGame(gamePath);
						startGameFromMenu();
					}
					return 0;
				};
			viewStack.pushAndShow(multiChoiceView, &menuAllocator);
		}
	};

public:
	SystemMenuView(Base::Window &win): MenuView(win) { }

	void onShow()
	{
		MenuView::onShow();
		c64IOControl.active = EmuSystem::gameIsRunning();
		swapJoystickPorts.on = optionSwapJoystickPorts;
	}

	void init(bool highlightFirst)
	{
		uint items = 0;
		loadFileBrowserItems(item, items);
		c64IOControl.init(); item[items++] = &c64IOControl;
		quickSettings.init(); item[items++] = &quickSettings;
		swapJoystickPorts.init(optionSwapJoystickPorts); item[items++] = &swapJoystickPorts;
		loadStandardItems(item, items);
		assert(items <= sizeofArray(item));
		BaseMenuView::init(item, items, highlightFirst);
	}
};
