# Java Style Guide

**Based on Google Java Style • Record-Style Accessors**

A comprehensive, opinionated coding standard for Java projects. Derived from the Google Java Style Guide with key modifications: record-style accessor naming (no `get`/`set` prefixes), modern Java idioms, and practical enforcement guidance.

**Applies to:** Java 17+ projects | **Format tool:** `google-java-format` | **Lint:** Checkstyle (Google config)

---

## 1. Source File Basics

### 1.1 File Name

The file name is the case-sensitive name of the single top-level class it contains, plus the `.java` extension. One top-level class per file - always.

### 1.2 Encoding

All source files are encoded in **UTF-8**. No exceptions.

### 1.3 Whitespace & Special Characters

- Only ASCII horizontal space (`0x20`) is used for whitespace. **Tabs are never used** for indentation.
- Use special escape sequences (`\n`, `\t`, etc.) rather than octal or Unicode escapes.
- For non-ASCII characters, prefer the actual Unicode character when it improves readability. Add a comment if using a Unicode escape.

### 1.4 Source File Structure

Every source file appears in this order, with exactly one blank line between each present section:

1. License or copyright header (if present)
2. Package declaration (never line-wrapped)
3. Import statements
4. Exactly one top-level class

### 1.5 Imports

- **Always import each class explicitly.** Never use wildcard (`*`) imports, static or otherwise.
- Imports are not line-wrapped (column limit does not apply).
- Order: (1) all static imports in one block, (2) all non-static imports in one block. Separated by one blank line. Within each block, sorted in ASCII order.
- Do not use static import for nested classes - use normal imports instead.

---

## 2. Formatting

### 2.1 Braces

Braces are **always used** with `if`, `else`, `for`, `do`, and `while` - even for single-statement or empty bodies.

Follow Kernighan & Ritchie ("Egyptian brackets") style:

- No line break before the opening brace.
- Line break after the opening brace.
- Line break before the closing brace.
- Line break after the closing brace only if it terminates a statement, method, constructor, or named class.

```java
return new MyClass() {
    @Override public void method() {
        if (condition()) {
            try {
                something();
            } catch (ProblemException e) {
                recover();
            }
        } else if (otherCondition()) {
            somethingElse();
        } else {
            lastThing();
        }
    }
};
```

Empty blocks may be closed immediately: `void doNothing() {}` - unless they appear in a multi-block statement (e.g. `try/catch`).

### 2.2 Indentation

**+4 spaces** per block level. Never tabs.

### 2.3 One Statement Per Line

Each statement is followed by a line break.

### 2.4 Column Limit: 100

Lines must not exceed 100 characters. Exceptions: long URLs in Javadoc, `package`/`import` declarations, text blocks, and shell commands in comments.

### 2.5 Line Wrapping

- Prefer to break at a **higher syntactic level**.
- Break **before** non-assignment operators (including `.` and `::`).
- Break **after** assignment operators (including enhanced-for colon).
- A method/constructor name stays attached to its opening `(`.
- Commas stay attached to the token before them.
- Continuation lines are indented at least **+4 spaces** from the original line.

#### 2.5.1 Parameter Lists

**Keep parameters on one line if they fit within the 100-character column limit.** Do not split parameters across lines unless the line would exceed the limit. When parameters must wrap, align each parameter with the first parameter after the opening `(`.

**Exception: If any parameter has an annotation, always split parameters onto separate lines** - even if they would fit on one line. Annotations add visual noise, and splitting keeps each parameter and its annotation clearly distinct.

Good - no annotations, fits on one line:

```java
public static void onGameProfileRequest(String playerName, UUID profileId) {
```

Bad - no annotations, unnecessary split:

```java
public static void onGameProfileRequest(String playerName,
                                        UUID profileId) {
```

Good - annotation present, always split:

```java
public void teleportToPlayer(Player sender,
                             @Arg("source") Player target) {
```

Bad - annotation present, not split:

```java
public void teleportToPlayer(Player sender, @Arg("source") Player target) {
```

#### 2.5.2 Assignments and Constructor Calls

**Do not break assignments or constructor calls to a new line if they fit within the column limit.** The `=` and the right-hand side stay on the same line whenever possible.

Good - fits on one line:

```java
PlayerJoinEvent.Init event = new PlayerJoinEvent.Init(playerName);
```

Bad - unnecessary break after `=`:

```java
PlayerJoinEvent.Init event =
        new PlayerJoinEvent.Init(playerName);
```

### 2.6 Vertical Whitespace

A single blank line appears between consecutive class members (fields, methods, constructors, nested classes). Multiple consecutive blank lines are permitted but never required.

### 2.7 Horizontal Alignment

Horizontal alignment (adding extra spaces to line up tokens across lines) is **permitted but never required**. It tends to create maintenance burden and noisy diffs.

### 2.8 Grouping Parentheses

Optional grouping parentheses should be used when they improve clarity. Do not assume readers have memorized the full Java operator precedence table.

---

## 3. Naming Conventions

