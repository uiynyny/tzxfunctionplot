#include "CSVLoader.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>

//##############################################################################
// Implementation of: CSVLoader::Field
//##############################################################################

//------------------------------------------------------------------------------
CSVLoader::Field::Field()
{

}

//------------------------------------------------------------------------------
CSVLoader::Field::Field(const string_type &value) : m_value(value)
{

}

//------------------------------------------------------------------------------
int CSVLoader::Field::AsInt() const
{
    // Re-use the float point conversion code, so that we can
    // treat integers as doubles if the user wants to.
    try
    {
        return AsDouble();
    }
    catch (...)
    {
		throw EConvertError("Cannot convert \"" + m_value + "\" to integer");
    }
}

//------------------------------------------------------------------------------
double CSVLoader::Field::AsDouble() const
{
	char *end;
	const char *s = m_value.c_str();
	double rv = std::strtod(s, &end);

	// Check if an error occurred
	if (end == s || end != s + m_value.length())
	{
		throw EConvertError("Cannot convert \"" + m_value + "\" to double");
	}

	return rv;
}

//------------------------------------------------------------------------------
bool CSVLoader::Field::IsNumeric() const
{
    // Silly way to check if this field contains a number.
    // Could probably make this more efficient.
    try
    {
        AsDouble();
        return true;
    }
    catch (...)
    {
        // If AsDouble() fails to convert to a number, it will throw an
        // exception and we will end up here.
        return false;
    }
}

//##############################################################################
// Implementation of: CSVLoader
//##############################################################################

//------------------------------------------------------------------------------
CSVLoader::CSVLoader() : m_commentsEnabled(false), m_commentChar('#')
{

}

//------------------------------------------------------------------------------
CSVLoader::~CSVLoader()
{

}

//------------------------------------------------------------------------------
void CSVLoader::ReadFromStream(std::istream &is)
{
	typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

	m_lines.clear();   // Delete existing CSV data

	std::string text;
	while (!is.eof() && !is.fail())
	{
		std::getline(is, text);  // read a line from the file
		boost::trim(text);

		// Strip comments if enabled
		if (IsCommentsEnabled())
		{
			size_t comment = text.find_first_of(m_commentChar);
			if (comment != std::string::npos)
				text.resize(comment);
		}

		// Skip the line if it is blank after trimming white
		// space and comments
		if (text.length() == 0)
			continue;

		Tokenizer tok(text);     // tokenize that shit

		m_lines.push_back(Line());
		Line &line = m_lines.back();    // access the last element

		// Split the line into fields, and store the fields in a Line vector
		for (Tokenizer::iterator i = tok.begin(); i != tok.end(); ++i)
		{
			std::string field = *i;
			boost::trim(field);
			line.m_fields.push_back(Field(field.c_str()));
		}
	}

	if (is.fail())
	{
		// TODO: report error
    }
}

//------------------------------------------------------------------------------
std::istream & operator >>(std::istream &is, CSVLoader &loader)
{
	loader.ReadFromStream(is);
	return is;
}

//------------------------------------------------------------------------------
std::ostream & operator <<(std::ostream &os, CSVLoader::Field &field)
{
    os << field.AsString();
    return os;
}

