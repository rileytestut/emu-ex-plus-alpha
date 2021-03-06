#include "EmuMenuViews.hh"

class SystemOptionView : public OptionView
{
	MultiChoiceSelectMenuItem tvPhosphor
	{
		"Simulate TV Phosphor",
		[](MultiChoiceMenuItem &, int val)
		{
			optionTVPhosphor.val = val;
			if(!EmuSystem::gameIsRunning())
			{
				return;
			}

			// change runtime phosphor value
			bool usePhosphor = false;
			if((int)optionTVPhosphor == TV_PHOSPHOR_AUTO)
			{
				usePhosphor = currGameProps.get(Display_Phosphor) == "YES";
			}
			else
			{
				usePhosphor = optionTVPhosphor;
			}
			//console->props.set(Display_Phosphor, usePhosphor ? "YES" : "NO");
			//osystem->frameBuffer().enablePhosphor(usePhosphor, atoi(myProperties.get(Display_PPBlend).c_str()));
			bool phospherInUse = console->properties().get(Display_Phosphor) == "YES";
			logMsg("Phosphor effect %s", usePhosphor ? "on" : "off");
			if(usePhosphor != phospherInUse)
			{
				logMsg("toggling phoshpor on console");
				console->togglePhosphor();
			}
		}
	};

	void tvPhosphorInit()
	{
		static const char *str[] =
		{
			"Off", "On", "Auto"
		};
		if(optionTVPhosphor > 2)
			optionTVPhosphor = 2;
		tvPhosphor.init(str, int(optionTVPhosphor), sizeofArray(str));
	}

public:
	SystemOptionView(Base::Window &win): OptionView(win) {}

	void loadVideoItems(MenuItem *item[], uint &items)
	{
		OptionView::loadVideoItems(item, items);
		tvPhosphorInit(); item[items++] = &tvPhosphor;
	}
};

#include "MenuView.hh"

class VCSSwitchesView : public BaseMenuView
{
	MenuItem *item[4] {nullptr};

	TextMenuItem softReset
	{
		"Soft Reset",
		[this](TextMenuItem &, const Input::Event &e)
		{
			if(EmuSystem::gameIsRunning())
			{
				auto &ynAlertView = *allocModalView<YesNoAlertView>(window());
				ynAlertView.init("Really Soft Reset Game?", !e.isPointer());
				ynAlertView.onYes() =
					[](const Input::Event &e)
					{
						Event &ev = osystem.eventHandler().event();
						ev.clear();
						ev.set(Event::ConsoleReset, 1);
						console->switches().update();
						TIA& tia = console->tia();
						tia.update();
						ev.set(Event::ConsoleReset, 0);
						startGameFromMenu();
					};
				modalViewController.pushAndShow(ynAlertView);
			}
		}
	};

	BoolMenuItem diff1
	{
		"Left (P1) Difficulty",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			p1DiffB = item.on;
		}
	};

	BoolMenuItem diff2
	{
		"Right (P2) Difficulty",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			p2DiffB = item.on;
		}
	};

	BoolMenuItem color
	{
		"Color",
		[this](BoolMenuItem &item, const Input::Event &e)
		{
			item.toggle(*this);
			vcsColor = item.on;
		}
	};

public:
	VCSSwitchesView(Base::Window &win): BaseMenuView("Switches", win) {}

	void init(bool highlightFirst)
	{
		uint i = 0;
		diff1.init("A", "B", p1DiffB); item[i++] = &diff1;
		diff2.init("A", "B", p2DiffB); item[i++] = &diff2;
		color.init(vcsColor); item[i++] = &color;
		softReset.init(); item[i++] = &softReset;
		assert(i <= sizeofArray(item));
		BaseMenuView::init(item, i, highlightFirst);
	}

	void onShow()
	{
		diff1.set(p1DiffB, *this);
		diff2.set(p2DiffB, *this);
		color.set(vcsColor, *this);
	}

};

class SystemMenuView : public MenuView
{
private:
	TextMenuItem switches
	{
		"Console Switches",
		[this](TextMenuItem &, const Input::Event &e)
		{
			if(EmuSystem::gameIsRunning())
			{
				auto &vcsSwitchesView = *menuAllocator.allocNew<VCSSwitchesView>(window());
				vcsSwitchesView.init(!e.isPointer());
				viewStack.pushAndShow(vcsSwitchesView, &menuAllocator);
			}
		}
	};

public:
	SystemMenuView(Base::Window &win): MenuView(win) {}

	void onShow()
	{
		MenuView::onShow();
		switches.active = EmuSystem::gameIsRunning();
	}

	void init(bool highlightFirst)
	{
		uint items = 0;
		loadFileBrowserItems(item, items);
		switches.init(); item[items++] = &switches;
		loadStandardItems(item, items);
		assert(items <= sizeofArray(item));
		BaseMenuView::init(item, items, highlightFirst);
	}
};
