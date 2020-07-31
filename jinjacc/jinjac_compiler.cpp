#include "jinjac_compiler.h"
#include "flex_decl.h"
#include <iostream>
#include <endian.h>
#include "asttext.h"

JinjacCompiler::JinjacCompiler(std::ifstream* in, std::ofstream* out)
{
  ASSERT(in != nullptr);
  ASSERT(out != nullptr);

  m_inFile = in;
  m_outFile = out;
  m_inError = false;
  m_inJinjaStatement = false;
  m_root = new Ast();
}

JinjacCompiler::~JinjacCompiler()
{
  if (m_root)
  {
    delete m_root;
  }
}

J_STATUS JinjacCompiler::doCompile()
{
  J_STATUS status;
  status = J_ERROR;

  parseInputFile();

  return status;
}

typedef enum
{
  IN_TEXT,
  DETECTION_START_DELIMITER,
  IN_JINJA_SCRIPT_STRING,
  IN_JINJA_COMMENT,
  IN_JINJA_COMMENT_STOP_DELIMITER,
  DETECTION_STOP_DELIMITER,
  IN_JINJA_STATEMENT,
  IN_JINJA_EXPRESSION
} parse_file_state;


void JinjacCompiler::parseInputFile()
{
  char previous;
  char current;
  char stringChar;
  bool bInError;
  parse_file_state parsingState;
  parse_file_state previousState;
  std::string currentText;
  std::string script;

  previousState = IN_TEXT;
  parsingState = IN_TEXT;

  bInError = false;
  m_noLine = 1;
  previous = '\0';

  while (m_inFile->get(current) && (!bInError))
  {
    if (current == '\n')
    {
      m_noLine++;
      if (parsingState != IN_JINJA_COMMENT)
      {
        currentText += current;
      }
    }
    else
    {
      switch (parsingState)
      {
        case IN_TEXT:
          if (current == '{')
          {
            parsingState = DETECTION_START_DELIMITER;
          }
          else
          {
            currentText += current;
          }
          break;

        case DETECTION_START_DELIMITER:
          switch (current)
          {
            case '%':
              parsingState = IN_JINJA_STATEMENT;
              {
                insertAstTextNode(currentText);
                currentText.clear();
              }

              break;

            case '{':
              parsingState = IN_JINJA_EXPRESSION;
              {
                insertAstTextNode(currentText);
                currentText.clear();
              }
              break;

            case '#':
              parsingState = IN_JINJA_COMMENT;
              break;

            default:
              parsingState = IN_TEXT;
              currentText += previous;
              currentText += current;
              break;
          }
          break;

        case IN_JINJA_COMMENT:
          if (current == '#')
          {
            parsingState = IN_JINJA_COMMENT_STOP_DELIMITER;
          }
          break;

        case IN_JINJA_COMMENT_STOP_DELIMITER:
          if (current == '}')
          {
            parsingState = IN_TEXT;
          }
          else
          {
            parsingState = IN_JINJA_COMMENT;
          }
          break;

        case IN_JINJA_STATEMENT:
          switch (current)
          {
            case '%':
              previousState = IN_JINJA_STATEMENT;
              parsingState = DETECTION_STOP_DELIMITER;
              break;

            case '\'':
            case '\"':
              stringChar = current;
              script += current;
              previousState = IN_JINJA_STATEMENT;
              parsingState = IN_JINJA_SCRIPT_STRING;
              break;

            default:
              // no change of state
              script += current;
              break;
          }

          break;

        case IN_JINJA_EXPRESSION:
          switch (current)
          {
            case '}':
              previousState = IN_JINJA_EXPRESSION;
              parsingState = DETECTION_STOP_DELIMITER;
              break;

            case '\'':
            case '\"':
              stringChar = current;
              script += current;
              previousState = IN_JINJA_EXPRESSION;
              parsingState = IN_JINJA_SCRIPT_STRING;
              break;

            default:
              // no change of state
              script += current;
              break;
          }
          break;

        case IN_JINJA_SCRIPT_STRING:
          if (current == stringChar)
          {
            parsingState = previousState;
          }

          script += current;
          break;

        case DETECTION_STOP_DELIMITER:
          if (current == '}')
          {
            m_inJinjaStatement = (previousState == IN_JINJA_STATEMENT) ? true : false;
            //launch parsing
            bInError = parseJinjaLang(script);
            parsingState = IN_TEXT;
            script.clear();
          }
          else
          {
            script += previous;
            script += current;
            parsingState = previousState;
          }
          break;

        default:
          ASSERT(false);
          break;
      }
    }
    previous = current;
  }

  insertAstTextNode(currentText);

  bool verbose = true;
  if (verbose)
  {
    m_root->print();
  }

  m_root->printAsm(*this);
  buildOutputFile();
}

void JinjacCompiler::insertAstTextNode(std::string& text)
{
  if (text != "")
  {
    AstText* node = new AstText(text);
    m_root->insert(node);
  }
}

bool JinjacCompiler::parseJinjaLang(std::string& scriptText)
{
  YY_BUFFER_STATE buffer;
  m_bStripBeginOfBlock = false;
  m_bStripEndOfBlock = false;

  dbg_print("script : '%s'\n", scriptText.c_str());

  if (scriptText.front() == '-')
  {
    //dbg_print("m_bStripEndOfBlock= true\n");
    m_bStripEndOfBlock = true;
    scriptText[0] = ' ';
  }

  if (scriptText.back() == '-')
  {
    //dbg_print("m_bStripBeginOfBlock = true\n");
    m_bStripBeginOfBlock = true;
    scriptText[scriptText.size() - 1] = ' ';
  }

  buffer = yy_scan_string(scriptText.c_str());
  yyparse();

  if (m_inError == true)
  {
    std::cerr << "error while parsing: " << m_errorDetails << std::endl;
  }

  yy_delete_buffer(buffer);
  yylex_destroy();

  return m_inError;
}

void JinjacCompiler::buildOutputFile()
{
  buildHeader();
  m_staticData.write(m_outFile);
  m_code.write(m_outFile);
}

void JinjacCompiler::buildHeader()
{
  uint16_t version;
  uint16_t pad = 0;
  uint32_t dataAbsOffset;
  uint32_t dataSize;
  uint32_t codeAbsOffset;
  uint32_t codeSize;

  m_outFile->write("JOBJ", 4);

  version = htobe16(0x0100);
  m_outFile->write((const char*) &version, sizeof(uint16_t));

  pad = htobe16(pad);
  m_outFile->write((const char*) &pad, sizeof(uint16_t));

  dataAbsOffset = 24;
  dataAbsOffset = htobe32(dataAbsOffset);
  m_outFile->write((const char*) &dataAbsOffset, sizeof(uint32_t));

  dataSize = m_staticData.getSize();
  dataSize = htobe32(dataSize);
  m_outFile->write((const char*) &dataSize, sizeof(uint32_t));

  codeAbsOffset = 24 + m_staticData.getSize();
  codeAbsOffset = htobe32(codeAbsOffset);
  m_outFile->write((const char*) &codeAbsOffset, sizeof(uint32_t));

  codeSize = m_code.getSize();
  codeSize = htobe32(codeSize);
  m_outFile->write((const char*) &codeSize, sizeof(uint32_t));

}

