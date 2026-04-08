#!/usr/bin/env python3
"""
Packet Code Generator

Reads packet definitions from YAML and generates C++ code with automatic serialization.
Supports multiple protocol versions.
"""

import yaml
import sys
from pathlib import Path
from typing import Dict, List, Optional, Any


def to_camel_case(snake_str: str) -> str:
    """Convert snake_case to camelCase."""
    components = snake_str.split("_")
    return components[0] + "".join(x.capitalize() for x in components[1:])


def to_pascal_case(name: str) -> str:
    """Convert snake_case or camelCase to PascalCase."""
    if "_" in name:
        # snake_case input
        components = name.split("_")
        return "".join(x.capitalize() for x in components)
    else:
        # camelCase input - capitalize first letter
        return name[0].upper() + name[1:]


class TypeMapper:
    """Maps schema types to C++ types and serialization methods."""

    TYPE_MAP = {
        "Bool": ("bool", "ReadBoolean", "WriteBoolean"),
        "Byte": ("int8_t", "ReadByte", "WriteByte"),
        "UnsignedByte": ("uint8_t", "ReadUnsignedByte", "WriteUnsignedByte"),
        "Short": ("int16_t", "ReadShort", "WriteShort"),
        "UnsignedShort": ("uint16_t", "ReadUnsignedShort", "WriteUnsignedShort"),
        "Int": ("int32_t", "ReadInt", "WriteInt"),
        "Long": ("int64_t", "ReadLong", "WriteLong"),
        "Float": ("float", "ReadFloat", "WriteFloat"),
        "Double": ("double", "ReadDouble", "WriteDouble"),
        "String": ("std::string", "ReadString", "WriteString"),
        "UUID": ("UUID", "ReadUUID", "WriteUUID"),
        "VarInt": ("int32_t", "ReadVarInt", "WriteVarInt"),
        "VarLong": ("int64_t", "ReadVarLong", "WriteVarLong"),
        "ChatComponent": (
            "Ref<ChatComponent>",
            "ReadChatComponent",
            "WriteChatComponent",
        ),
        "NBT": ("nbt::NBT", "ReadNBT", "WriteNBT"),
        "Position": ("BlockPos", "ReadBlockPosition", "WriteBlockPosition"),
        "ByteArray": ("std::vector<uint8_t>", "ReadByteArray", "WriteByteArray"),
        "VarIntArray": ("std::vector<int32_t>", "ReadVarIntArray", "WriteVarIntArray"),
        "BlockEntityArray": (
            "std::vector<BlockEntity>",
            "ReadBlockEntities",
            "WriteBlockEntities",
        ),
    }

    @classmethod
    def get_cpp_type(cls, schema_type: str) -> str:
        if schema_type.startswith("Optional<"):
            inner = schema_type[9:-1]  # Extract inner type
            return f"std::optional<{cls.get_cpp_type(inner)}>"
        elif schema_type.startswith("ByteArray<"):
            return "std::vector<uint8_t>"
        return cls.TYPE_MAP.get(schema_type, ("std::string", "", ""))[0]

    @classmethod
    def get_read_method(cls, schema_type: str) -> str:
        if schema_type.startswith("Optional<"):
            inner = schema_type[9:-1]
            return f"ReadOptional(&NetworkBuffer::{cls.get_read_method(inner)})"
        elif schema_type.startswith("ByteArray<"):
            size = schema_type[10:-1]  # Extract size
            return f"ReadByteArray({size})"
        return cls.TYPE_MAP.get(schema_type, ("", "ReadString", ""))[1]

    @classmethod
    def get_write_method(cls, schema_type: str) -> str:
        if schema_type.startswith("Optional<"):
            inner = schema_type[9:-1]
            return f"WriteOptional(&NetworkBuffer::{cls.get_write_method(inner)})"
        elif schema_type.startswith("ByteArray<"):
            return "WriteByteArray"
        return cls.TYPE_MAP.get(schema_type, ("", "", "WriteString"))[2]


