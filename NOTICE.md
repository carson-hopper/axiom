# Axiom Licensing

Axiom uses a dual-license model to balance strong copyleft protection for the
server implementation with maximum flexibility for plugin developers.

## Server (AGPL-3.0)

The Axiom server implementation is licensed under the
**GNU Affero General Public License v3.0** ([LICENSE](LICENSE)).

This covers all source code in this repository **except** the plugin API
directory listed below. If you modify the Axiom server and run it publicly
(including as a hosted service), you must make your modifications available
under the same license to all users who interact with it over the network.

## Plugin API (Apache-2.0)

The plugin API, located at:

```
Axiom/src/Axiom/Plugin/
```

is licensed under the **Apache License 2.0** ([LICENSE-PLUGIN-API](LICENSE-PLUGIN-API)).

This allows plugin developers to write plugins under any license they choose,
including proprietary or closed-source licenses, without being required to
release their plugin source code under the AGPL.

## What this means for you

| Scenario | License that applies |
|---|---|
| Running Axiom as-is on your server | AGPL-3.0 (no obligations beyond attribution) |
| Modifying Axiom's server code and hosting publicly | AGPL-3.0 (must share modifications) |
| Writing a plugin that uses the Plugin API | Apache-2.0 (any license, including proprietary) |
| Forking the server for your own distribution | AGPL-3.0 (must share source) |

## Contributing

By contributing to this repository, you agree that your contributions will be
licensed under the same dual-license scheme: AGPL-3.0 for server code and
Apache-2.0 for plugin API code, matching the directory where the contribution
is made.
