# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.4] - 2026-05-20

### Added
- Dictionary-style member access via `operator[]` for `TJValue`.
- Templated `as<T>()` helper method for simplified type casting (e.g., `json["key"].as<int>()`).

### Changed
- Incremented version to 0.2.4.

## [0.2.3] - 2026-05-19

### Added
- Atomic file saving: Implemented a safe save mechanism using temporary `-journal` files to prevent data loss or corruption during writes.
- Explicit `flush()` and `close()` checks for improved reliability during file operations.

### Changed
- Incremented version to 0.2.3.

## [0.2.2] - 2026-05-15

### Added
- Full support for JSON5 (v1.0.0).
- Support for single and multi-line comments in JSON5.
- Support for hexadecimal numbers, leading/trailing decimal points, and explicit plus signs.
- Support for `NaN`, `Infinity`, and `-Infinity`.
- Support for single-quoted strings and multi-line strings.
- Support for trailing commas in objects and arrays.

### Changed
- Incremented version to 0.2.2 to reflect major addition of JSON5 support.
- Split `TJJSON5Numbers` tests for better granularity.

### Fixed
- Fixed `TJValueArray::remove_at` to correctly handle arrays containing comments.

## [0.2.1]

### Added
- Added `remove_at` method to `TJValueArray`.

## [0.2.0]

### Changed
- **Breaking Change**: `get_*` methods no longer take `throw` parameters. Use `parse_options::strict` instead.

## [0.1.4]

### Added
- Added copy and move constructors and assignment operators to `TJValue` and derived classes.

## [0.1.3]

### Added
- Added iterator support for `TJValue`.

## [0.1.2]

### Added
- Added `get` and `set` methods for values and objects.

## [0.1.1]

### Added
- Added various `add(...)` and `set(...)` methods.

## [0.0.1]

### Added
- Initial release with basic RFC 8259 support.