def generate_packet_header(packet_name: str, packet_def: Dict, version: int) -> str:
    """Generate C++ header file for a packet."""

    direction = packet_def["direction"]
    state = packet_def["state"]
    packet_id = packet_def["id"]
    fields = packet_def.get("fields", [])

    base_class = (
        "ServerboundPacket" if direction == "Serverbound" else "ClientboundPacket"
    )

    # Generate field declarations
    field_decls = []
    getter_setters = []

    for field in fields:
        raw_name = field["name"]
        name = to_camel_case(raw_name)
        schema_type = field["type"]
        cpp_type = TypeMapper.get_cpp_type(schema_type)

        member_name = f"m{to_pascal_case(name)}"
        field_decls.append(f"    {cpp_type} {member_name};")

        getter_name = to_pascal_case(raw_name)
        if cpp_type == "bool":
            getter_name = f"Is{getter_name}"
        else:
            getter_name = f"Get{getter_name}"

        if schema_type.startswith("Optional<"):
            getter_setters.append(
                f"    const {cpp_type}& {getter_name}() const {{ return {member_name}; }}"
            )
        else:
            getter_setters.append(
                f"    {cpp_type} {getter_name}() const {{ return {member_name}; }}"
            )

        setter_name = f"Set{getter_name[3:] if not getter_name.startswith('Is') else getter_name[2:]}"
        getter_setters.append(
            f"    void {setter_name}({cpp_type} value) {{ {member_name} = std::move(value); }}"
        )

    # Generate includes
    includes = [
        "#pragma once",
        "",
        '#include "Axiom/Core/Base.h"',
        '#include "Axiom/Network/Packet/{0}.h"'.format(base_class),
    ]

    # Add type-specific includes
    includes.append('#include "Axiom/Network/NetworkBuffer.h"')
    if any("ChatComponent" in f["type"] for f in fields):
        includes.append('#include "Axiom/Chat/ChatComponent.h"')
    if any("UUID" in f["type"] for f in fields):
        includes.append('#include "Axiom/Core/UUID.h"')
    if any(f["type"].startswith("Optional<") for f in fields):
        includes.append("#include <optional>")
    if any(
        f["type"].startswith("ByteArray") or f["type"].endswith("Array") for f in fields
    ):
        includes.append("#include <vector>")

    includes.append("")
    includes.append("namespace Axiom {")
    includes.append("")

    # Class declaration
    lines = [
        f"class {packet_name} : public {base_class} {{",
        "public:",
        f"    static constexpr int32_t PacketId = {packet_id};",
        f"    static constexpr ConnectionState PacketState = ConnectionState::{state};",
        "",
    ]

    lines.append(f"    {packet_name}() = default;")
    lines.append(f"    {packet_name}({packet_name}&&) = default;")
    lines.append(f"    {packet_name}& operator=({packet_name}&&) = default;")
    lines.append("")

    if fields:
        params = []
        inits = []
        for f in fields:
            raw_name = f["name"]
            name = to_camel_case(raw_name)
            cpp_type = TypeMapper.get_cpp_type(f["type"])
            member_name = f"m{name[0].upper() + name[1:]}"
            params.append(f"{cpp_type} {name}")
            inits.append(f"{member_name}(std::move({name}))")

        lines.append(f"    {packet_name}({', '.join(params)})")
        lines.append("        : " + ", ".join(inits))
        lines.append("    {}")

    lines.append("")

    # Getters and setters
    lines.extend(getter_setters)
    lines.append("")

    # Serialize/Deserialize
    if direction == "Serverbound":
        lines.append("    void Deserialize(NetworkBuffer& buffer) override;")
        lines.append(
            "    void Handle(Ref<Connection> connection, PacketContext& context) override;"
        )
    else:
        lines.append("    int32_t GetPacketId() const override { return PacketId; }")
        lines.append("    void Serialize(NetworkBuffer& buffer) const override;")

    lines.append("")
    lines.append("private:")
    lines.extend(field_decls)
    lines.append("};")
    lines.append("")
    lines.append("} // namespace Axiom")

    return "\n".join(includes + lines)


