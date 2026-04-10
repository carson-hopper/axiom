#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BREW_PREFIX="$([ -d /opt/homebrew ] && echo /opt/homebrew || echo /usr/local)"
export PATH="$BREW_PREFIX/bin:$PATH"
PREMAKE="$(command -v premake5 2>/dev/null || echo "$BREW_PREFIX/bin/premake5")"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

CONFIG="${CONFIG:-debug_26.1}"

usage() {
    echo -e "${BOLD}Usage:${RESET} $0 [--ide <type>] [--config <config>] [--clean]"
    echo ""
    echo "  --ide <type>    IDE to generate for: clion, xcode, vscode, vs, gmake"
    echo "  --config <cfg>  Build configuration (default: debug_26.1)"
    echo "  --clean         Clean build artifacts before building"
    echo "  -h, --help      Show this help message"
    echo ""
    echo "  If --ide is not provided, you will be prompted to choose."
    exit 0
}

IDE=""
CLEAN=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --ide)
            IDE="$(echo "$2" | tr '[:upper:]' '[:lower:]')"
            shift 2
            ;;
        --config)
            CONFIG="$2"
            shift 2
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        -h|--help)
            usage
            ;;
        *)
            echo -e "${RED}Unknown option: $1${RESET}"
            usage
            ;;
    esac
done

if ! command -v "$PREMAKE" &>/dev/null; then
    echo -e "${RED}premake5 not found.${RESET}"
    echo -e "Install with: ${CYAN}brew install premake${RESET}"
    exit 1
fi

if [[ -z "$IDE" ]]; then
    echo -e "${BOLD}Select your IDE:${RESET}"
    echo ""
    echo -e "  ${CYAN}1)${RESET} CLion"
    echo -e "  ${CYAN}2)${RESET} Xcode"
    echo -e "  ${CYAN}3)${RESET} VS Code"
    echo -e "  ${CYAN}4)${RESET} Visual Studio"
    echo -e "  ${CYAN}5)${RESET} Terminal / gmake only"
    echo ""
    read -rp "$(echo -e "${BOLD}> ${RESET}")" choice

    case "$choice" in
        1|clion)   IDE="clion"  ;;
        2|xcode)   IDE="xcode"  ;;
        3|vscode)  IDE="vscode" ;;
        4|vs)      IDE="vs"     ;;
        5|gmake)   IDE="gmake"  ;;
        *)
            echo -e "${RED}Invalid choice.${RESET}"
            exit 1
            ;;
    esac
fi

echo ""
echo -e "${BOLD}Setting up Axiom for ${CYAN}${IDE}${RESET}${BOLD}...${RESET}"
echo ""

# --- Step 1: Generate makefiles (all IDEs need this for building) ----------

echo -e "${YELLOW}[1/3]${RESET} Generating makefiles..."
cd "$ROOT_DIR"
"$PREMAKE" gmake 2>&1 | grep -v "^$"
echo -e "      ${GREEN}done${RESET}"

# --- Step 2: IDE-specific generation ---------------------------------------

