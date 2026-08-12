/// @file code_info_source_line_test.cpp
/// @brief
/// Copyright (c) 2026, Electronic Systems Design (ESD) Group,
/// University of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.
///
/// Regression for a known bug in XmlParser::_addCodeInfoPropertiesComments():
/// after correctly reading a node's CODE_INFO (file/line/column) from XML, it
/// applies the line number twice -- once with the real line number, once
/// (bug) with the column number, via a copy-pasted setSourceLineNumber() call
/// that should have been setSourceColumnNumber(). The column value silently
/// overwrites the correct line number, and the real column is never applied
/// at all (so getSourceColumnNumber() is always 0 on anything read back from
/// XML).
///
/// fixtures/and2_known_good.hif.xml is a real verilog2hif-produced HIF file
/// (paths stripped/relativized) for `assign y = a & b;` on and2.v's line 2,
/// column 13 -- values independently confirmed correct in the raw XML on
/// disk before this test was written. This test reads that file back and
/// checks the resulting Assign node's line/column survive the round trip.

#include <hif/hif.hpp>
#include <iostream>

using namespace hif;

namespace
{

class FindAssignVisitor : public GuideVisitor
{
public:
    Assign *found = nullptr;

    int visitAssign(Assign &o) override
    {
        found = &o;
        return GuideVisitor::visitAssign(o);
    }
};

} // namespace

auto main(int argc, char *argv[]) -> int
{
    if (argc != 2) {
        std::cerr << "usage: code_info_source_line_test <fixture.hif.xml>\n";
        return 2;
    }

    auto *system = dynamic_cast<System *>(hif::readFile(argv[1]));
    if (system == nullptr) {
        std::cerr << "FAIL: could not read fixture: " << argv[1] << "\n";
        return 1;
    }

    FindAssignVisitor visitor;
    system->acceptVisitor(visitor);

    if (visitor.found == nullptr) {
        std::cerr << "FAIL: fixture has no Assign node\n";
        delete system;
        return 1;
    }

    const unsigned int expectedLine   = 2;
    const unsigned int expectedColumn = 13;
    const unsigned int actualLine     = visitor.found->getSourceLineNumber();
    const unsigned int actualColumn   = visitor.found->getSourceColumnNumber();

    bool ok = true;
    if (actualLine != expectedLine) {
        std::cerr << "FAIL: expected line " << expectedLine << ", got " << actualLine << "\n";
        ok = false;
    }
    if (actualColumn != expectedColumn) {
        std::cerr << "FAIL: expected column " << expectedColumn << ", got " << actualColumn << "\n";
        ok = false;
    }

    delete system;
    return ok ? 0 : 1;
}