### 3.1 General Rules

Identifiers use only ASCII letters, digits, and (in certain cases) underscores. Every identifier should be descriptive and unambiguous. Avoid single-character names except for conventional loop variables (`i`, `j`, `k`) and lambdas where context is obvious.

### 3.2 Naming by Identifier Type

| Identifier | Convention | Example |
|---|---|---|
| Package | `all.lowercase.joined` | `com.example.deepspace` |
| Class / Interface / Record | UpperCamelCase (nouns) | `ImmutableList`, `HttpClient` |
| Enum class | UpperCamelCase (noun/adjective) | `ConnectionState` |
| Enum constant | CONSTANT_CASE | `READY`, `IN_PROGRESS` |
| Method | lowerCamelCase (verb/phrase) | `sendMessage`, `isReady` |
| Field (non-constant) | lowerCamelCase (noun) | `playerHealth`, `currentIndex` |
| Constant (`static final`) | CONSTANT_CASE | `MAX_RETRIES`, `DEFAULT_TIMEOUT` |
| Parameter | lowerCamelCase | `fileName`, `maxRetries` |
| Local variable | lowerCamelCase | `result`, `blockCount` |
| Type parameter | Single capital letter or T-suffix | `E`, `T`, `RequestT` |

### 3.3 CamelCase Conversion Rules

When converting English prose to CamelCase: split into words, lowercase each, then uppercase the first letter of each word (and the very first letter for UpperCamelCase). Acronyms are treated as single words: `HttpUrl`, not `HTTPURL`. `XmlParser`, not `XMLParser`.

---

## 4. Record-Style Accessors

> **This is the most significant departure from Google's standard guide.**

We adopt **record-style accessor naming** for all classes - not just records. Accessor methods are named after the field they expose, without `get`/`set` prefixes. Mutators (setters) also drop the `set` prefix.

> **Rationale:** Java records (JEP 395) use `name()`, not `getName()`. Adopting this pattern everywhere creates a uniform API surface, reduces noise, and aligns with how modern Java is heading. The `is` prefix is retained for booleans because it reads naturally.

### 4.1 Rules

- Accessor methods are named **exactly** as the field: `name()`, `health()`, `position()`.
- Mutator methods use the same name with a parameter: `name(String name)` or a fluent return.
- The `is` prefix is **retained** for boolean accessors: `isReady()`, `isConnected()`.
- Boolean mutators drop `set` but keep clarity: `ready(boolean ready)`.
- Builder-style / fluent APIs follow the same pattern naturally.

### 4.2 Comparison

| Traditional (JavaBeans) | This Guide (Record-Style) | Notes |
|---|---|---|
| ~~`getName()`~~ | `name()` | Drop the `get` prefix |
| ~~`setName(String n)`~~ | `name(String name)` | Drop the `set` prefix |
| ~~`getHealth()`~~ | `health()` | Same pattern for all types |
| ~~`setHealth(int hp)`~~ | `health(int health)` | Mutator mirrors accessor name |
| `isReady()` | `isReady()` | `is` prefix retained for booleans |
| ~~`setReady(boolean r)`~~ | `ready(boolean ready)` | Boolean mutator drops `set` |
| ~~`getItems()`~~ | `items()` | Collections follow the same rule |

### 4.3 Full Example

```java
public class Player {

  private String name;
  private int health;
  private boolean ready;
  private List<Item> inventory;

  // ── Accessors ──────────────────────────────
  public String name() { return name; }
  public int health() { return health; }
  public boolean isReady() { return ready; }
  public List<Item> inventory() {
    return Collections.unmodifiableList(inventory);
  }

  // ── Mutators ───────────────────────────────
  public void name(String name) { this.name = name; }
  public void health(int health) { this.health = health; }
  public void ready(boolean ready) { this.ready = ready; }
}
```

### 4.4 Interop with JavaBeans

When interacting with frameworks that require JavaBeans conventions (e.g., some serialization libraries, JSP EL), you may add traditional `get`/`set` methods as **aliases** delegating to the record-style methods. Mark them with `@Deprecated` or a comment indicating they exist only for framework compatibility.

```java
/** @deprecated Use {@link #name()} instead - exists for Jackson compat. */
@Deprecated
public String getName() { return name(); }
```

---

## 5. Specific Constructs

### 5.1 Variable Declarations

- One variable per declaration. No `int a, b;`.
- Declare local variables close to where they are first used.
- **Do not use `var`**. Always declare the explicit type. This keeps code readable and unambiguous at a glance.

### 5.2 Arrays

Always Java-style type brackets: `String[] args`, never `String args[]`.

### 5.3 Switch

- Prefer **new-style** switch expressions with arrows (`->`) over colon-based fall-through.
- Every switch must be **exhaustive** - include a `default` branch even if all enum values are covered.
- If using old-style switch, mark fall-through with `// fall through`.

```java
return switch (state) {
  case READY -> handleReady();
  case RUNNING -> handleRunning();
  case STOPPED -> handleStopped();
  default -> throw new IllegalStateException("Unexpected: " + state);
};
```