case "$IDE" in
    xcode)
        echo -e "${YELLOW}[2/3]${RESET} Generating Xcode project..."
        "$PREMAKE" xcode4 2>&1 | grep -v "^$"
        echo -e "      ${GREEN}done${RESET}"
        ;;
    vs)
        echo -e "${YELLOW}[2/3]${RESET} Generating Visual Studio solution..."
        "$PREMAKE" vs2022 2>&1 | grep -v "^$"
        echo -e "      ${GREEN}done${RESET}"
        ;;
    vscode)
        VSCODE_MODULE="$ROOT_DIR/vendor/premake-vscode"
        if [[ ! -f "$VSCODE_MODULE/vscode.lua" ]]; then
            echo -e "${YELLOW}[2/3]${RESET} Downloading premake-vscode module..."
            git clone --depth 1 https://github.com/Enhex/premake-vscode.git "$VSCODE_MODULE" 2>&1 | tail -1
            echo -e "      ${GREEN}done${RESET}"
        fi

        echo -e "${YELLOW}[2/3]${RESET} Generating VS Code project..."
        "$PREMAKE" vscode 2>&1 | grep -v "^$"
        echo -e "      ${GREEN}launch.json + tasks.json generated${RESET}"

        echo -e "      Generating compile_commands.json..."

        if ! command -v bear &>/dev/null; then
            echo -e "      ${RED}bear not found.${RESET} Installing..."
            "$BREW_PREFIX/bin/brew" install bear 2>&1 | tail -3
        fi

        make -C "$ROOT_DIR" clean "config=$CONFIG" 2>/dev/null || true
        bear -- make -C "$ROOT_DIR" "config=$CONFIG" -j"$(sysctl -n hw.ncpu)" 2>&1 | tail -5
        echo -e "      ${GREEN}done${RESET} — $(wc -l < "$ROOT_DIR/compile_commands.json" | tr -d ' ') lines"
        ;;
    clion)
        echo -e "${YELLOW}[2/3]${RESET} Generating CLion run configurations..."
        "$PREMAKE" clion 2>&1 | grep -v "^$"
        echo -e "      ${GREEN}run/debug configs generated${RESET}"

        echo -e "      Generating compile_commands.json..."

        if ! command -v bear &>/dev/null; then
            echo -e "      ${RED}bear not found.${RESET} Installing..."
            "$BREW_PREFIX/bin/brew" install bear 2>&1 | tail -3
        fi

        make -C "$ROOT_DIR" clean "config=$CONFIG" 2>/dev/null || true
        bear -- make -C "$ROOT_DIR" "config=$CONFIG" -j"$(sysctl -n hw.ncpu)" 2>&1 | tail -5
        echo -e "      ${GREEN}done${RESET} — $(wc -l < "$ROOT_DIR/compile_commands.json" | tr -d ' ') lines"
        ;;
    gmake)
        echo -e "${YELLOW}[2/3]${RESET} Skipping IDE generation (gmake only)"
        ;;
esac

# --- Step 3: Build --------------------------------------------------------

if [[ "$IDE" != "clion" && "$IDE" != "vscode" && "$IDE" != "vs" ]]; then
    echo -e "${YELLOW}[3/3]${RESET} Building (config=${CYAN}${CONFIG}${RESET})..."

    if "$CLEAN"; then
        make -C "$ROOT_DIR" clean "config=$CONFIG" 2>/dev/null || true
    fi

    if make -C "$ROOT_DIR" "config=$CONFIG" -j"$(sysctl -n hw.ncpu)" 2>&1 | tail -5; then
        echo -e "      ${GREEN}Build succeeded${RESET}"
    else
        echo -e "      ${RED}Build failed${RESET}"
        exit 1
    fi
elif [[ "$IDE" == "vs" ]]; then
    echo -e "${YELLOW}[3/3]${RESET} Skipping build (use Visual Studio to build)"
else
    echo -e "${YELLOW}[3/3]${RESET} Build already completed during compile_commands.json generation"
fi

echo ""
echo -e "${GREEN}${BOLD}Setup complete!${RESET}"
echo ""

case "$IDE" in
    clion)
        echo -e "Open in CLion: ${CYAN}File > Open > ${ROOT_DIR}${RESET}"
        echo -e "CLion will detect ${CYAN}compile_commands.json${RESET} automatically."
        echo ""
        echo -e "To regenerate after file changes:"
        echo -e "  ${CYAN}./scripts/setup.sh --ide clion${RESET}"
        ;;
    xcode)
        echo -e "Open in Xcode: ${CYAN}open ${ROOT_DIR}/Axiom.xcworkspace${RESET}"
        ;;
    vs)
        echo -e "Open in Visual Studio: ${CYAN}${ROOT_DIR}/Axiom.sln${RESET}"
        echo -e "Build from VS or with: ${CYAN}make config=${CONFIG} -j\$(sysctl -n hw.ncpu)${RESET}"
        ;;
    vscode)
        echo -e "Open in VS Code: ${CYAN}code Axiom.code-workspace${RESET}"
        echo -e "Install the ${CYAN}clangd${RESET} and ${CYAN}CodeLLDB${RESET} extensions."
        echo -e "Use ${CYAN}Cmd+Shift+B${RESET} to build, ${CYAN}F5${RESET} to debug."
        ;;
    gmake)
        echo -e "Build with: ${CYAN}make config=${CONFIG} -j\$(sysctl -n hw.ncpu)${RESET}"
        ;;
esac
