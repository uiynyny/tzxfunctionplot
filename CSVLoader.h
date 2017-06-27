#ifndef CSVLoaderH
#define CSVLoaderH

#include <exception>
#include <string>
#include <iostream>
#include <vector>

/*
Class: CSVLoader

*/
class CSVLoader
{
public:
	typedef std::string string_type;
	
	class EConvertError : public std::exception
	{
	private:
		string_type msg;
	public:
		EConvertError(const string_type &m = "") throw() : msg(m)
		{}

		virtual ~EConvertError() throw()
		{}

		virtual const char *what() const throw() { return msg.c_str(); }
	};

    // Class: Field
	// Class for managing a single CSV field
	// Is stored as a string, and can be converted to integer or double
	// by using one of the methods.
	class Field
	{
	private:
		string_type m_value;
	public:
		Field();
		Field(const string_type &value);

		const string_type &AsString() const { return m_value; }
		int AsInt() const;
		double AsDouble() const;
		bool IsNumeric() const;

		// Conversion operators
		operator string_type() const { return m_value; }
		operator int() const { return AsInt(); }
		operator double() const { return AsDouble(); }
	};

	// Each row in the CSV will be represented as a Line
	typedef std::vector<Field> FieldList;

	// Class: Line
	// Represents a single line in the CSV file
	class Line
	{
	private:
		FieldList m_fields;
		
		// Line number inside the CSV file that corresponds to this line
		int m_fileLineNumber;

	public:
		Line() {}
		
		Line(int fileLineNumber) : m_fileLineNumber(fileLineNumber)
		{}

        int Size() const { return m_fields.size(); }

		Field &operator[](int index) { return m_fields[index]; }
		const Field &operator[](int index) const { return m_fields[index]; }

		FieldList &GetFields() { return m_fields; }
		const FieldList &GetFields() const { return m_fields; }

		int GetFileLineNumber() const { return m_fileLineNumber; }
		void SetFileLineNumber(int n) { m_fileLineNumber = n; }

		/*
		FieldList::iterator begin() { return m_fields.begin(); }
		FieldList::iterator end() { return m_fields.end(); }

		FieldList::const_iterator begin() const { return m_fields.begin(); }
		FieldList::const_iterator end() const { return m_fields.end(); }
		*/
		
		friend class CSVLoader;
	};

	// A Lines object represents the entire CSV file.
	typedef std::vector<Line> Lines;

private:
	bool m_commentsEnabled;
	char m_commentChar;
	Lines m_lines;
public:
	CSVLoader();
	virtual ~CSVLoader();

	// Method: ReadFromStream
	// Reads a CSV file from the given stream.
	void ReadFromStream(std::istream &is);

	// These methods provide access to the CSV data, once it has been
	// loaded.
	Lines &GetLines() { return m_lines; }
	const Lines &GetLines() const { return m_lines; }

	// Get the number of lines in the loaded CSV.
	int GetLineCount() const { return m_lines.size(); }

	char GetCommentChar() const { return m_commentChar; }
	void SetCommentChar(char c) { m_commentChar = c; }
	
	bool IsCommentsEnabled() const { return m_commentsEnabled; }
	void SetCommentsEnabled(bool flag) { m_commentsEnabled = flag; }
};

// Stream input operator for CSVLoader class
std::istream & operator >>(std::istream &is, CSVLoader &loader);

// Stream output operator
std::ostream & operator <<(std::ostream &os, CSVLoader::Field &field);

#endif
