/// @file IndentedStream.cpp
/// @brief
/// Copyright (c) 2024-2025, Electronic Systems Design (ESD) Group,
/// Univeristy of Verona.
/// This file is distributed under the BSD 2-Clause License.
/// See LICENSE.md for details.

#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <ostream>
#include <sstream>

#include "hif/backends/IndentedStream.hpp"

/// @brief Macro for casting buffer to IndentedBuffer.
#define buffer_cast(b) static_cast<IndentedBuffer *>(b)

namespace hif
{
namespace backends
{

namespace /* anon */
{

/// @brief This class is the buffer associated with IndentedStream.
/// The implementation just forwards all the action to the wrapped buffer stream.
///
class IndentedBuffer : public std::streambuf
{
public:
    /// @name Traits.
    ///@{

    /// @brief The string type.
    typedef IndentedStream::String String;

    /// @brief The size type.
    typedef IndentedStream::Size Size;

    /// @brief The stream type.
    typedef IndentedStream::OutStream OutStream;

    /// @brief The file stream type.
    typedef std::ofstream OutFileStream;

    /// @brief The string stream type.
    typedef std::stringstream StringStream;

    /// @brief The bool type.
    typedef IndentedStream::Bool Bool;

    typedef std::list<Bool> BoolList;

    ///@}

    /// @brief Constructor.
    IndentedBuffer(const String &fileName, const String &ext);

    /// @brief Constructor.
    ///
    /// @param buffer The stringbuffer which must be wrapped.
    ///
    explicit IndentedBuffer(std::stringbuf *buffer);

    /// @brief Constructor.
    ///
    /// @param buffer The streambuffer which must be wrapped.
    ///
    explicit IndentedBuffer(std::streambuf *buffer);
    virtual ~IndentedBuffer();

    /// @brief The stream name with extension.
    String getName() const;

    /// @brief Gets the name without extension.
    /// @return The current base name.
    const String &getBaseName() const;

    /// @brief Gets the extension.
    /// @return The current extension.
    const String &getExtension() const;

    /// @name Indentation related fields.
    /// @{

    /// @brief The indentation string.
    String indentationString;

    /// @brief The indentation level.
    Size indentation;

    /// @brief The maximum allowed column width.
    Size columnWidth;

    /// @brief Comment prefix string.
    String commentPrefix;

    /// @brief Comment infix string.
    String commentInfix;

    /// @brief Comment postfix string.
    String commentPostfix;

    /// @brief Macro-printing mode.
    Bool macroMode;

    /// @brief Macro-printing before newline.
    String macroNewLine;

    /// @brief Wrap-and-indent.
    Bool indentWhenWrapping;

    /// @brief Wrappping chars.
    String wrappingChars;

    /// @brief String mode.
    bool stringMode;

    /// @}

    /// @name File auto-split vars.
    /// @{

    Size maxLines;

    BoolList isBlockOpen;

    Bool isCommonBlock;

    /// @}

    void setCommentMode(bool b);
    bool getCommentMode() const;
    void openCommonTopBlock();
    void closeCommonTopBlock();
    void openCommonBottomBlock();
    void closeCommonBottomBlock();
    void checkNewFile();
    bool fileIsNew() const;

protected:
    /// @name Inherited virtual functions.
    ///@{

    virtual int overflow(int c);

    virtual int underflow();

    virtual int uflow();

    virtual int sync();

    //@}

    void _startNewFile();
    void _endCurrentFile();
    void _printIndentation();
    void _printComment(String &str);

    /// The common top stream.
    StringStream _commonTopStream;

    /// The common bottom stream.
    StringStream _commonBottomStream;

    /// The current file stream.
    OutStream *_currentStream;

    /// The actual file stream.
    OutStream *_actualStream;

    /// The wrapped buffer.
    std::streambuf *_buffer;

    /// @brief Flag to signal when it must be indented.
    Bool _mustBeIndented;

    /// @brief Comment-printing mode.
    Bool _commentMode;

    /// @brief Flag to signal when comment mode was already active.
    Bool _commentIsActive;

    /// @brief The current column.
    Size _column;

    /// @brief The current line.
    Size _lines;

    const String _fileName;

    const String _extension;

    /// @brief Tracks the current index for auto-split.
    Size _currentIndex;

