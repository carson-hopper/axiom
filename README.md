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
  <a href="https://github.com/carson-hopper/axiom/blob/main/LICENSE"><img src="https://img.shields.io/badge/license-AGPL--3.0%20%2F%20Apache--2.0-7c6caf?style=flat-square" alt="License"></a>
  <a href="https://github.com/carson-hopper/axiom/releases"><img src="https://img.shields.io/github/v/release/carson-hopper/axiom?style=flat-square&color=7c6caf&label=release" alt="Latest Release"></a>
  <a href="https://github.com/carson-hopper/axiom/actions"><img src="https://img.shields.io/github/actions/workflow/status/carson-hopper/axiom/build.yml?style=flat-square&label=build" alt="Build Status"></a>
  <img src="https://img.shields.io/badge/C%2B%2B-23-7c6caf?style=flat-square&logo=cplusplus&logoColor=white" alt="C++23">
  <img src="https://img.shields.io/badge/premake-5-7c6caf?style=flat-square" alt="Premake5">
  <a href="STYLE.md"><img src="https://img.shields.io/badge/style-axiom--cpp-7c6caf?style=flat-square" alt="Code Style"></a>
</p>

<p align="center">
  <a href="#getting-started">Getting Started</a> &bull;
  <a href="#features">Features</a> &bull;
  <a href="#documentation">Docs</a> &bull;
  <a href="#contributing">Contributing</a> &bull;
  <a href="#license">License</a>
</p>

---

## About

Axiom is an open-source Minecraft 26.1 server built from scratch in modern C++23. It provides a structured, high-performance foundation with a built-in plugin API: commands, events, permissions, configuration, world management, and more. Designed for scalability and clean architecture.

Axiom follows strict code style conventions (PascalCase methods, `m_` member prefixes, `Scope`/`Ref` smart pointer aliases), targets **C++23** as its baseline, and uses **Premake5** for builds.

## Features

| Feature | Description |
|---|---|
| **Network Layer** | Async TCP networking with protocol-level packet handling, encryption, and compression |
| **Plugin API** | Built-in plugin system with dependency resolution, scoped event buses, and isolated classloading |
| **Command Framework** | Registration-based command system with argument parsing, tab completion, and permission checks |
| **Event System** | Priority-ordered event dispatch with cancellation support and type-safe listeners |
| **World Engine** | Chunk loading, block storage, entity management, and fluid simulation |
| **Configuration** | TOML-backed configuration with type-safe accessors and hot-reload support |
| **Modern C++** | Concepts, coroutines, std::expected, std::format, structured bindings |

## Getting Started

### Prerequisites

| Dependency | Version | macOS | Linux | Windows |
|---|---|---|---|---|
| C++ compiler | Clang 18+ / GCC 14+ / MSVC 19.38+ | Xcode CLT | `apt install g++-14` | Visual Studio 2022 |
| Premake5 | 5.x | `brew install premake` | [premake.github.io](https://premake.github.io) | [premake.github.io](https://premake.github.io) |
| OpenSSL | 3.x | `brew install openssl` | `apt install libssl-dev` | [slproweb.com](https://slproweb.com/products/Win32OpenSSL.html) |
| zlib | any | included with macOS | `apt install zlib1g-dev` | [zlib.net](https://zlib.net) |
| Git | any | `brew install git` | `apt install git` | [git-scm.com](https://git-scm.com) |

Vendors (asio, nlohmann/json, spdlog, toml++, yaml-cpp) are included as git submodules. After cloning, run:

```bash
git submodule update --init --recursive
```

### Setup

Clone the repository and run the setup script:

```bash
git clone https://github.com/carson-hopper/axiom.git
cd axiom
./scripts/setup.sh
```

The setup script will prompt you to choose your IDE:

```
Select your IDE:

  1) CLion
  2) Xcode
  3) VS Code
  4) Visual Studio
  5) Terminal / gmake only

>
```

You can also pass the IDE directly:

```bash
./scripts/setup.sh --ide vscode
./scripts/setup.sh --ide clion
./scripts/setup.sh --ide xcode
./scripts/setup.sh --ide vs
./scripts/setup.sh --ide gmake
```

#### What the setup script does

| IDE | Generates |
|---|---|
| **CLion** | gmake Makefiles + `compile_commands.json` (via bear) |
| **VS Code** | gmake Makefiles + `compile_commands.json` + `.vscode/launch.json` + `.vscode/tasks.json` + `.code-workspace` |
| **Xcode** | gmake Makefiles + `.xcodeproj` / `.xcworkspace` |
| **Visual Studio** | gmake Makefiles + `.sln` (vs2022) |
| **gmake** | gmake Makefiles only |

For VS Code, the [premake-vscode](https://github.com/Enhex/premake-vscode) module is downloaded automatically if not present.

#### Additional flags

```
--config <cfg>    Build configuration (default: debug_26.1)
--clean           Clean build artifacts before building
```

### Building manually

If you prefer to skip the setup script:

```bash
premake5 gmake
make config=debug_26.1 -j$(sysctl -n hw.ncpu)
```

### Running

```bash
cd bin/Debug_26.1_AARCH64/Axiom
./Axiom
```

On first run, an `eula.txt` file will be created. Edit it and set `eula=true` to accept, then run again.

### Build configurations

| Configuration | Optimization | Debug symbols | Use case |
|---|---|---|---|
| `debug_26.1` | Off | On | Development and debugging |
| `release_26.1` | Full | Off | Production builds |

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

| Convention | Rule |
|---|---|
| **Naming** | PascalCase for classes, methods, and enums. camelCase for locals and parameters. |
| **Members** | `m_` prefix for member variables, `s_` prefix for static members. |
| **Formatting** | Tabs for indentation, 120 character column limit. |
| **Smart Pointers** | `Scope<T>` / `Ref<T>` aliases for `std::unique_ptr` / `std::shared_ptr`. |
| **Commits** | [Conventional Commits](https://www.conventionalcommits.org/) with scoped prefixes (e.g., `feat(network): add packet compression`). |

> **Pull requests that do not follow the style guide will be rejected.**

### How to Contribute

1. **Fork** the repository.
2. **Create a branch** from `main` for your feature or fix.
3. **Run the setup script** for your IDE: `./scripts/setup.sh`
4. **Write code** following the style guide and add tests where applicable.
5. **Commit** using Conventional Commits: `feat(network): add packet compression`
6. **Open a PR** against `main` with a clear description of the change.

### Reporting Issues

Open an issue on GitHub with a clear title and as much detail as possible. Include OS, compiler version, and steps to reproduce.

## License

Axiom uses a dual-license model. See [`NOTICE.md`](NOTICE.md) for details.

| Component | License |
|---|---|
| **Server** (everything except the plugin API) | [AGPL-3.0](LICENSE) |
| **Plugin API** (`Axiom/src/Axiom/Plugin/`) | [Apache-2.0](LICENSE-PLUGIN-API) |

The server is licensed under the **GNU Affero General Public License v3.0**. Modifications to the server that are made publicly available (including via hosted services) must be released under the same license. The plugin API is licensed under the **Apache License 2.0**, meaning plugin developers are free to release their plugins under any license they choose, including proprietary licenses.

---
