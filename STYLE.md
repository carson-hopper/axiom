# C++ Style Guide

**Modern C++23 • Clean Architecture**

A comprehensive, opinionated coding standard for Axiom targeting C++23.

**Applies to:** C++23 projects | **Format tool:** `clang-format` | **Lint:** `clang-tidy`

---

## 1. Source File Basics

### 1.1 File Names

Header files use `.h`, source files use `.cpp`. File names match the primary class they contain, in PascalCase: `PacketHandler.h`, `NetworkServer.cpp`. One primary class per file.

### 1.2 Encoding

All source files are encoded in **UTF-8**. No exceptions.

### 1.3 Header Protection

Use `#pragma once` at the top of every header file. Do not use traditional include guards.

```cpp
#pragma once

#include "Axiom/Core/Base.h"
```

### 1.4 Include Organization

Includes appear in this order, separated by blank lines between groups:

1. Corresponding header (in `.cpp` files only)
2. Project headers (`"Axiom/..."`)
3. Third-party library headers (`<spdlog/...>`, `<asio/...>`)
4. Standard library headers (`<string>`, `<vector>`, `<memory>`)

```cpp
#include "NetworkServer.h"

#include "Axiom/Core/Log.h"
#include "Axiom/Network/Connection.h"

#include <asio/io_context.hpp>

#include <string>
#include <vector>
```

### 1.5 Source File Structure

Every source file appears in this order:

1. License or copyright header (if present)
2. `#pragma once` (headers only)
3. Includes
4. Forward declarations
5. Namespace block
6. Implementation

---

## 2. Formatting

### 2.1 Braces

Follow Kernighan & Ritchie ("Egyptian brackets") style — opening brace on the same line:

```cpp
class Player {
public:
	void SendMessage(const std::string& message) {
		if (IsConnected()) {
			m_Connection->Write(message);
		} else {
			AX_CORE_WARN("Player {} is not connected", m_Name);
		}
	}
};
```

Empty bodies may be closed immediately: `void OnUpdate() {}`

### 2.2 Indentation

**Tabs** for indentation. One tab per block level. Never spaces for indentation.

Alignment (lining up tokens across lines) uses spaces after the leading tabs.

### 2.3 One Statement Per Line

Each statement is followed by a line break.

### 2.4 Column Limit: 120

Lines should not exceed 120 characters. Exceptions: long URLs in comments, `#include` paths, and string literals.

### 2.5 Line Wrapping

- Break at a **higher syntactic level** when possible.
- Break **after** opening parentheses or **before** operators.
- Continuation lines are indented one additional tab from the original line.

### 2.6 Vertical Whitespace

A single blank line separates logical sections within a class: member groups, method implementations. Two blank lines between top-level definitions (classes, free functions) in a source file.

### 2.7 Namespace Indentation

Content inside namespace blocks is indented one level. Opening brace on the same line as the namespace declaration:

```cpp
namespace Axiom {

	class Application {
	public:
		void Run();
	};

}
```

For nested namespaces, use C++17 nested syntax:

```cpp
namespace Axiom::Network {

	class Connection {
		// ...
	};

}
```

### 2.8 Access Specifier Layout

Access specifiers (`public`, `protected`, `private`) are not indented relative to the class. Members beneath them are indented one level:

```cpp
class Window {
public:
	void Show();
	uint32_t Width() const;

private:
	uint32_t m_Width = 0;
	uint32_t m_Height = 0;
};
```

---

## 3. Naming Conventions

### 3.1 General Rules

Identifiers use only ASCII letters, digits, and underscores. Every identifier should be descriptive and unambiguous. Use full words — never abbreviations like `buf`, `pos`, `conn`, or `mgr`.

### 3.2 Naming by Identifier Type

| Identifier | Convention | Example |
|---|---|---|
| Namespace | PascalCase | `Axiom`, `Axiom::Network` |
| Class / Struct | PascalCase | `PacketHandler`, `BlockStore` |
| Enum (scoped) | PascalCase | `ConnectionState` |
| Enum value | PascalCase | `Connected`, `Disconnected` |
| Method / Function | PascalCase | `SendMessage`, `IsConnected` |
| Member variable | `m_` + PascalCase | `m_PlayerName`, `m_Health` |
| Static member | `s_` + PascalCase | `s_Instance`, `s_Allocator` |
| Local variable | camelCase | `playerCount`, `blockState` |
| Parameter | camelCase | `fileName`, `maxRetries` |
| Constant (`constexpr`) | PascalCase or CONSTANT_CASE | `MaxRetries`, `DEFAULT_PORT` |
| Template parameter | Single letter or PascalCase | `T`, `EventType` |
| Macro | `AX_` + CONSTANT_CASE | `AX_CORE_ASSERT`, `AX_BIND_FN` |

### 3.3 Acronyms

Treat acronyms as single words in PascalCase: `HttpClient`, not `HTTPClient`. `NbtParser`, not `NBTParser`. `JsonReader`, not `JSONReader`.

