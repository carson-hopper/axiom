<p align="center">
  <img src="assets/axiom-logo.png" alt="Axiom" width="280">
</p>

<h1 align="center">Axiom</h1>

<p align="center">
  <b>Build Beyond Vanilla</b>
  <br><br>
  A high-performance Minecraft server written in modern C++23<br>
  with a built-in plugin API, async networking, and clean architecture.
</p>

<p align="center">
  <a href="https://github.com/carson-hopper/axiom/blob/main/LICENSE"><img src="https://img.shields.io/github/license/carson-hopper/axiom?style=flat-square&color=7c6caf" alt="License"></a>
  <a href="https://github.com/carson-hopper/axiom/releases"><img src="https://img.shields.io/github/v/release/carson-hopper/axiom?style=flat-square&color=7c6caf&label=release" alt="Latest Release"></a>
  <a href="https://github.com/carson-hopper/axiom/actions"><img src="https://img.shields.io/github/actions/workflow/status/carson-hopper/axiom/build.yml?style=flat-square&label=build" alt="Build Status"></a>
  <img src="https://img.shields.io/badge/C%2B%2B-23-7c6caf?style=flat-square&logo=cplusplus&logoColor=white" alt="C++23">
  <img src="https://img.shields.io/badge/cmake-3.28%2B-7c6caf?style=flat-square&logo=cmake&logoColor=white" alt="CMake 3.28+">
  <a href="STYLE.md"><img src="https://img.shields.io/badge/style-axiom--cpp-7c6caf?style=flat-square" alt="Code Style"></a>
</p>

<p align="center">
  <a href="#getting-started">Getting Started</a> •
  <a href="#features">Features</a> •
  <a href="#documentation">Docs</a> •
  <a href="#contributing">Contributing</a> •
  <a href="#license">License</a>
</p>

---

## About

Axiom is an open-source Minecraft 26.1 server built from scratch in modern C++23. It provides a structured, high-performance foundation with a built-in plugin API — commands, events, permissions, configuration, world management, and more — designed for scalability and clean architecture.

Axiom follows strict code style conventions (PascalCase methods, `m_` member prefixes, `Scope`/`Ref` smart pointer aliases), targets **C++23** as its baseline, and uses **CMake** for builds.

## Features

- **Network Layer** — Async TCP networking with protocol-level packet handling, encryption, and compression.
- **Plugin API** — Built-in plugin system with dependency resolution, scoped event buses, and isolated classloading.
- **Command Framework** — Registration-based command system with argument parsing, tab completion, and permission checks.
- **Event System** — Priority-ordered event dispatch with cancellation support and type-safe listeners.
- **World Engine** — Chunk loading, block storage, entity management, and fluid simulation.
- **Configuration** — YAML-backed configuration with type-safe accessors and hot-reload support.
- **Modern C++** — Concepts, coroutines, std::expected, std::format, structured bindings — Axiom embraces modern C++ idioms across the board.

## Getting Started

### Requirements

- **C++23** compatible compiler (GCC 14+, Clang 18+, MSVC 19.38+)
- **CMake 3.28** or newer
- **vcpkg** or **Conan 2** for dependency management

### Building

```bash
git clone https://github.com/carson-hopper/axiom.git
cd axiom
cmake --preset release
cmake --build --preset release
```

### Running

```bash
./build/release/axiom-server
```

## Documentation

Full documentation is available at **[docs.axiommc.com](https://docs.axiommc.com)** *(coming soon)*.

| Resource | Description |
|---|---|
| [API Reference](https://docs.axiommc.com/api) | Doxygen-generated API docs |
| [Guides](https://docs.axiommc.com/guides) | Step-by-step tutorials for common tasks |
| [Examples](examples/) | Example plugins in this repository |
| [Style Guide](STYLE.md) | C++ coding standards enforced in this project |

## Contributing

Contributions are welcome and appreciated! Before opening a PR, please read the following carefully.

### Code Style

This project enforces a strict code style. See [`STYLE.md`](STYLE.md) for the full guide. The key points:

- **PascalCase** for classes, methods, and enums. **camelCase** for local variables and parameters.
- **`m_` prefix** for member variables, **`s_` prefix** for static members.
- **Tabs** for indentation, **120-character** column limit.
- **`Scope<T>`/`Ref<T>`** aliases for `std::unique_ptr`/`std::shared_ptr`.
- **Format with `clang-format`** before committing — configuration is in `.clang-format` at the repo root.
- **Commits must follow [Conventional Commits](https://www.conventionalcommits.org/)** with module-scoped prefixes (e.g., `feat(network): add packet compression`).

> **Pull requests that do not follow the style guide will be rejected.** Run the formatter before pushing — CI will catch violations anyway.

### How to Contribute

1. **Fork** the repository.
2. **Create a branch** from `main` for your feature or fix.
3. **Write code** following the style guide and add tests where applicable.
4. **Format** your code: `clang-format -i src/**/*.h src/**/*.cpp`
5. **Commit** using Conventional Commits: `feat(network): add packet compression`
6. **Open a PR** against `main` with a clear description of the change.

### Reporting Issues

Open an issue on GitHub with a clear title and as much detail as possible. Include OS, compiler version, and steps to reproduce.

## License

Axiom is licensed under the **GNU General Public License v3.0** — see the [`LICENSE`](LICENSE) file for details.

This means:

- You are **free to use, modify, and distribute** this software.
- Any modified versions **must also be open-source** under GPL v3.
- You **must give credit** to the original authors and indicate changes made.
- You **must include the original license** and copyright notice in any distribution.

---