### 5.4 Annotations

- Class/method annotations go on their own line, before the declaration.
- A single parameterless annotation may share a line: `@Override public int hashCode() { ... }`.
- Field annotations may share a line: `@Nullable @Mock DataLoader loader;`.
- Type-use annotations appear immediately before the type: `final @Nullable String name;`.

### 5.5 Modifiers

When present, modifiers appear in the JLS-recommended order:

```
public protected private abstract default static final sealed non-sealed
transient volatile synchronized native strictfp
```

### 5.6 Numeric Literals

- `long` literals use uppercase `L`, never lowercase `l`.
- Use underscores for readability: `1_000_000`, `0xFF_EC_DE_5E`.

### 5.7 Records

Records already follow our accessor convention naturally. Prefer records for simple data carriers. Custom accessors in records still follow the field-name pattern (the compiler generates them this way).

```java
public record Position(int x, int y, int z) {

  public double distanceTo(Position other) {
    int dx = x - other.x();
    int dy = y - other.y();
    int dz = z - other.z();
    return Math.sqrt(dx * dx + dy * dy + dz * dz);
  }
}
```

---

## 6. Javadoc

### 6.1 Where Required

Javadoc is required on every `public` class, interface, enum, record, and every `public` or `protected` member - with the following exceptions: self-explanatory simple methods (e.g., `name()` on a `Player`), overrides (unless adding info), and test methods.

### 6.2 Format

```java
/**
 * Sends a message to the specified player.
 *
 * <p>The message is delivered asynchronously. If the player is offline,
 * it is queued until their next login.
 *
 * @param player  the target player (must not be null)
 * @param message the message content
 * @return true if the message was delivered immediately
 */
public boolean sendMessage(Player player, String message) { ... }
```

- Opening `/**` and closing `*/` on their own lines (or single-line for very short docs).
- First sentence is a concise summary; it stands alone as the short description.
- Use `<p>` to start new paragraphs.
- Block tags appear in order: `@param`, `@return`, `@throws`, `@since`, `@deprecated`.

### 6.3 Single-Line Form

```java
/** Returns the player's current health. */
public int health() { return health; }
```

### 6.4 TODO Comments

Use `TODO:` followed by a bug/issue link and a dash-separated explanation. Never use a person's name as the context.

```java
// TODO: gh-123 - Remove after the v2 migration is complete.
```

---

## 7. Programming Practices

### 7.1 @Override

Always use `@Override` when a method overrides a superclass method, implements an interface method, or overrides a superinterface default method. The only exception: when the parent method is `@Deprecated`.

### 7.2 Caught Exceptions

Never silently swallow exceptions. At minimum, log a warning or rethrow. If ignoring is truly justified, use `_` as the catch variable name and add a comment explaining why.

```java
try {
int parsed = Integer.parseInt(input);
} catch (NumberFormatException _) {
        // Expected for user-typed input; fallback to default.
        }
```

### 7.3 Static Members

Qualify static member access by the class name, not a variable reference: `Foo.staticMethod()`, never `fooInstance.staticMethod()`.

### 7.4 Finalizers

Never use `finalize()`. Use try-with-resources or `Cleaner` instead.

### 7.5 Immutability

- Prefer `final` fields and local variables where practical.
- Return **unmodifiable views** of collections from accessors (see Section 4.3 example).
- Prefer records for value types.

### 7.6 Null Safety

- Annotate nullable parameters and return types with `@Nullable`.
- Prefer `Optional<T>` for return types that may be absent.
- Never pass `null` where a non-null contract exists.
- Consider using `@NullMarked` at the package or module level (JSpecify).

### 7.7 Modern Java Idioms

- Use text blocks (`"""`) for multi-line strings.
- Use pattern matching for `instanceof`: `if (obj instanceof String s)`.
- Use sealed classes/interfaces where a fixed set of subtypes is intended.
- Prefer `switch` expressions over `if/else` chains when mapping values.

---

## 8. Enforcement

### 8.1 Formatter

Use `google-java-format` to auto-format all code. Integrate it as a pre-commit hook or CI step. There is no configurability by design - this eliminates style debates.

```bash
java -jar google-java-format.jar --replace src/**/*.java
```

### 8.2 IDE Setup

- **IntelliJ:** Install the google-java-format plugin. Enable it in Settings → google-java-format Settings.
- **Eclipse:** Import the `eclipse-java-google-style.xml` under Preferences → Java → Code Style → Formatter.
- Enable format-on-save in your IDE to enforce automatically.

### 8.3 Checkstyle

Use the Google Checkstyle configuration as a baseline, then add custom checks for the accessor naming convention (no `get`/`set` prefixes). Run Checkstyle in CI to catch violations before merge.

### 8.4 Code Review

Style enforcement in code review should be minimal - the formatter and linter should catch most issues. Reserve review comments for naming quality, documentation clarity, and API design decisions (like whether an accessor should exist at all).