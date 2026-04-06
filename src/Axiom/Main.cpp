#include "Axiom/Core/Application.h"
#include "Axiom/Core/Log.h"

int main() {
	Axiom::Application application;
	application.Init();
	application.Run();
	application.Shutdown();
	return 0;
}
