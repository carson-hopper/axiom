#!/bin/sh
# Increments .buildcount and generates BuildCount.generated.h
# Resets to 0 on each new commit
COUNTFILE="$1/.buildcount"
COMMITFILE="$1/.buildcount-commit"
HEADER="$2/src/Axiom/Core/BuildCount.generated.h"

# Check if HEAD changed since last build
CURRENT_COMMIT=$(git -C "$1" rev-parse HEAD 2>/dev/null || echo "none")
LAST_COMMIT=$(cat "$COMMITFILE" 2>/dev/null || echo "")

if [ "$CURRENT_COMMIT" != "$LAST_COMMIT" ]; then
    echo 0 > "$COUNTFILE"
    echo "$CURRENT_COMMIT" > "$COMMITFILE"
fi

COUNT=$(cat "$COUNTFILE" 2>/dev/null || echo 0)
COUNT=$((COUNT + 1))
echo "$COUNT" > "$COUNTFILE"

echo "// Auto-generated — do not edit" > "$HEADER"
echo "#pragma once" >> "$HEADER"
echo "#define AX_BUILD_COUNT $COUNT" >> "$HEADER"