---

## 4. Smart Pointers & Ownership

Axiom uses two project-wide smart-pointer types: `Scope<T>` for sole ownership and `Ref<T>` for shared ownership. `Scope<T>` is a thin alias over `std::unique_ptr<T>`. `Ref<T>` is a **custom intrusive** smart pointer, not `std::shared_ptr`, and the distinction matters — see 4.2.

### 4.1 `Scope<T>`

`Scope<T>` is `std::unique_ptr<T>`. Use it for sole ownership — this is the default for every owning pointer unless you have a specific reason to reach for `Ref<T>`.

```cpp
template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}
```

### 4.2 `Ref<T>` — intrusive, not `std::shared_ptr`

`Ref<T>` is a custom intrusive reference-counted smart pointer defined in `Axiom/Utilities/Memory/Ref.h`. It is **not** `std::shared_ptr`, and is not interchangeable with it.

The reference count lives on the object itself via the `RefCounted` base class:

```cpp
class RefCounted {
public:
	virtual ~RefCounted() = default;
	void IncRefCount() const;
	void DecRefCount() const;
	uint32_t GetRefCount() const;

private:
	mutable std::atomic<uint32_t> m_RefCount = 0;
};
```

Any class that wants to be held by `Ref<T>` must publicly inherit from `RefCounted`. The templated constructor enforces this at compile time:

```cpp
static_assert(std::is_base_of_v<RefCounted, T>, "Class is not RefCounted!");
```

**Why intrusive?** So that `Ref<T>(this)` works correctly from any method of a `RefCounted`-derived class. This is a common pattern across the network and async layers:

```cpp
void Connection::ReadFrameLength() {
	auto self = Ref<Connection>(this);
	m_Socket.async_read(..., [self](const asio::error_code& errorCode, std::size_t bytesRead) {
		// `self` keeps the Connection alive for the duration of the async operation
	});
}
```

With `std::shared_ptr`, wrapping `this` would create a second, independent control block and lead to a double delete. The workaround — `enable_shared_from_this<T>` + `shared_from_this()` — only works if the object is already owned by a `shared_ptr` and throws `bad_weak_ptr` in a constructor or after the last owner drops. The intrusive design avoids this entirely: the count is on the object, so any `RefCounted*` can be wrapped in a fresh `Ref<T>` at any point in its lifetime.

**Construction:**

```cpp
template<typename T, typename... Args>
Ref<T> CreateRef(Args&&... args) {
	return Ref<T>::Create(std::forward<Args>(args)...);
}
```

Factory methods return `Scope<T>` or `Ref<T>`. Never `new` raw.

### 4.3 Usage Rules

- Use `Scope<T>` for sole ownership. **This is the default.**
- Use `Ref<T>` when shared ownership is genuinely needed — typically for objects whose lifetime spans async callbacks, for objects referenced from multiple subsystems, or for any class that needs to wrap `this` in a smart pointer.
- Any class held by `Ref<T>` must publicly inherit from `RefCounted`.
- Use raw pointers (`T*`) only for non-owning observation. Never `delete` a raw pointer.
- Prefer passing by `const&` or `std::string_view` over pointer parameters.
- Factory methods return `Scope<T>` or `Ref<T>`, never raw `new`.
- `Ref<T>(this)` is the idiomatic way to extend lifetime inside a `RefCounted`-derived method.

---

## 5. Classes & Structs

### 5.1 Struct vs Class

Use `struct` for plain data aggregates with no invariants. Use `class` for types with behavior, invariants, or non-trivial lifetimes.

```cpp
struct WindowProperties {
	std::string Title = "Axiom";
	uint32_t Width = 1280;
	uint32_t Height = 720;
	bool VSync = true;
};
```

### 5.2 Member Order

Within a class, order members as:

1. `public` types and aliases
2. `public` constructors and destructor
3. `public` methods
4. `protected` methods
5. `private` methods
6. `private` member variables

### 5.3 Constructors

- Use member initializer lists, not assignment in the constructor body.
- Mark single-argument constructors `explicit` unless implicit conversion is intentional.
- Use `= default` for trivial constructors and destructors.
- Use `= delete` to prohibit copy/move when appropriate.

### 5.4 Virtual Methods

- Always use `override` on overriding methods.
- Always use `virtual` on the base class declaration.
- Use `= default` for virtual destructors in base classes.
- Prefer `final` on classes/methods that should not be further overridden.

---

## 6. Modern C++ Idioms

### 6.1 auto

Use `auto` sparingly. Prefer explicit types for clarity. `auto` is acceptable for:

- Iterator types: `auto iterator = map.find(key);`
- Factory return types where the type is obvious: `auto window = CreateScope<Window>(properties);`
- Range-based for loops: `for (auto& element : elements)`

Do not use `auto` when the type is not immediately obvious from the right-hand side.

