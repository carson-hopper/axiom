#include "Axiom/Core/EntryPoint.h"

namespace Axiom {
    
	Scope<Application> CreateApplication(const ApplicationCommandLineArgs args) {
		ApplicationSpecification spec;
		spec.Name = "Axiom Server";
		spec.CommandLineArgs = args;

		auto app = CreateScope<Application>(spec);
		app->Init();
		return app;
	}

}
