#pragma once
#include "Axiom/Core/Base.h"
#include "Axiom/Core/Application.h"

#if defined(AX_PLATFORM_LINUX) || defined(AX_PLATFORM_MACOS)

extern Axiom::Scope<Axiom::Application> CreateApplication(Axiom::ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Axiom::Log::Init();

	const auto app = Axiom::CreateApplication({ argc, argv });
	app->Run();
}

#endif