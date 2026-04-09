#!/bin/sh
# Increments .buildcount and generates BuildCount.generated.h
COUNTFILE="$1/.buildcount"
HEADER="$2/src/Axiom/Core/BuildCount.generated.h"

COUNT=$(cat "$COUNTFILE" 2>/dev/null || echo 0)
COUNT=$((COUNT + 1))
echo "$COUNT" > "$COUNTFILE"

echo "// Auto-generated — do not edit" > "$HEADER"
echo "#pragma once" >> "$HEADER"
echo "#define AX_BUILD_COUNT $COUNT" >> "$HEADER"