### 6.2 constexpr and const

- Mark variables `const` whenever they should not change after initialization.
- Mark functions `constexpr` when they can be evaluated at compile time.
- Mark methods `const` when they do not modify object state.

### 6.3 Enums

Always use `enum class` for type safety. PascalCase for both the enum name and its values:

```cpp
enum class ConnectionState {
	Disconnected,
	Handshaking,
	LoggingIn,
	Configuring,
	Playing
};
```

Use a loose `enum` (not `enum class`) only for bitfield flags, paired with a `BIT()` macro:

```cpp
enum EventCategory {
	None                = 0,
	EventCategoryInput  = BIT(0),
	EventCategoryWorld  = BIT(1),
	EventCategoryPlayer = BIT(2)
};
```

### 6.4 std::expected and Error Handling

- Use `std::expected<T, E>` for operations that can fail with a known error type.
- Use exceptions only for truly exceptional conditions (out of memory, corrupted state).
- Never silently ignore errors. Log at minimum.

### 6.5 Concepts and Constraints

Use concepts to constrain templates instead of SFINAE or static_assert:

```cpp
template<typename T>
concept Serializable = requires(T value, Buffer& buffer) {
	{ value.Serialize(buffer) } -> std::same_as<void>;
	{ T::Deserialize(buffer) } -> std::same_as<T>;
};
```

### 6.6 std::format

Prefer `std::format` (or `fmt::format`) over manual string concatenation or `sprintf`.

---

## 7. Macros

### 7.1 Naming

All macros use the `AX_` prefix followed by CONSTANT_CASE: `AX_CORE_ASSERT`, `AX_BIND_FN`, `AX_ENABLE_ASSERTS`.

### 7.2 Usage Guidelines

- Minimize macro usage. Prefer `constexpr`, `inline`, and templates.
- Macros are acceptable for: assertions, logging wrappers, platform detection, and boilerplate reduction (e.g., event class type/category macros).
- Always use `do { ... } while (0)` for multi-statement macros.
- Use `AX_EXPAND_MACRO` / `AX_STRINGIFY_MACRO` helpers for macro composition.

---

## 8. Comments

### 8.1 Style

Use `//` for single-line and inline comments. Use `/** */` for documentation comments on public APIs.

### 8.2 Documentation Comments

```cpp
/**
 * Sends a chat message to the player.
 *
 * The message is queued and delivered on the next
 * network tick. If the player disconnects before
 * delivery, the message is discarded.
 *
 * @param message The chat message content.
 * @return true if the message was queued.
 */
bool SendMessage(const std::string& message);
```

### 8.3 Section Dividers

Use decorated comments to visually separate logical groups within a file:

```cpp
// ----- Lifecycle ------------------------------------------------

void Init();
void Shutdown();

// ----- Rendering ------------------------------------------------

void BeginFrame();
void EndFrame();
```

### 8.4 TODO Comments

```cpp
// TODO(gh-123): Remove after the v2 migration is complete.
```

---

## 9. Platform Abstraction

### 9.1 Conditional Compilation

Use `AX_PLATFORM_*` macros for platform-specific code. Isolate platform-specific implementations in separate source files, not `#ifdef` blocks scattered through shared code:

```
Axiom/Platform/Linux/LinuxWindow.cpp
Axiom/Platform/Windows/WindowsWindow.cpp
```

### 9.2 Platform Macros

| Macro | Platform |
|---|---|
| `AX_PLATFORM_LINUX` | Linux |
| `AX_PLATFORM_WINDOWS` | Windows |
| `AX_PLATFORM_MACOS` | macOS |

---

## 10. Project Structure

```
Axiom/
├── src/
│   └── Axiom/
│       ├── Core/           # Application, Log, Assert, Base
│       ├── Network/        # Connection, PacketHandler, Protocol
│       ├── World/          # Chunk, Block, Entity, World
│       ├── Plugin/         # Plugin API, PluginManager
│       ├── Command/        # Command registry and parsing
│       ├── Event/          # Event system and categories
│       ├── Config/         # Server configuration
│       └── Platform/       # OS-specific implementations
├── tests/
├── vendor/                 # Third-party dependencies
├── CMakeLists.txt
└── CMakePresets.json
```

Headers and source files live together in the same directory (not split into `include/` and `src/`). This keeps related code co-located.

---

## 11. Enforcement

### 11.1 Formatter

Use `clang-format` with the project's `.clang-format` configuration at the repo root. Integrate it as a pre-commit hook or CI step:

```bash
clang-format -i src/**/*.h src/**/*.cpp
```

### 11.2 Linter

Use `clang-tidy` with the project's `.clang-tidy` configuration. Key checks: modernize-*, readability-*, performance-*, bugprone-*.

### 11.3 Code Review

Style enforcement in code review should be minimal — the formatter and linter should catch most issues. Reserve review comments for naming quality, ownership semantics, API design, and architecture decisions.
