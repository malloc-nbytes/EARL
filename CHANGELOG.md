# Change Log
All notable changes to this project will be documented in this file.

## [0.1.4] - 2024-11-8

### Added
- Added intrinsic casting functions

### Fixed
- Fixed file handler mode type to except multiple types
- The appropriate unit functions now return a unit type instead of null.

## [0.1.3] - 2024-10-8

### Added
- MAJOR New Tuple type

## [0.1.2] - 2024-09-8

### Changes
- MAJOR `foreach` loops now take an expression (list/range) as the argument.
- MAJOR in `foreach` loops, the enumerator can now have attributes.
- MAJOR `<list>.rev()` does not reverse the list in-place anymore. It now produces a new list with the elements reversed.
- MAJOR changed syntax for the regular `for` loops.

### Added
- Syntax sugar for lists in the form of a range ie [0..10] (range is from 0-9) or ['a'..'z'].
- Member variables and methods in classes now adhere to the @pub and private system

## [0.1.1] - 2024-08-8

### Changes
- Renamed keyword `mod` -> `module`

### Added
- Enums
- Float type
- Unary Expressions
- Better output when printing enumerations
- Import depth
- Enum type security
- Better error messages for type mismatchs

### Fixed

- MINOR Fixed segfault bug when creating a float without a left number ie `.34`.
- MAJOR Fixed integer+float bug