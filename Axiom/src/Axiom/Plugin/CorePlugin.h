#pragma once

#include "Axiom/Plugin/Plugin.h"

namespace Axiom {

	class CorePlugin : public Plugin {
	public:
		CorePlugin();

		void OnEnable(PluginContext& context) override;
		void OnDisable() override;
	};

}