def generate_packet_impl(packet_name: str, packet_def: Dict, version: int) -> str:
    """Generate C++ implementation file for a packet."""

    direction = packet_def["direction"]
    fields = packet_def.get("fields", [])

    lines = [f'#include "{packet_name}.h"', "namespace Axiom {", ""]

    if direction == "Serverbound":
        lines.append(f"void {packet_name}::Deserialize(NetworkBuffer& buffer) {{")
        for field in fields:
            raw_name = field["name"]
            name = to_camel_case(raw_name)
            schema_type = field["type"]
            read_method = TypeMapper.get_read_method(schema_type)
            member_name = f"m{name[0].upper() + name[1:]}"
            lines.append(f"    {member_name} = buffer.{read_method};")
        lines.append("}")
        lines.append("")

        lines.append(
            f"void {packet_name}::Handle(Ref<Connection> connection, PacketContext& context) {{"
        )
        lines.append("    // TODO: Implement packet handling")
        lines.append("}")
    else:
        lines.append(f"void {packet_name}::Serialize(NetworkBuffer& buffer) const {{")
        for field in fields:
            raw_name = field["name"]
            name = to_camel_case(raw_name)
            schema_type = field["type"]
            write_method = TypeMapper.get_write_method(schema_type)
            member_name = f"m{name[0].upper() + name[1:]}"

            if schema_type.startswith("Optional<"):
                lines.append(f"    buffer.{write_method}({member_name});")
            elif schema_type.startswith("ByteArray<"):
                lines.append(f"    buffer.{write_method}({member_name});")
            else:
                lines.append(f"    buffer.{write_method}({member_name});")
        lines.append("}")

    lines.append("")
    lines.append(f"template class {packet_name}<{version}>;")
    lines.append("")
    lines.append("} // namespace Axiom")

    return "\n".join(lines)


def generate_registry(schema: Dict, output_dir: Path):
    """Generate packet registry file."""

    version = schema["version"]

    lines = [
        "#pragma once",
        "",
        '#include "Axiom/Network/Packet/PacketRegistry.h"',
        "namespace Axiom {",
        "",
        f"// Protocol version {version}",
        f"inline void RegisterPackets_{version}(PacketRegistry& registry) {{",
    ]

    for category, packets in schema.items():
        if category == "version":
            continue
        for packet_name, packet_def in packets.items():
            direction = packet_def["direction"]
            base_class = (
                "ServerboundPacket"
                if direction == "Serverbound"
                else "ClientboundPacket"
            )
            lines.append(
                f"    registry.Register<{version}, {packet_name}<{version}>>();"
            )

    lines.append("}")
    lines.append("")
    lines.append("} // namespace Axiom")

    output_path = output_dir / f"PacketRegistry_{version}.h"
    output_path.write_text("\n".join(lines))
    print(f"Generated: {output_path}")


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <schema.yml> [output_dir]")
        sys.exit(1)

    schema_path = Path(sys.argv[1])
    output_dir = Path(sys.argv[2]) if len(sys.argv) > 2 else Path("generated")

    print(f"Reading schema from: {schema_path}")
    schema = yaml.safe_load(schema_path.read_text())
    version = schema["version"]

    print(f"Generating code for protocol version {version}...")

    # Create output directories
    header_dir = output_dir / "include" / "Axiom" / "Network" / "Packet" / str(version)
    impl_dir = output_dir / "src"
    header_dir.mkdir(parents=True, exist_ok=True)
    impl_dir.mkdir(parents=True, exist_ok=True)

    generated_count = 0

    for category, packets in schema.items():
        if category == "version":
            continue

        category_dir = header_dir / category
        category_impl_dir = impl_dir / category
        category_dir.mkdir(parents=True, exist_ok=True)
        category_impl_dir.mkdir(parents=True, exist_ok=True)

        for packet_name, packet_def in packets.items():
            # Generate header
            header_content = generate_packet_header(packet_name, packet_def, version)
            header_path = category_dir / f"{packet_name}.h"
            header_path.write_text(header_content)

            # Generate implementation
            impl_content = generate_packet_impl(packet_name, packet_def, version)
            impl_path = category_impl_dir / f"{packet_name}.cpp"
            impl_path.write_text(impl_content)

            generated_count += 1
            print(f"Generated: {packet_name}")

    # Generate registry
    generate_registry(schema, header_dir)

    print(f"\nSuccessfully generated {generated_count} packets!")
    print(f"Headers: {header_dir}")
    print(f"Implementations: {impl_dir}")


if __name__ == "__main__":
    main()
