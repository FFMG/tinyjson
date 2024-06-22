# Various JSON Specifications

## Key Changes from RFC 4627 to RFC 7159

### JSON Text Flexibility

- RFC 4627: A JSON text must be either an object or an array.

- RFC 7159: A JSON text can be any JSON value, including objects, arrays, strings, numbers, booleans, or null.

### Unicode Handling

- RFC 4627: Was less explicit about Unicode handling, particularly for surrogate pairs.

- RFC 7159: Clarified the handling of Unicode, especially with regards to surrogate pairs, ensuring proper representation and validation.

### Number Precision

- RFC 4627: Did not explicitly state limitations on number precision.

- RFC 7159: Emphasized that implementations should not expect to process numbers exactly as received if they fall outside of what can be represented by a double-precision floating-point format (IEEE 754).

### White Space Handling

- RFC 4627: Allowed a limited set of whitespace characters (space, tab, newline, carriage return).

- RFC 7159: Allowed these same characters but provided more clarity on their usage.

## Key Changes from RFC 7159 to RFC 8259

### Clarifications and Editorial Improvements

- RFC 8259: Incorporated various editorial improvements and clarifications to enhance readability and precision without altering the technical content.

### Consistent Terminology

- RFC 8259: Ensured consistent terminology and definitions throughout the document.

### No Major Technical Changes

- RFC 8259: Did not introduce major technical changes but focused on consolidating and clarifying the changes introduced in RFC 7159.

## Summary of Key Differences

### RFC 4627 (2006)

- JSON text must be an object or array.
- Less explicit about Unicode handling.
- No explicit precision constraints on numbers.

### RFC 7159 (2014)

- JSON text can be any JSON value.
- Clarified Unicode handling, especially surrogate pairs.
- Advised on number precision and representation.
- Provided clearer guidance on allowed whitespace.

### RFC 8259 (2017)

- Incorporated editorial improvements from RFC 7159.
- Ensured consistent terminology and definitions.
- No major technical changes from RFC 7159.
