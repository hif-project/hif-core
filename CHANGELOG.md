# Changelog

All notable changes to this project are documented in this file.

## [Unreleased]

## [1.1.0] - 2026-08-13

- Implemented `VerilogAnalysis::map(Int*, Int*)`, closing a typing-rule gap that made any binary expression combining two `Int`-typed operands fail to type under Verilog semantics.
- Registered `$clog2` in Verilog's standard library (`VerilogSemantics::getStandardPackage()` had only a placeholder comment for it, unlike `$time`/`$stime`/`$random`).
- Migrated the project to the `hif-project` GitHub organization; updated internal references accordingly.
- Replaced the README's ecosystem-navigation list with a link to the organization profile.

## [1.0.0] - 2026-08-12

Initial coordinated release of the HIF toolchain baseline (hif-core, hif-frontend, hif-backend, hif-muffin, all tagged v1.0.0).

- Fixed `XmlParser::_addCodeInfoPropertiesComments()` overwriting every read object's line number with its column number on XML read.
- Consolidated CI to a single Linux workflow (gcc + clang), replacing an unreliable 12-way OS/compiler matrix.
- Added a proper README describing the toolchain and how to build and run tests.