    bool _isNewLine;

private:
    IndentedBuffer(const IndentedBuffer &);
    IndentedBuffer &operator=(IndentedBuffer &);
};
IndentedBuffer::IndentedBuffer(const String &fileName, const String &ext)
    : // Parent:
    std::streambuf()
    ,
    // Indentation related fields:
    indentationString("    ")
    , indentation(0)
    , columnWidth(80)
    , commentPrefix("// ")
    , commentInfix("// ")
    , commentPostfix("")
    , macroMode(false)
    , macroNewLine(" \\")
    , indentWhenWrapping(true)
    , wrappingChars(" \t{}()[]")
    , stringMode(false)
    ,
    // File auto-split vars.
    maxLines(0)
    , isBlockOpen()
    , isCommonBlock(false)
    ,
    // Protected fields:
    _commonTopStream()
    , _commonBottomStream()
    , _currentStream()
    , _actualStream()
    , _buffer(nullptr)
    , _mustBeIndented(true)
    , _commentMode(false)
    , _commentIsActive(false)
    , _column(1)
    , _lines(0)
    , _fileName(fileName)
    , _extension(ext)
    , _currentIndex(0)
    , _isNewLine(false)
{
    _currentStream = new OutFileStream(getName().c_str());
    _buffer        = _currentStream->rdbuf();
    _actualStream  = _currentStream;
}

IndentedBuffer::IndentedBuffer(std::stringbuf *buffer)
    : // Parent:
    std::streambuf()
    ,
    // Indentation related fields:
    indentationString("    ")
    , indentation(0)
    , columnWidth(80)
    , commentPrefix("// ")
    , commentInfix("// ")
    , commentPostfix("")
    , macroMode(false)
    , macroNewLine(" \\")
    , indentWhenWrapping(true)
    , wrappingChars(" \t{}()[]")
    , stringMode(false)
    ,
    // File auto-split vars.
    maxLines(0)
    , isBlockOpen()
    , isCommonBlock(false)
    ,
    // Protected fields:
    _commonTopStream()
    , _commonBottomStream()
    , _currentStream()
    , _actualStream()
    , _buffer(nullptr)
    , _mustBeIndented(true)
    , _commentMode(false)
    , _commentIsActive(false)
    , _column(1)
    , _lines(0)
    , _fileName("")
    , _extension("")
    , _currentIndex(0)
    , _isNewLine(false)
{
    _currentStream = nullptr;
    _buffer        = buffer;
    _actualStream  = _currentStream;
}

IndentedBuffer::IndentedBuffer(std::streambuf *buffer)
    : // Parent:
    std::streambuf()
    ,
    // Indentation related fields:
    indentationString("    ")
    , indentation(0)
    , columnWidth(80)
    , commentPrefix("// ")
    , commentInfix("// ")
    , commentPostfix("")
    , macroMode(false)
    , macroNewLine(" \\")
    , indentWhenWrapping(true)
    , wrappingChars(" \t{}()[]")
    , stringMode(false)
    ,
    // File auto-split vars.
    maxLines(0)
    , isBlockOpen()
    , isCommonBlock(false)
    ,
    // Protected fields:
    _commonTopStream()
    , _commonBottomStream()
    , _currentStream()
    , _actualStream()
    , _buffer(nullptr)
    , _mustBeIndented(true)
    , _commentMode(false)
    , _commentIsActive(false)
    , _column(1)
    , _lines(0)
    , _fileName("")
    , _extension("")
    , _currentIndex(0)
    , _isNewLine(false)
{
    _currentStream = nullptr;
    _buffer        = buffer;
    _actualStream  = _currentStream;
}

IndentedBuffer::~IndentedBuffer()
{
    _endCurrentFile();
    delete _currentStream;
}

IndentedBuffer::String IndentedBuffer::getName() const
{
    StringStream ss;
    String s;
    ss << _fileName;
    if (_currentIndex != 0)
        ss << "_" << _currentIndex;
    ss << "." << _extension;
    ss >> s;
    return s;
}

const IndentedBuffer::String &IndentedBuffer::getBaseName() const { return _fileName; }

const IndentedBuffer::String &IndentedBuffer::getExtension() const { return _extension; }

void IndentedBuffer::setCommentMode(bool b) { _commentMode = b; }

bool IndentedBuffer::getCommentMode() const { return _commentMode; }

void IndentedBuffer::openCommonTopBlock()
{
    if (isCommonBlock)
        return;
    isCommonBlock = true;

    _lines          = 0;
    _column         = 0;
    _mustBeIndented = true;

    _currentStream = &_commonTopStream;
    _buffer        = _commonTopStream.rdbuf();
}

void IndentedBuffer::closeCommonTopBlock()
{
    if (!isCommonBlock)
        return;
    isCommonBlock = false;

    _buffer = nullptr;

    _lines          = 0;
    _column         = 0;
    _mustBeIndented = true;

    _currentStream = _actualStream;
    _buffer        = _currentStream->rdbuf();
}

void IndentedBuffer::openCommonBottomBlock()
{
    if (isCommonBlock)
        return;
    isCommonBlock = true;

    _lines          = 0;
    _column         = 0;
    _mustBeIndented = true;

    _currentStream = &_commonBottomStream;
    _buffer        = _commonBottomStream.rdbuf();
}

void IndentedBuffer::closeCommonBottomBlock()
{
    if (!isCommonBlock)
        return;
    isCommonBlock = false;

    _buffer = nullptr;

    _lines          = 0;
    _column         = 0;
    _mustBeIndented = true;

    _currentStream = _actualStream;
    _buffer        = _currentStream->rdbuf();
}

void IndentedBuffer::checkNewFile()
{
    if (maxLines != 0 && _lines >= maxLines && isBlockOpen.empty() && !isCommonBlock) {
        _endCurrentFile();
        _startNewFile();
    }
}

bool IndentedBuffer::fileIsNew() const { return (_lines == 0 && _column == 0 && !isCommonBlock); }
// ///////////////////////////////////////////////////////////////////
// Inherited virtual functions.
// ///////////////////////////////////////////////////////////////////

int IndentedBuffer::overflow(int c)
{
    if (fileIsNew()) {
        _startNewFile();
    }

    if (_mustBeIndented && c != '\n') {
        _printIndentation();
        if (stringMode && c != '\"')
            _buffer->sputc('"');
        _mustBeIndented = false;
    }

    if (c == '\n') {
        _mustBeIndented = true;
        _column         = 0;
        if (!isCommonBlock)
            ++_lines;
    } else if (columnWidth != 0 && _column >= columnWidth && wrappingChars.find(static_cast<char>(c)) != String::npos) {
        if (stringMode)
            _buffer->sputc('"');
        overflow('\n');
        if (indentWhenWrapping && !_commentMode) {
            ++indentation;
            _printIndentation();
            if (stringMode)
                _buffer->sputc('"');
            --indentation;
            _mustBeIndented = false;
        }
    }

    checkNewFile();

    ++_column;

    // Comment management (new line or splitted comment).
    if (_commentMode && !_commentIsActive) {
        // starting comment block:
        _commentIsActive = true;
        _printComment(commentPrefix);
    } else if (!_commentMode && _commentIsActive) {
        // closing comment block:
        _commentIsActive = false;
        if (commentPostfix != "")
            _printComment(commentPostfix);
    } else if (_commentMode && _commentIsActive && commentInfix != "" && _isNewLine) {
        // comments prefix/infix already have spaces, so skip them:
        if (c == ' ')
            return 1;
        // printing comment infix
        _printComment(commentInfix);
    }

    if (macroMode && c == '\n') {
        _buffer->sputn(macroNewLine.c_str(), static_cast<std::streamsize>(macroNewLine.size()));
    }

    int ret = _buffer->sputc(std::streambuf::char_type(c));
    _isNewLine    = (c == '\n');

    return ret;
}
int IndentedBuffer::underflow()
{
    // Bad, but needed: underflow() is protected!
    return _buffer->sgetc();
}

int IndentedBuffer::uflow()
{
    // Bad, but needed: uflow() is protected!
    return _buffer->sbumpc();
}

int IndentedBuffer::sync()
{
    std::streambuf::sync();
    return _buffer->pubsync();
}
void IndentedBuffer::_startNewFile()
{
    _buffer = nullptr;
    delete _currentStream;

    _lines          = 0;
    _column         = 0;
    _mustBeIndented = true;

    _currentStream = new OutFileStream(getName().c_str());
    _buffer        = _currentStream->rdbuf();
    _actualStream  = _currentStream;

    if (_commonTopStream.str() != "")
        (*_currentStream) << _commonTopStream.str();
}

void IndentedBuffer::_endCurrentFile()
{
    if (!fileIsNew()) {
        if (_currentStream != nullptr)
            (*_currentStream) << _commonBottomStream.str();
        ++_currentIndex;
    }
}

void IndentedBuffer::_printIndentation()
{
    const Size len = indentationString.length();
    for (Size i = 0; i < indentation; ++i) {
        _column += len;

        if (static_cast<String::size_type>(
                _buffer->sputn(indentationString.c_str(), static_cast<std::streamsize>(len))) != len)
            return;
    }
}

void IndentedBuffer::_printComment(String &str)
{
    const Size len   = str.size();
    // _column += len;
    bool r1    = _commentIsActive;
    bool r2    = _commentMode;
    _commentIsActive = false;
    _commentMode     = false;
    sputn(str.c_str(), static_cast<std::streamsize>(len));
    //_buffer->sputn(str.c_str(), static_cast<std::streamsize>(len));
    _commentIsActive = r1;
    _commentMode     = r2;
}
} // end unnamed namespace
// //////////////////////////////////////////////////////////////////////////
// Constructors, destructor.
// //////////////////////////////////////////////////////////////////////////
IndentedStream::IndentedStream(const String &fileName, const String &extension)
    : // Parents:
    std::ostream(new IndentedBuffer(fileName, extension))
{
    // ntd
}

IndentedStream::IndentedStream(std::stringbuf *stringBuf)
    : // Parents:
    std::ostream(new IndentedBuffer(stringBuf))
{
    // ntd
}

IndentedStream::IndentedStream(std::streambuf *streamBuf)
    : // Parents:
    std::ostream(new IndentedBuffer(streamBuf))
{
    // ntd
}

IndentedStream::~IndentedStream() { delete rdbuf(); }
// //////////////////////////////////////////////////////////////////////////
// General accessors.
// //////////////////////////////////////////////////////////////////////////
const IndentedStream::String IndentedStream::getName() const { return buffer_cast(rdbuf())->getName(); }

const IndentedStream::String &IndentedStream::getBaseName() const { return buffer_cast(rdbuf())->getBaseName(); }

const IndentedStream::String &IndentedStream::getExtension() const { return buffer_cast(rdbuf())->getExtension(); }
// //////////////////////////////////////////////////////////////////////////
// Indentation related accessors and modifiers.
// //////////////////////////////////////////////////////////////////////////
void IndentedStream::setIndentationString(const String &s) { buffer_cast(rdbuf())->indentationString = s; }
IndentedStream::String IndentedStream::getIndentationString() const { return buffer_cast(rdbuf())->indentationString; }
void IndentedStream::setIndentation(const Size s) { buffer_cast(rdbuf())->indentation = s; }

IndentedStream::Size IndentedStream::getIndentation() const { return buffer_cast(rdbuf())->indentation; }

void IndentedStream::newLine(int n)
{
    for (int i = 0; i < n; ++i) {
        rdbuf()->sputc('\n');
    }
}

void IndentedStream::indent() { ++(buffer_cast(rdbuf())->indentation); }

void IndentedStream::unindent()
{
    if (buffer_cast(rdbuf())->indentation == 0)
        return;
    --(buffer_cast(rdbuf())->indentation);
}

void IndentedStream::setColumnWidth(const Size s) { buffer_cast(rdbuf())->columnWidth = s; }
IndentedStream::Size IndentedStream::getColumnWidth() const { return buffer_cast(rdbuf())->columnWidth; }
void IndentedStream::setCommentMode(const Bool r) { buffer_cast(rdbuf())->setCommentMode(r); }
IndentedStream::Bool IndentedStream::isCommentMode() const { return buffer_cast(rdbuf())->getCommentMode(); }

void IndentedStream::setComment(const String &prefix, const String &infix, const String &postfix)
{
    buffer_cast(rdbuf())->commentPrefix  = prefix;
    buffer_cast(rdbuf())->commentInfix   = infix;
    buffer_cast(rdbuf())->commentPostfix = postfix;
}
const IndentedStream::String &IndentedStream::getCommentPrefix() const { return buffer_cast(rdbuf())->commentPrefix; }
const IndentedStream::String &IndentedStream::getCommentInfix() const { return buffer_cast(rdbuf())->commentInfix; }
const IndentedStream::String &IndentedStream::getCommentPostfix() const { return buffer_cast(rdbuf())->commentPostfix; }

void IndentedStream::setMacroMode(const Bool r) { buffer_cast(rdbuf())->macroMode = r; }

IndentedStream::Bool IndentedStream::isMacroMode() const { return buffer_cast(rdbuf())->macroMode; }

const IndentedStream::String &IndentedStream::getMacroNewLine() const { return buffer_cast(rdbuf())->macroNewLine; }

void IndentedStream::setMacroNewLine(const IndentedStream::String &n) { buffer_cast(rdbuf())->macroNewLine = n; }

void IndentedStream::setIndentWhenWrapping(const Bool ind) { buffer_cast(rdbuf())->indentWhenWrapping = ind; }

IndentedStream::Bool IndentedStream::getIndentWhenWrapping() const { return buffer_cast(rdbuf())->indentWhenWrapping; }
void IndentedStream::setWrappingChars(const String &s) { buffer_cast(rdbuf())->wrappingChars = s; }

const IndentedStream::String &IndentedStream::getWrappingChars() const { return buffer_cast(rdbuf())->wrappingChars; }

void IndentedStream::setStringMode(bool isString) { buffer_cast(rdbuf())->stringMode = isString; }

bool IndentedStream::isStringMode() const { return buffer_cast(rdbuf())->stringMode; }
// //////////////////////////////////////////////////////////////////////////
// File auto-split methods.
// //////////////////////////////////////////////////////////////////////////
void IndentedStream::setMaxLines(const Size max) { buffer_cast(rdbuf())->maxLines = max; }

IndentedStream::Size IndentedStream::getMaxLines() { return buffer_cast(rdbuf())->maxLines; }

void IndentedStream::openBlock()
{
    IndentedBuffer *ib = buffer_cast(rdbuf());
    ib->checkNewFile();
    ib->isBlockOpen.push_back(true);
}

void IndentedStream::closeBlock() { buffer_cast(rdbuf())->isBlockOpen.pop_back(); }

IndentedStream::Bool IndentedStream::isBlockActive() { return !buffer_cast(rdbuf())->isBlockOpen.empty(); }

void IndentedStream::openCommonTopBlock() { buffer_cast(rdbuf())->openCommonTopBlock(); }

void IndentedStream::closeCommonTopBlock() { buffer_cast(rdbuf())->closeCommonTopBlock(); }

void IndentedStream::openCommonBottomBlock() { buffer_cast(rdbuf())->openCommonBottomBlock(); }

void IndentedStream::closeCommonBottomBlock() { buffer_cast(rdbuf())->closeCommonBottomBlock(); }

IndentedStream::Bool IndentedStream::isCommonBlockActive() { return buffer_cast(rdbuf())->isCommonBlock; }
// //////////////////////////////////////////////////////////////////////////
// Manipulators.
// //////////////////////////////////////////////////////////////////////////
IndentedStream::OutStream &indent(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->indent();

    return out;
}
IndentedStream::OutStream &unindent(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->unindent();

    return out;
}
IndentedStream::OutStream &indentWhenWrapping(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->setIndentWhenWrapping(true);

    return out;
}
IndentedStream::OutStream &noIndentWhenWrapping(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->setIndentWhenWrapping(false);

    return out;
}
IndentedStream::OutStream &openBlock(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->openBlock();

    return out;
}
IndentedStream::OutStream &closeBlock(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->closeBlock();

    return out;
}
IndentedStream::OutStream &openCommonBlock(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->openCommonTopBlock();

    return out;
}
IndentedStream::OutStream &closeCommonBlock(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->closeCommonTopBlock();

    return out;
}
IndentedStream::OutStream &openComment(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->setCommentMode(true);

    return out;
}
IndentedStream::OutStream &closeComment(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->setCommentMode(false);

    return out;
}
IndentedStream::OutStream &openString(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->setStringMode(true);

    return out;
}
IndentedStream::OutStream &closeString(IndentedStream::OutStream &out)
{
    IndentedStream *ind = dynamic_cast<IndentedStream *>(&out);
    if (ind == nullptr)
        return out;
    ind->setStringMode(false);

    return out;
}

} // namespace backends
} // namespace hif
